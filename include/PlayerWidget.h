#pragma once

#include "Widget.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/SamplePlayerNode.h"

class PlayerWidget : public Widget {
	bool _isPlaying;
	float _width;
	
	ci::audio::BufferPlayerNodeRef _playerNode;
	ci::audio::GainNodeRef _gainNode;
	
public:
	PlayerWidget();
	PlayerWidget(std::string filename);
	virtual ~PlayerWidget();
	
	void draw();
	void tap(ci::Vec2f p);
	bool hit(ci::Vec2f p);
};