#include "radomeApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1216, 812, OF_WINDOW);
	ofRunApp(new radomeApp());
}
