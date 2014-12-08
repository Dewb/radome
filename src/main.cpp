#include "ofMain.h"
#include "radomeApp.h"

//========================================================================
int main( ){
    
    ofxAppGLFWWindowMulti* window = new ofxAppGLFWWindowMulti();
	
    ofSetupOpenGL(window, 1024, 920, OF_WINDOW);
    ofRunApp(new radomeApp());
}
