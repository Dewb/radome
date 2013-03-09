#include "radomeApp.h"
#include "ofxFensterManager.h"

int main() {
    ofSetupOpenGL(ofxFensterManager::get(), 1216, 812, OF_WINDOW);
    ofRunFensterApp(new radomeApp());
}
