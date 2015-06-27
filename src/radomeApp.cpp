#include "radomeApp.h"
#include "radomeUtils.h"
#include "radomePlugin.h"

#define SIDEBAR_WIDTH 190
#define CALIBRATIONUI_WIDTH 280
#define NUM_PROJECTORS 4
#define DEFAULT_SETTINGS_FILE "projectorSettings.xml"
#define PROJECTOR_INITIAL_HEIGHT 147.5

#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"


#define APP_VERSION "v0.5"

radomeApp::radomeApp() {
    _pUI = NULL;
}

radomeApp::~radomeApp() {
    if (_pUI)
        delete (_pUI);
    
    deletePointerCollection(_projectorList);
}

void radomeApp::setup() {
    
    _winManager.setup((ofxAppGLFWWindowMulti *)ofGetWindowPtr());
    ofSetWindowTitle("Radome");
    
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    
    _displayMode = DisplayScene;

    _renderer.setup();
    _renderer.params.domeHeight = 110;
    _renderer.params.domeDiameter = 300;
    _renderer.params.domeCurve = 1.0;
    _renderer.params.domeBaseHeight = 0.0;
    _renderer.updateDomeModel();

    _cam.setTarget(ofVec3f(0.0, _renderer.params.domeHeight*0.25, 0.0));
    _cam.setRotation(0.66, 0.5);
    _cam.setupPerspective(false);
            
    for (int ii = 0; ii < NUM_PROJECTORS; ii++) {
        _projectorList.push_back(new radomeProjector(ii*360.0/(NUM_PROJECTORS*1.0)+60.0, _renderer.params.domeDiameter*1.5, PROJECTOR_INITIAL_HEIGHT));
    }    

    initGUI();
    
    _pCalibrationUI->loadSettings(DEFAULT_SETTINGS_FILE);
    
    glEnable(GL_DEPTH_TEST);

#ifdef USE_OSC
    _oscReceiver.setup(6000);
#endif
}

void radomeApp::createProjectorCalibrationUI(ofxUICanvas* pCanvas, int index) {
    index++;
    char buf[30];
    int w = CALIBRATIONUI_WIDTH - 40;
    sprintf(buf, "PROJECTOR %d", index);
    pCanvas->addWidgetDown(new ofxUILabel(buf, OFX_UI_FONT_MEDIUM));    
    sprintf(buf, "P%d HEIGHT", index);
    pCanvas->addMinimalSlider(buf, 5.0, 30.0, PROJECTOR_INITIAL_HEIGHT/10.0, w, 15);
    sprintf(buf, "P%d HEADING", index);
    pCanvas->addMinimalSlider(buf, 0.0, 360.0, 120.0 * index, w, 15);
    sprintf(buf, "P%d DISTANCE", index);
    pCanvas->addMinimalSlider(buf, _renderer.params.domeHeight/20.0, _renderer.params.domeDiameter/5.0, _renderer.params.domeDiameter*0.15, w, 15);
    sprintf(buf, "P%d FOV", index);
    pCanvas->addMinimalSlider(buf, 20.0, 90.0, 30.0, w, 15);
    sprintf(buf, "P%d TARGET", index);
    pCanvas->addMinimalSlider(buf, 0.0, 40.0, 2.0, w, 15);
    sprintf(buf, "P%d SHIFT", index);
    pCanvas->addMinimalSlider(buf, -3.0, 3.0, 0.0, w, 15);
    sprintf(buf, "P%d ROLL", index);
    pCanvas->addMinimalSlider(buf, -30.0, 30.0, 0.0, w, 15);
    sprintf(buf, "P%d JOINED", index);
    pCanvas->addToggle(buf, true);
}

void setUIColors(ofxUICanvas* pCanvas) {
    ofColor cb = ofColor(0, 25);
    ofColor co = ofColor(220, 220, 255, 120);
    ofColor coh = ofColor(240, 240, 255, 180);
    ofColor cf = ofColor(225, 225, 255, 140);
    ofColor cfh = ofColor(225, 225, 255, 255);
    ofColor cp = ofColor(120, 100);
    ofColor cpo = ofColor(255, 200);
    pCanvas->setUIColors( cb, co, coh, cf, cfh, cp, cpo );
}

