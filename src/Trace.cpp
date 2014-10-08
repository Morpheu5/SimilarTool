#include "Trace.h"

Trace::Trace() {
	state = State::TOUCH_DOWN;
	widgetId = 0;
}

Trace::~Trace() {
	points.clear();
}

void Trace::touchDown(Vec2f p) {
	points.push_back(p);
	state = State::TOUCH_DOWN;
}

void Trace::touchMove(Vec2f p) {
	points.push_back(p);
	state = State::TOUCH_MOVING;
}

void Trace::touchUp(Vec2f p) {
	points.push_back(p);
	state = State::TOUCH_UP;
}