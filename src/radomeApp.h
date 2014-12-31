#pragma once

#include "ofMain.h"
#include "WindowManager.h"

#include "ofxAssimpModelLoader.h"
#include "ofxCubeMap.h"
#include "ofxUI.h"
#include "ofxAutoReloadedShader.h"
#include "ofxOsc.h"

#include "turntableCam.h"
#include "icosohedron.h"
#include "radomeSyphonClient.h"
#include "radomeProjector.h"
#include "radomeModel.h"

using std::list;
using std::vector;
using std::string;

#define SHOW_CUBE_MAP



enum DisplayMode {
    DisplayScene = 0,
#ifdef SHOW_CUBE_MAP
    DisplayCubeMap,
#endif
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
    void updateProjectorOutput();
    
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
    void beginShader();
    void endShader();
    
    void prepDrawList();
    
    ofxUICanvas* _pUI;
    ofxUICanvas* _pCalibrationUI;
    ofxUICanvas* _pInputUI;
    
    ofxCubeMap _cubeMap;
    ofxAutoReloadedShader _shader;
    ofxTurntableCam _cam;
    unsigned int domeDrawIndex;

    list<radomeModel*> _modelList;
    vector<radomeProjector*> _projectorList;
    
    radomeSyphonClient _vidOverlay;
    ofImage _testPatternImage;
    bool _showTestPattern;

    float _animationTime;
    unsigned long long _lastSystemTime;
    
    int _mixMode;
    int _mappingMode;
    
    float _contrast;
    float _saturation;
    float _brightness;
    
    enum DisplayMode _displayMode;
    vector<string> _displayModeNames;
    vector<string> _mixModeNames;
    vector<string> _mappingModeNames;

    vector<icosohedron::Triangle> _triangles;
    
    float _domeDiameter;
    float _domeHeight;
    float _domeSliceParameterization;
    
    ofxOscReceiver _oscReceiver;
    
    WindowManager _winManager;
};
