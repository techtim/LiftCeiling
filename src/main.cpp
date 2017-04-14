#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    
    ofPtr<ofAppGLFWWindow> window = ofPtr<ofAppGLFWWindow>(new ofAppGLFWWindow());
    // set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
    ofSetupOpenGL(window, 1000,600, OF_WINDOW);
    
//	ofSetupOpenGL(1600,1080,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new testApp());

}
