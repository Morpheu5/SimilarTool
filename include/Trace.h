#pragma once

#include "cinder/Vector.h"

using namespace ci;
using namespace std;

class Trace {
public:
	vector<ci::Vec2f> points;
	long int widgetId;
	
	enum class State {
		TOUCH_DOWN,
		TOUCH_MOVING,
		TOUCH_STILL,
		TOUCH_UP
	} state;

	Trace();
	~Trace();
	
	void touchDown(Vec2f p);
	void touchMove(Vec2f p);
	void touchUp(Vec2f p);
};