#include "radomeApp.h"
#include "ofxFensterManager.h"
#include "ofxFensterManager.cpp" // FUCK FINKSTER


int main() {
    ofSetupOpenGL(ofxFensterManager::get(), 1024, 720, OF_WINDOW);
    ofRunFensterApp(new radomeApp());
}
