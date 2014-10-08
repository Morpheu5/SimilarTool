#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimilarToolApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void SimilarToolApp::setup()
{
}

void SimilarToolApp::mouseDown( MouseEvent event )
{
}

void SimilarToolApp::update()
{
}

void SimilarToolApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( SimilarToolApp, RendererGl )