void radomeApp::initGUI() {
    _pUI = new ofxUICanvas(10, 0, SIDEBAR_WIDTH, ofGetHeight());
    _pUI->setWidgetSpacing(5.0);
    _pUI->setDrawBack(true);
    _pUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
    
    _pUI->addWidgetDown(new ofxUILabel("RADOME " APP_VERSION, OFX_UI_FONT_LARGE));
    _pUI->addWidgetDown(new ofxUILabel(__DATE__ " " __TIME__, OFX_UI_FONT_SMALL));
    _pUI->addSpacer(0, 12);
    
    _displayModeNames.push_back("3D Scene");
#ifdef SHOW_CUBE_MAP
    _displayModeNames.push_back("Cube Map");
#endif
    _displayModeNames.push_back("Dome Preview");
    _displayModeNames.push_back("Output Preview");
    addRadioAndSetFirstItem(_pUI, "DISPLAY MODE", _displayModeNames, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addSpacer(0, 12);

    _pUI->addWidgetDown(new ofxUILabel("PROJECTORS", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUILabelButton("Show", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addWidgetRight(new ofxUILabelButton("Calibrate...", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addSpacer(0, 12);

    _pUI->addWidgetDown(new ofxUILabel("CONTENT", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUILabelButton("Video Input...", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ofxUILabelButton("Add 3D Model...", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addSpacer(0, 12);

    _pUI->addWidgetDown(new ofxUILabel("OUTPUT MIXER", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 12, 0, 2.0, 1, "CONTRAST", "CONTRAST", "2X", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 12, 0, 2.0, 1, "SATURATION", "SATURATION", "2X", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 12, 0, 2.0, 1, "BRIGHTNESS", "BRIGHTNESS", "2X", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 25, 0, 100, _renderer.params.inputOpacity*100.0, "XFADE", "2D", "3D", OFX_UI_FONT_MEDIUM));
    
    _mixModeNames.push_back("Underlay");
    _mixModeNames.push_back("Overlay");
    _mixModeNames.push_back("Mask");
    addRadioAndSetFirstItem(_pUI, "BLEND MODE", _mixModeNames, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addSpacer(0, 12);
    
    _mappingModeNames.push_back("Fisheye (Fulldome)");
    _mappingModeNames.push_back("Lat/Long (Standard)");
    _mappingModeNames.push_back("Mirrored Quadrants");
    addRadioAndSetFirstItem(_pUI, "MAPPING MODE", _mappingModeNames, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addSpacer(0, 12);
    
    _pInputUI = new ofxUICanvas(SIDEBAR_WIDTH + 10, 0, CALIBRATIONUI_WIDTH, ofGetHeight());
    _pInputUI->setWidgetSpacing(5.0);
    _pInputUI->setDrawBack(true);
    _pInputUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
    
    //setUIColors(_pInputUI);
    
    _pInputUI->addWidgetDown(new ofxUILabel("VIDEO INPUT", OFX_UI_FONT_LARGE));
    _pInputUI->addSpacer(0, 12);
    _pInputUI->addWidgetDown(new ofxUILabel("SYPHON APP NAME", OFX_UI_FONT_MEDIUM));
    addTextInput(_pInputUI, "SYPHON_APP", DEFAULT_SYPHON_APP, 180);
    _pInputUI->addWidgetDown(new ofxUILabel("SYPHON SERVER NAME", OFX_UI_FONT_MEDIUM));
    addTextInput(_pInputUI, "SYPHON_SERVER", DEFAULT_SYPHON_SERVER, 180);
    _pInputUI->addSpacer(0, 12);
    _pInputUI->addWidgetDown(new ofxUILabel("TEST PATTERN", OFX_UI_FONT_MEDIUM));    
	_pInputUI->addToggle("Show Pattern", false, 25, 25);
    _pInputUI->setVisible(false);
    _pInputUI->addSpacer(0, 12);
    _pInputUI->addWidgetDown(new ofxUILabel("SYPHON PREVIEW", OFX_UI_FONT_LARGE));
    _pInputUI->addSpacer(0, 320);

    _pInputUI->addWidgetDown(new ofxUILabel("PLUGINS", OFX_UI_FONT_LARGE));
    for (auto plug : PluginLibrary::getList()) {
        _pInputUI->addToggle(typeid(*plug).name(), false, 25, 25);
    }
    
    _pCalibrationUI = new ofxUICanvas(SIDEBAR_WIDTH + 10, 0, CALIBRATIONUI_WIDTH, ofGetHeight());
    _pCalibrationUI->setWidgetSpacing(5.0);
    _pCalibrationUI->setDrawBack(true);
    _pCalibrationUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
    //setUIColors(_pCalibrationUI);

    _pCalibrationUI->addWidgetDown(new ofxUILabel("CALIBRATION", OFX_UI_FONT_LARGE));
    _pCalibrationUI->addSpacer(0, 12);
    
    int w = CALIBRATIONUI_WIDTH - 10;
    
    _pCalibrationUI->addMinimalSlider("DOME HEIGHT", 5.0, 30.0, _renderer.params.domeHeight/10, w, 15);
    _pCalibrationUI->addMinimalSlider("DOME DIAMETER", 5.0, 50.0, _renderer.params.domeDiameter/10, w, 15);
    _pCalibrationUI->addMinimalSlider("DOME CURVE", 0.75, 1.0, _renderer.params.domeCurve, w, 15);
    _pCalibrationUI->addMinimalSlider("DOME BASE HEIGHT", 0.0, 10.0, _renderer.params.domeBaseHeight/10, w, 15);

    _pCalibrationUI->addSpacer(0, 12);

    _pCalibrationUI2 = new ofxUICanvas(SIDEBAR_WIDTH + CALIBRATIONUI_WIDTH + 10, 0, CALIBRATIONUI_WIDTH, ofGetHeight());
    _pCalibrationUI2->setWidgetSpacing(5.0);
    _pCalibrationUI2->setDrawBack(true);
    _pCalibrationUI2->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
    _pCalibrationUI2->addSpacer(0, 140);

    for (int ii = 0; ii < NUM_PROJECTORS; ii++) {
        auto pUI = ii < 2 ? _pCalibrationUI : _pCalibrationUI2;
        createProjectorCalibrationUI(pUI, ii);
        pUI->addSpacer(0, 8);
    }
    
    _pCalibrationUI->addWidgetDown(new ofxUILabelButton("Load", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));
    _pCalibrationUI->addWidgetRight(new ofxUILabelButton("Save", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));

    _pCalibrationUI->setVisible(false);
    _pCalibrationUI2->setVisible(false);

    ofBackground(40, 20, 32);

    ofAddListener(_pUI->newGUIEvent, this, &radomeApp::guiEvent);
    ofAddListener(_pInputUI->newGUIEvent, this, &radomeApp::guiEvent);
    ofAddListener(_pCalibrationUI->newGUIEvent, this, &radomeApp::guiEvent);
    ofAddListener(_pCalibrationUI2->newGUIEvent, this, &radomeApp::guiEvent);
}

void radomeApp::loadFile() {
    ofFileDialogResult result = ofSystemLoadDialog("Load Model", false, "./radome/content");
    
    if (result.bSuccess) {
        _renderer.addModelFromFile(result.getPath());
    }
}

void radomeApp::showProjectorWindows() {

    if (_winManager.getNumWindows() == 1) {
        int joinedProjectors = 0;
        for (auto projector : _projectorList) {
            if (projector->isJoined()) {
                joinedProjectors++;
            }
        }
        if (joinedProjectors > 0) {
            _winManager.createWindow("Joined Output", 400, 300, 250 * joinedProjectors, 200, false);
        }
        if (_projectorList.size() - joinedProjectors > 0) {
            _winManager.createWindow("Output A", 400, 300, 400, 300, false);
        }
        if (_projectorList.size() - joinedProjectors > 1) {
            _winManager.createWindow("Output B", 400, 300, 400, 300, false);
        }
        if (_projectorList.size() - joinedProjectors > 2) {
            _winManager.createWindow("Output C", 400, 300, 400, 300, false);
        }
        if (_projectorList.size() - joinedProjectors > 3) {
            _winManager.createWindow("Output D", 400, 300, 400, 300, false);
        }
    }
}

void radomeApp::update() {

    
    DomeInfo dome;
    dome.height = _renderer.params.domeHeight;
    dome.radius = _renderer.params.domeDiameter/2;
    dome.frameTime = ofGetSystemTime();
    
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->update(dome);
        }
    }

    _renderer.render(_projectorList);
    
#ifdef USE_OSC
    while(_oscReceiver.hasWaitingMessages()){
		ofxOscMessage m;
		_oscReceiver.getNextMessage(&m);
        for (auto plug : PluginLibrary::getList()) {
            if (plug->isEnabled()) {
                plug->receiveOscMessage(m);

            }
        }
    }

    _pUI->setHeight(ofGetHeight());
    _pCalibrationUI->setHeight(ofGetHeight());
    _pCalibrationUI2->setHeight(ofGetHeight());
    _pInputUI->setHeight(ofGetHeight());
#endif
}

void radomeApp::draw() {
    if (_winManager.getActiveWindowNo() != 0) {
        if (_projectorList.size()) {
            int joinedProjectors = 0;
            for (auto projector : _projectorList) {
                if (projector->isJoined()) {
                    joinedProjectors++;
                }
            }
            int h = ofGetHeight();
            int x = 0;
            if (_winManager.getActiveWindowNo() == 1 && joinedProjectors > 0) {
                int w = ofGetWidth() / joinedProjectors;
                for (auto projector : _projectorList) {
                    if (projector->isJoined()) {
                        projector->drawFramebuffer(x, 0, w, h);
                        x += w;
                    }
                }
            } else {
                int w = ofGetWidth();
                int unjoinedProjectorIndex = 0;
                for (auto projector : _projectorList) {
                    if (!projector->isJoined()) {
                        if (_winManager.getActiveWindowNo() == unjoinedProjectorIndex + 2) {
                            projector->drawFramebuffer(x, 0, w, h);
                            break;
                        } else {
                            unjoinedProjectorIndex++;
                        }
                    }
                }
            }
        }
        return;
    }

    _renderer.drawScenePreview(_displayMode, _cam, _projectorList, SIDEBAR_WIDTH);
    
    glDisable(GL_DEPTH_TEST);
    _pUI->draw();
    _pCalibrationUI2->setVisible(_pCalibrationUI->isVisible());
    if (_pCalibrationUI->isVisible()) {
        _pCalibrationUI->draw();
        _pCalibrationUI2->draw();
    }
    if (_pInputUI->isVisible()) {
        _pInputUI->draw();
        _renderer.drawInputVideo(SIDEBAR_WIDTH, 260, 290);
    }
    glEnable(GL_DEPTH_TEST);
}


void radomeApp::keyPressed(int key) {
    
    if (_pInputUI && _pInputUI->isVisible())
        return;
    
    float accel = 3.0;
    auto model = *(_renderer.getModelList().rbegin());
    
    switch (key) {
        case 'w': if (model) model->_origin.y += accel; break;
        case 's': if (model) model->_origin.y -= accel; break;
        case 'a': if (model) model->_origin.x -= accel; break;
        case 'd': if (model) model->_origin.x += accel; break;
        case 'z': if (model) model->_origin.z += accel; break;
        case 'x': if (model) model->_origin.z -= accel; break;
        case 'W': if (model) model->_origin.y += accel * 4; break;
        case 'S': if (model) model->_origin.y -= accel * 4; break;
        case 'A': if (model) model->_origin.x -= accel * 4; break;
        case 'D': if (model) model->_origin.x += accel * 4; break;
        case 'Z': if (model) model->_origin.z += accel * 4; break;
        case 'X': if (model) model->_origin.z -= accel * 4; break;
        case 'l': loadFile(); break;
        case 'm':
            {
                int mode = (int)getDisplayMode();
                mode++;
                changeDisplayMode((DisplayMode)mode);
            }
            break;
        case 'f':
            {
                // use OS native fullscreen features for now
                /*
                int screen = _winManager.getActiveWindowNo();
                if (screen == 0) {
                    screen = 1;
                }
                _winManager.setFullscreen(screen, true);
                */
            }
            break;
        case 'p':
            {
                showProjectorWindows();
            }
            break;
        case 'c':
            {
                if (_pCalibrationUI) {
                    bool bVis = _pCalibrationUI->isVisible();
                    if (!bVis && _pInputUI && _pInputUI->isVisible()) {
                        _pInputUI->setVisible(false);
                    }
                    _pCalibrationUI->setVisible(!bVis);
                }
            }
            break;
        case 'C':
            {
                if (_renderer.getModelList().size() > 0)
                {
                    auto m = _renderer.getModelList().back();
                    _renderer.getModelList().pop_back();
                    if (m) delete(m);
                }
            }
            break;
        case 'r':
            {
                if (model) {
                    if (model->getRotationIncrement() == 0) {
                        model->setRotationOrigin(model->getOrigin());
                        model->setRotation(ofVec4f(0.0,
                                                   frand_bounded(),
                                                   frand_bounded(),
                                                   frand_bounded()));
                    }
                    model->setRotationIncrement(model->getRotationIncrement() + 2.0);
                    if (model->getRotationIncrement() == 10.0) {
                        model->setRotationIncrement(0.0);
                    }
                }
            }
            break;
    }
    
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->keyPressed(key);
        }
    }
}

void radomeApp::mousePressed(int x, int y, int button) {
    if ((x > SIDEBAR_WIDTH + 5) &&
        (!_pCalibrationUI || !_pCalibrationUI->isVisible() || x > (SIDEBAR_WIDTH + CALIBRATIONUI_WIDTH * 2 + 10))) {
        _cam.mousePressed(x, y, button);
    }
    
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->mousePressed(x, y, button);
        }
    }
}

void radomeApp::mouseReleased(int x, int y, int button) {
    _cam.mouseReleased(x, y, button);

    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->mouseReleased(x, y, button);
        }
    }
}

void radomeApp::mouseDragged(int x, int y, int button) {
    _cam.mouseDragged(x, y, button);
    
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->mouseDragged(x, y, button);
        }
    }
}

void radomeApp::mouseMoved(int x, int y) {
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->mouseMoved(x, y);
        }
    }
}

