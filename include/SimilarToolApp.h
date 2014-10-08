#pragma once

#include "cinder/app/AppNative.h"

#include "TuioClient.h"
#include "OscSender.h"

#define FPS 60

using namespace std;
using namespace ci;
using namespace ci::app;

class Trace;
class Widget;

class SimilarToolApp : public AppNative {
	map<unsigned long, shared_ptr<Widget>> _widgets;
	mutex _widgetsMutex;
	
	map<unsigned long, shared_ptr<Trace>> _traces;
	mutex _tracesMutex;
	
	thread _tracesProcessor;
	bool _tracesProcessorShouldRun;
	
	void processTraces();
	
public:
	void prepareSettings(Settings *settings);
	void setup();
	void shutdown();
	void update();
	void draw();

	void mouseDown(MouseEvent event);
	void mouseDrag(MouseEvent event);
	void mouseUp(MouseEvent event);
};