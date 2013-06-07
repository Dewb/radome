#include "radomeApp.h"
#include "ofxFensterManager.h"

int main() {
    ofSetupOpenGL(ofxFensterManager::get(), 1024, 820, OF_WINDOW);
    ofRunFensterApp(new radomeApp());
}
