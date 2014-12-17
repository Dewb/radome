#pragma once

#include "ofMain.h"
#include "WindowManager.h"

#include "ofxUI.h"
#include "ofxOsc.h"

#include "turntableCam.h"
#include "radomeProjector.h"
#include "radomeRenderer.h"

using std::list;
using std::vector;
using std::string;


class radomeApp : public ofBaseApp {
public:
    radomeApp();
    ~radomeApp();
    
    void setup();
    void update();
    void draw();
    
    void loadFile();
    void showProjectorWindow();

    DisplayMode getDisplayMode() const { return _displayMode; }
    void changeDisplayMode(DisplayMode mode);
    
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseMoved(int x, int y);

protected:
    
    void createProjectorCalibrationUI(ofxUICanvas* pCanvas, int index);
    
    void initGUI();
    void guiEvent(ofxUIEventArgs &e);

    radomeRenderer _renderer;
    
    ofxUICanvas* _pUI;
    ofxUICanvas* _pCalibrationUI;
    ofxUICanvas* _pCalibrationUI2;

    ofxUICanvas* _pInputUI;

    ofxTurntableCam _cam;

    vector<radomeProjector*> _projectorList;

    enum DisplayMode _displayMode;
    vector<string> _displayModeNames;
    vector<string> _mixModeNames;
    vector<string> _mappingModeNames;
    
    ofxOscReceiver _oscReceiver;
    
    WindowManager _winManager;
};
