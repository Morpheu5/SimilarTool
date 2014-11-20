#include "PlayerWidget.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/SamplePlayerNode.h"

#include "cinder/Utilities.h"

#include "Logger.h"

using namespace ci;
using namespace ci::app;
using namespace std;

PlayerWidget::PlayerWidget() : Widget() {
	_isPlaying = false;
	_width = 75.0;
	type = -1;
	filename = "";
}

PlayerWidget::PlayerWidget(string f) : PlayerWidget() {
	filename = f;
	
	auto context = audio::Context::master();
#if DEBUG==1
    auto filepath = getHomeDirectory()/"src/SimilarTool";
#else
	auto filepath = ci::app::getAppPath().remove_filename();
#endif
	audio::SourceFileRef audiofile = audio::load(loadFile(filepath/filename));
	audio::BufferRef buffer = audiofile->loadBuffer();
	_playerNode = context->makeNode(new audio::BufferPlayerNode(buffer));
	_gainNode = context->makeNode(new audio::GainNode(0.25f));
	
	_playerNode >> _gainNode >> context->getOutput();
	context->enable();
}

PlayerWidget::~PlayerWidget() {
	
}

void PlayerWidget::draw() {
	gl::pushModelView();
	
	Matrix44f transform;
	transform.translate(Vec3f(position));
	
	gl::multModelView(transform);
	
	gl::lineWidth(5.0f);
	float width = -_width/2;
	if(_playerNode->isEnabled()) {
		width = -_width/2 + _width * _playerNode->getReadPosition() / _playerNode->getNumFrames();
	}
	Rectf vr(-_width/2, -_width/2, width, _width/2);
	Rectf sr(-_width/2, -_width/2, _width/2, _width/2);
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);
	gl::drawSolidRoundedRect(vr, 1.0f);
	gl::color(1.0f, 1.0f, 1.0f, 1.0f);
	gl::drawStrokedRoundedRect(sr, 1.0f);
	
	gl::popModelView();
}

void PlayerWidget::tap(Vec2f p) {
	if(_playerNode->isEnabled()) {
		Logger::instance().log(stringstream() << "PlayerWidget " << id << " -- Tap -- Stop -- type: " << type << "; filename: " << filename);
		_playerNode->stop();
	} else {
		Logger::instance().log(stringstream() << "PlayerWidget " << id << " -- Tap -- Play -- type: " << type << "; filename: " << filename);
		_playerNode->start();
	}
}

bool PlayerWidget::hit(Vec2f p) {
	if(position.distance(p) < _width/2) {
		return true;
	}
	return false;
}