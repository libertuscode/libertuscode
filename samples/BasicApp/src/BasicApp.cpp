
#define TINYCI_IMPLEMENTATION
#include "tinyci.hpp"

using namespace ci;
using namespace ci::app;

class BasicApp : public App {
public:
  void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;

	void draw() override;

private:
};

void prepareSettings( BasicApp::Settings* settings )
{
  int stopMe = 1;
}

void BasicApp::setup()
{
  //int stopMe = 1;
  fs::path p = "hello";
  std::string s = p.str();
  p = "c:\\somefile.txt";
  s = p.str();
  p = "c:\\Windows\\somefile.txt";
  s = p.str();
  p = "c:\\Windows\\com\\\\\\other\\\\somefile.txt";
  s = p.str();
  p = "//usr/share/lib/gcc///lib.a";
  s = p.str();
  p = p / "milk";
  s = p.str();
  p /= "cat";
  s = p.str();
  p = "/orig";
  //p /= "/rootpath";
  s = p;

  p = "C:\\";
  s = p;
  bool b = p.is_root();

  s = getAssetPath("file.txt");

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
