#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxCubeMap.h"
#include "ofxUI.h"

#include "turntableCam.h"
#include "icosohedron.h"
#include "radomeSyphonClient.h"

using std::list;
using std::vector;
using std::string;


enum DisplayMode {
    DisplayScene = 0,
    DisplayCubeMap,
    DisplayDome,
    DisplayProjectorOutput,
    LastDisplayMode,
};


class radomeApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void drawScene();
    void drawDome();
    void drawGroundPlane();
    void updateCubeMap();
    
    void loadFile();
    DisplayMode getDisplayMode() const { return _displayMode; }
    void changeDisplayMode(DisplayMode mode);
    
    void keyPressed(int key);
    void keyReleased(int key);

protected:
    void initGUI();
    void guiEvent(ofxUIEventArgs &e);
    
    ofxCubeMap _cubeMap;
    ofShader _shader;
    list<ofxAssimpModelLoader> _modelList;
    ofxTurntableCam _cam;
    ofxUICanvas* _pUI;
    
    radomeSyphonClient _vidOverlay;
    
    bool _fullscreen;
    ofVec3f _domeOrigin;
    float _animationTime;
    
    int _mixMode;
    int _mappingMode;
    
    enum DisplayMode _displayMode;
    vector<string> _displayModeNames;
    vector<string> _mixModeNames;
    vector<string> _mappingModeNames;

    vector<icosohedron::Triangle> _triangles;
};
