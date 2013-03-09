#pragma once

#include "ofMain.h"
#include "ofAppGlutWindow.h"

#include "ofxAssimpModelLoader.h"
#include "ofxCubeMap.h"
#include "ofxUI.h"
#include "ofxFenster.h"

#include "turntableCam.h"
#include "icosohedron.h"
#include "radomeSyphonClient.h"
#include "radomeProjector.h"
#include "radomeModel.h"

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
    radomeApp();
    ~radomeApp();
    
    void setup();
    void update();
    void draw();
    void drawScene();
    void drawDome();
    void drawGroundPlane();
    void updateCubeMap();
    
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
    void initGUI();
    void guiEvent(ofxUIEventArgs &e);
    
    ofxUICanvas* _pUI;
    
    ofxCubeMap _cubeMap;
    ofShader _shader;
    ofxTurntableCam _cam;

    list<radomeModel*> _modelList;
    list<radomeProjector*> _projectorList;
    ofxFenster* _projectorWindow;
    
    radomeSyphonClient _vidOverlay;
    ofImage _blankImage;
    
    bool _fullscreen;
    
    float _animationTime;
    unsigned long long _lastSystemTime;
    
    int _mixMode;
    int _mappingMode;
    
    enum DisplayMode _displayMode;
    vector<string> _displayModeNames;
    vector<string> _mixModeNames;
    vector<string> _mappingModeNames;

    vector<icosohedron::Triangle> _triangles;
};
