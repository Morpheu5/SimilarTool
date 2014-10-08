#include "SimilarToolApp.h"

#include "Trace.h"
#include "Widget.h"
#include "PlayerWidget.h"

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
	
	_tracesProcessorShouldRun = true;
	_tracesProcessor = thread(bind(&SimilarToolApp::processTraces, this));
	
	Vec2f c = getWindowCenter();
	
	shared_ptr<PlayerWidget> player = make_shared<PlayerWidget>("assets/drums.mp3");
	player->position = Vec2f(c.x - 150.0f, c.y);
	_widgets[player->id] = player;

	player = make_shared<PlayerWidget>("assets/guitar.mp3");
	player->position = Vec2f(c.x, c.y);
	_widgets[player->id] = player;

	player = make_shared<PlayerWidget>("assets/jdee_beat.mp3");
	player->position = Vec2f(c.x + 150.0f, c.y);
	_widgets[player->id] = player;
}

void SimilarToolApp::shutdown() {
	_tracesProcessorShouldRun = false;
	_tracesProcessor.join();
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
