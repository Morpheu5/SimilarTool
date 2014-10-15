#include <memory>

#include "SimilarToolApp.h"

#include "Trace.h"
#include "Widget.h"
#include "PlayerWidget.h"

#include "cinder/Xml.h"

#include "Model.h"
#include "Logger.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void SimilarToolApp::prepareSettings(Settings *settings) {
	settings->enableHighDensityDisplay();
	settings->setFullScreen();
}

void SimilarToolApp::setup() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	_tuioClient.registerCursorAdded(this, &SimilarToolApp::cursorAdded);
	_tuioClient.registerCursorUpdated(this, &SimilarToolApp::cursorUpdated);
	_tuioClient.registerCursorRemoved(this, &SimilarToolApp::cursorRemoved);
	
	_tuioClient.connect(); // Defaults to UDP:3333
	
	_tracesProcessorShouldRun = true;
	_tracesProcessor = thread(bind(&SimilarToolApp::processTraces, this));
	
#if DEBUG==1
	auto basepath = ci::fs::path("/Users/morpheu5/src/SimilarTool");
#else
	auto basepath = getAppPath().remove_filename();
#endif
	
	auto logdir = basepath/"logs";
	if(!fs::exists(logdir)) {
		fs::create_directories(logdir);
	}
	std::time_t t = std::time(nullptr);
	stringstream fn;
	fn << "SimilarTool_" << std::put_time(std::localtime(&t), "%Y-%m-%d_%H-%M-%S") << ".log";
	logdir /= fn.str();
	Logger::instance().init(logdir.string());
	Logger::instance().log("SimilarTool starting up...");
	_loggerThread = thread(bind(&Logger::run, &(Logger::instance())));
	
	Logger::instance().log(stringstream() << "Window size: " << getWindowSize());
	
	auto filepath = basepath/"assets/index.xml";
	XmlTree doc(loadFile(filepath));
	XmlTree xTriplets = doc.getChild("root");
	
	for(auto xTriplet : xTriplets) {
		Triplet triplet(xTriplet.getAttributeValue<std::string>("params"));
		for(auto xItem : xTriplet) {
			Item item(xItem.getAttributeValue<unsigned long>("type"), xItem.getValue());
			triplet.items.push_back(item);
		}
		_triplets.push_back(triplet);
	}
	_currentTriplet = -1;
}

void SimilarToolApp::shutdown() {
	_tracesProcessorShouldRun = false;
	_tracesProcessor.join();
	Logger::instance().stop();
	_loggerThread.join();
}

void SimilarToolApp::keyUp(KeyEvent event) {
	switch(event.getCode()) {
		case KeyEvent::KEY_RETURN: {
			_currentTriplet++;
			// If this is past the first round, record data and clear the board
			if(_currentTriplet > 0 && _currentTriplet <= _triplets.size()) {
				_widgetsMutex.lock();
				stringstream ss;
				ss << "End of round " << _currentTriplet << " -- params: " << _triplets[_currentTriplet-1].params << endl;
				for(auto w : _widgets) {
					if(auto player = dynamic_pointer_cast<PlayerWidget>(w.second)) {
						ss << "type: " << player->type << " -- filename: " << player->filename << " -- position: " << player->position << endl;
					}
				}
				Logger::instance().log(ss.str());
				_widgets.clear();
				_widgetsMutex.unlock();
			}
			
			// Then prepare the next round...
			Logger::instance().log(stringstream() << "Begin round " << _currentTriplet+1);
			Vec2f c = getWindowCenter();
			
			// ... but only if there is one to be prepared
			if(_currentTriplet < _triplets.size()) {
				Triplet t = _triplets[_currentTriplet];
				int d = 150;
				_widgetsMutex.lock();
				for(int i = 0; i < 3; i++) {
					int p = -d + d*i;
					shared_ptr<PlayerWidget> player = make_shared<PlayerWidget>("assets/" + t.items[i].filename);
					player->position = Vec2f(c.x + p, c.y);
					player->type = t.items[i].type;
					cout << player->type << endl;
					_widgets[player->id] = player;
				}
				_widgetsMutex.unlock();
			} else {
				_widgetsMutex.lock();
				_widgets.clear();
				_widgetsMutex.unlock();
			}
			break;
		}
	}
}

