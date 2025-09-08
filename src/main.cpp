#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(1200, 1920);
    
#ifdef __APPLE__
	settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN
#elif __LINUX
    settings.windowMode = OF_FULLSCREEN; //can also be OF_FULLSCREEN
#endif
    
    auto window = ofCreateWindow(settings);
    
	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();

}
