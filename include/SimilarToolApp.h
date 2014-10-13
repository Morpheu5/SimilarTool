#pragma once

#include "cinder/app/AppNative.h"

#include "TuioClient.h"

#define FPS 60

using namespace std;
using namespace ci;
using namespace ci::app;

class Trace;
class Widget;
class Triplet;

class SimilarToolApp : public AppNative {
	map<unsigned long, shared_ptr<Widget>> _widgets;
	mutex _widgetsMutex;
	
	map<unsigned long, shared_ptr<Trace>> _traces;
	mutex _tracesMutex;
	
	thread _tracesProcessor;
	bool _tracesProcessorShouldRun;
	
	void processTraces();
	
	vector<Triplet> _triplets;
	long _currentTriplet;
	
	tuio::Client _tuioClient;
	
	thread _loggerThread;
	
public:
	void prepareSettings(Settings *settings);
	void setup();
	void shutdown();
	void update();
	void draw();
	
	void keyUp(KeyEvent event);

	void mouseDown(MouseEvent event);
	void mouseDrag(MouseEvent event);
	void mouseUp(MouseEvent event);
	
	void cursorAdded(tuio::Cursor cursor);
	void cursorUpdated(tuio::Cursor cursor);
	void cursorRemoved(tuio::Cursor cursor);
};