#pragma once

class Widget {
	static unsigned long s_id;
	
public:
	unsigned long id;
	ci::Vec2f position;
	
	Widget();
	
	virtual void draw() = 0;
	virtual bool hit(ci::Vec2f p) = 0;
	virtual void tap(ci::Vec2f p) = 0;
};