void radomeApp::changeDisplayMode(DisplayMode mode) {
    _displayMode = mode;
    if (_displayMode == LastDisplayMode)
        _displayMode = DisplayScene;
    
    ofxUIRadio* pRadio = dynamic_cast<ofxUIRadio*>(_pUI->getWidget("DISPLAY MODE"));
    if (pRadio) {
        pRadio->activateToggle(_displayModeNames[_displayMode]);
    }
}

void radomeApp::guiEvent(ofxUIEventArgs &e) {
    string name = e.widget->getName();
    
    int radio;
    if(matchRadioButton(name, _displayModeNames, &radio)) {
        changeDisplayMode((DisplayMode)radio);
        return;
    }
    
    if (matchRadioButton(name, _mixModeNames, &_renderer.params.mixMode))
        return;

    if (matchRadioButton(name, _mappingModeNames, &_renderer.params.mappingMode))
        return;
            
    if (name == "XFADE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.inputOpacity = slider->getValue();
        }
    } else if (name == "Add 3D Model...") {
        auto pButton = dynamic_cast<ofxUIButton*>(e.widget);
        if (pButton && !pButton->getValue())
        {
            loadFile();
        }
    } else if (name == "Show") {
        auto pButton = dynamic_cast<ofxUIButton*>(e.widget);
        if (pButton && !pButton->getValue())
        {
            showProjectorWindows();
        }
    } else if (name == "Calibrate...") {
        auto pButton = dynamic_cast<ofxUIButton*>(e.widget);
        if (pButton && !pButton->getValue())
        {
            if (_pCalibrationUI) {
                bool bVis = _pCalibrationUI->isVisible();
                if (!bVis && _pInputUI && _pInputUI->isVisible()) {
                    _pInputUI->setVisible(false);
                }
                _pCalibrationUI->setVisible(!bVis);
            }
        }
    } else if (name == "Video Input...") {
        auto pButton = dynamic_cast<ofxUIButton*>(e.widget);
        if (pButton && !pButton->getValue())
        {
            if (_pInputUI) {
                bool bVis = _pInputUI->isVisible();
                if (!bVis && _pCalibrationUI && _pCalibrationUI->isVisible()) {
                    _pCalibrationUI->setVisible(false);
                }
                _pInputUI->setVisible(!bVis);
            }
        }
    } else if(name == "SYPHON_APP") {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            _renderer.setInputApplicationName(trim(pInput->getTextString()));
        }
    } else if(name == "SYPHON_SERVER") {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            _renderer.setInputServerName(trim(pInput->getTextString()));
        }
    } else if (name[0] == 'P' && name[2] == ' ') {
        char p[2] = { name[1], 0 };
        int pindex = atoi(p) - 1;
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        auto togglebox = dynamic_cast<ofxUIToggle*>(e.widget);
        if (slider && _projectorList.size() > pindex && _projectorList[pindex]) {
            if (name.substr(3) == "HEIGHT") {
                _projectorList[pindex]->setHeight(slider->getScaledValue() * 10);
            } else if (name.substr(3) == "HEADING") {
                _projectorList[pindex]->setHeading(slider->getScaledValue());
            } else if (name.substr(3) == "DISTANCE") {
                _projectorList[pindex]->setDistance(slider->getScaledValue() * 10);
            } else if (name.substr(3) == "FOV") {
                _projectorList[pindex]->setFOV(slider->getScaledValue());
            } else if (name.substr(3) == "TARGET") {
                _projectorList[pindex]->setTargetHeight(slider->getScaledValue() * 10);
            } else if (name.substr(3) == "SHIFT") {
                _projectorList[pindex]->setLensOffsetY(slider->getScaledValue());
            } else if (name.substr(3) == "ROLL") {
                _projectorList[pindex]->setRoll(slider->getScaledValue());
            }
        } else if (togglebox && _projectorList.size() > pindex && _projectorList[pindex]) {
            if (name.substr(3) == "JOINED") {
                _projectorList[pindex]->setJoined(togglebox->getValue());
            }
        }
    } else if (name == "Load") {
        if (_pCalibrationUI) {
            _pCalibrationUI->loadSettings(DEFAULT_SETTINGS_FILE);
        }
    } else if (name == "Save") {
        if (_pCalibrationUI) {
            _pCalibrationUI->saveSettings(DEFAULT_SETTINGS_FILE);
        }
    } else if (name == "Show Pattern") {
        auto pButton = dynamic_cast<ofxUIButton*>(e.widget);
        if (pButton) {
            _renderer.params.showTestPattern = pButton->getValue();
        }
    } else if (name == "CONTRAST") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.contrast = slider->getScaledValue();
        }
    } else if (name == "SATURATION") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.saturation = slider->getScaledValue();
        }
    } else if (name == "BRIGHTNESS") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.brightness = slider->getScaledValue();
        }
    } else if (name == "DOME HEIGHT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.domeHeight = slider->getScaledValue() * 10;
            _renderer.updateDomeModel();
        }
    } else if (name == "DOME DIAMETER") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.domeDiameter = slider->getScaledValue() * 10;
            _renderer.updateDomeModel();
        }
    } else if (name == "DOME CURVE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.domeCurve = slider->getScaledValue();
            _renderer.updateDomeModel();
        }
    } else if (name == "DOME BASE HEIGHT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _renderer.params.domeBaseHeight = slider->getScaledValue() * 10;
            _renderer.updateDomeModel();
        }
    }

    for (auto plug : PluginLibrary::getList()) {
        if (typeid(*plug).name() == name) {
            plug->setEnabled(!plug->isEnabled());
        }
    }
}