void SimilarToolApp::mouseDown(MouseEvent event) {
	Vec2f p(event.getPos());
	_tracesMutex.lock();
	_traces[0] = make_shared<Trace>();
	_widgetsMutex.lock();
	for(auto const &item : _widgets) {
		shared_ptr<Widget> widget = item.second;
		if(widget->hit(p)) {
			_traces[0]->widgetId = widget->id;
		}
	}
	_widgetsMutex.unlock();
	_traces[0]->touchDown(p);
	_tracesMutex.unlock();
}

void SimilarToolApp::mouseDrag(MouseEvent event) {
	Vec2f p(event.getPos());
	_tracesMutex.lock();
	_traces[0]->touchMove(p);
	_tracesMutex.unlock();
}

void SimilarToolApp::mouseUp(MouseEvent event) {
	Vec2f p(event.getPos());
	_tracesMutex.lock();
	_traces[0]->touchUp(p);
	_tracesMutex.unlock();
}

void SimilarToolApp::cursorAdded(tuio::Cursor cursor) {
	Vec2f p(Vec2f(getWindowSize()) * cursor.getPos());
	_tracesMutex.lock();
	_traces[cursor.getSessionId()] = make_shared<Trace>();
	_widgetsMutex.lock();
	for(auto const &item : _widgets) {
		shared_ptr<Widget> widget = item.second;
		if(widget->hit(p)) {
			_traces[cursor.getSessionId()]->widgetId = widget->id;
		}
	}
	_widgetsMutex.unlock();
	_traces[cursor.getSessionId()]->touchDown(p);
	_tracesMutex.unlock();
}

void SimilarToolApp::cursorUpdated(tuio::Cursor cursor) {
	Vec2f p(Vec2f(getWindowSize()) * cursor.getPos());
	_tracesMutex.lock();
	_traces[cursor.getSessionId()]->touchMove(p);
	_tracesMutex.unlock();
}

void SimilarToolApp::cursorRemoved(tuio::Cursor cursor) {
	Vec2f p(Vec2f(getWindowSize()) * cursor.getPos());
	_tracesMutex.lock();
	_traces[cursor.getSessionId()]->touchUp(p);
	_tracesMutex.unlock();
}

void SimilarToolApp::update() {
	
}

void SimilarToolApp::draw() {
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::color(1, 1, 1);
	gl::lineWidth(10.0f);
	gl::drawStrokedCircle(getWindowCenter(), getWindowSize().y*0.75*0.5);
	
	_widgetsMutex.lock();
	for(auto const &widget : _widgets) {
		widget.second->draw();
	}
	_widgetsMutex.unlock();
	
	_tracesMutex.lock();
	for(auto trace : _traces) {
		gl::drawSolidCircle(trace.second->points.back(), 10.0f);
		
	}
	_tracesMutex.unlock();
}

#pragma mark - Private methods

void SimilarToolApp::processTraces() {
	while(_tracesProcessorShouldRun) {
		_tracesMutex.lock();
		list<int> deadTraces;
		for(auto const &item : _traces) {
			shared_ptr<Trace> trace = item.second;
			// Check for tap
			if(trace->state == Trace::State::TOUCH_UP) {
				float d = trace->points.front().distance(trace->points.back());
				if(d < 10 && trace->points.size() < 5) {
					// Tap!
					if(trace->widgetId > 0) {
						_widgetsMutex.lock();
						_widgets[trace->widgetId]->tap(trace->points.front());
						_widgetsMutex.unlock();
					}
				} else if(trace->state == Trace::State::TOUCH_UP && trace->widgetId > 0) {
					_widgetsMutex.lock();
					if(dynamic_pointer_cast<PlayerWidget>(_widgets[trace->widgetId])) {
						Logger::instance().log(stringstream() << "PlayerWidget " << trace->widgetId << " -- Move -- from: " << trace->points.front() << " -- to: " << trace->points.back());
					}
					_widgetsMutex.unlock();
				}
				deadTraces.push_back(item.first);
			}
		}
		for(int i : deadTraces) {
			_traces.erase(i);
		}
		for(auto const &item : _traces) {
			shared_ptr<Trace> trace = item.second;
			// These traces are not dead yet, so we use them to trag things around
			if(trace->widgetId > 0 && trace->points.size() > 1) {
				_widgetsMutex.lock();
				_widgets[trace->widgetId]->position = trace->points.back();
				_widgetsMutex.unlock();
			}
		}
		_tracesMutex.unlock();
	}
}

CINDER_APP_NATIVE( SimilarToolApp, RendererGl(RendererGl::AA_MSAA_4) )
