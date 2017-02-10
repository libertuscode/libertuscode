
#define TINYCI_IMPLEMENTATION
#include "tinyci.hpp"

using namespace ci;
using namespace ci::app;

class BasicApp : public App {
public:
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;

	void draw() override;

private:
};

void prepareSettings( BasicApp::Settings* settings )
{
  int stopMe = 1;
}

void BasicApp::mouseDrag( MouseEvent event )
{
}

void BasicApp::keyDown( KeyEvent event )
{
}

void BasicApp::draw()
{
  int stopMe = 1;
}

// This line tells Cinder to actually create and run the application.
//CINDER_APP( BasicApp, RendererGl, prepareSettings )
CINDER_APP( BasicApp, RendererGl)
