#include "radomeApp.h"
#include "ofxFensterManager.h"

int main() {
    ofSetupOpenGL(ofxFensterManager::get(), 1024, 720, OF_WINDOW);
    ofRunFensterApp(new radomeApp());
}
