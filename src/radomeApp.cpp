#include "radomeApp.h"
#include "radomeUtils.h"
#include "radomePlugin.h"

#define SIDEBAR_WIDTH 190
#define CALIBRATIONUI_WIDTH 280
#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"
#define NUM_PROJECTORS 3
#define DEFAULT_SETTINGS_FILE "projectorSettings.xml"
#define PROJECTOR_INITIAL_HEIGHT 147.5

#define APP_VERSION "v0.4"

radomeApp::radomeApp() {
    _pUI = NULL;
}

radomeApp::~radomeApp() {
    if (_pUI)
        delete (_pUI);
    
    deletePointerCollection(_projectorList);
    deletePointerCollection(_modelList);
}

void radomeApp::setup() {
    
    _winManager.setup((ofxAppGLFWWindowMulti *)ofGetWindowPtr());
    ofSetWindowTitle("Radome");
    
    ofSetFrameRate(45);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    
    _displayMode = DisplayScene;
    _mixMode = 0;
    _mappingMode = 0;
    _animationTime = 0.0;
    _lastSystemTime = ofGetSystemTime();

    _shader.load("radome");
    
    _cubeMap.initEmptyTextures(2048, GL_RGBA);
    _cubeMap.setNearFar(ofVec2f(0.01, 8192.0));
    
    _domeHeight = 110;
    _domeDiameter = 300;
    _domeSliceParameterization = 1.0;
    
    _cam.setTarget(ofVec3f(0.0, _domeHeight*0.25, 0.0));
    _cam.setRotation(0.66, 0.5);
    _cam.setupPerspective(false);
    
    _triangles = icosohedron::create(4);
    
    _vidOverlay.initialize(DEFAULT_SYPHON_APP, DEFAULT_SYPHON_SERVER);
    _vidOverlay.setFaderValue(1.0);

    _contrast = 1.0;
    _saturation = 1.0;
    _brightness = 1.0;
    
    _testPatternImage.setUseTexture(true);
    _testPatternImage.loadImage("testpatt.jpg");
    _showTestPattern = false;
    
    for (int ii = 0; ii < NUM_PROJECTORS; ii++) {
        _projectorList.push_back(new radomeProjector(ii*360.0/(NUM_PROJECTORS*1.0)+60.0, _domeDiameter*1.5, PROJECTOR_INITIAL_HEIGHT));
    }    
    
    initGUI();
    
    _pCalibrationUI->loadSettings(DEFAULT_SETTINGS_FILE);
    
    glEnable(GL_DEPTH_TEST);
    prepDrawList();
    
    _oscReceiver.setup(6000);
}

void radomeApp::createProjectorCalibrationUI(ofxUICanvas* pCanvas, int index) {
    index++;
    char buf[30];
    int w = CALIBRATIONUI_WIDTH - 10;
    sprintf(buf, "PROJECTOR %d", index);
    pCanvas->addWidgetDown(new ofxUILabel(buf, OFX_UI_FONT_MEDIUM));    
    sprintf(buf, "P%d HEIGHT", index);
    pCanvas->addMinimalSlider(buf, 5.0, 30.0, PROJECTOR_INITIAL_HEIGHT/10.0, w, 15);
    sprintf(buf, "P%d HEADING", index);
    pCanvas->addMinimalSlider(buf, 0.0, 360.0, 120.0 * index, w, 15);
    sprintf(buf, "P%d DISTANCE", index);
    pCanvas->addMinimalSlider(buf, _domeHeight/20.0, _domeDiameter/5.0, _domeDiameter*0.15, w, 15);
    sprintf(buf, "P%d FOV", index);
    pCanvas->addMinimalSlider(buf, 20.0, 90.0, 30.0, w, 15);
    sprintf(buf, "P%d TARGET", index);
    pCanvas->addMinimalSlider(buf, 0.0, 40.0, 2.0, w, 15);
    sprintf(buf, "P%d SHIFT", index);
    pCanvas->addMinimalSlider(buf, -3.0, 3.0, 0.0, w, 15);
    sprintf(buf, "P%d ROLL", index);
    pCanvas->addMinimalSlider(buf, -30.0, 30.0, 0.0, w, 15);

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
    _pUI->addWidgetDown(new ofxUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 25, 0, 100, _vidOverlay.getFaderValue()*100.0, "XFADE", "2D", "3D", OFX_UI_FONT_MEDIUM));
    
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
    
    _pCalibrationUI->addMinimalSlider("DOME HEIGHT", 5.0, 30.0, _domeHeight/10, w, 15);
    _pCalibrationUI->addMinimalSlider("DOME DIAMETER", 5.0, 50.0, _domeDiameter/10, w, 15);
    _pCalibrationUI->addMinimalSlider("DOME SLICE", 0.75, 1.0, _domeSliceParameterization, w, 15);

    _pCalibrationUI->addSpacer(0, 12);
    
    for (int ii = 0; ii < 3; ii++) {
        createProjectorCalibrationUI(_pCalibrationUI, ii);
        _pCalibrationUI->addSpacer(0, 8);
    }
    
    _pCalibrationUI->addWidgetDown(new ofxUILabelButton("Load", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));
    _pCalibrationUI->addWidgetRight(new ofxUILabelButton("Save", false, 0, 25, 0, 0, OFX_UI_FONT_SMALL));

    _pCalibrationUI->setVisible(false);
    
    ofBackground(40, 20, 32);

    ofAddListener(_pUI->newGUIEvent, this, &radomeApp::guiEvent);
    ofAddListener(_pInputUI->newGUIEvent, this, &radomeApp::guiEvent);
    ofAddListener(_pCalibrationUI->newGUIEvent, this, &radomeApp::guiEvent);
}

void radomeApp::prepDrawList()
{
    domeDrawIndex = glGenLists(1);
    glNewList(domeDrawIndex, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    float yshift = (1 - _domeSliceParameterization) * _domeHeight;
    
    // desired slice radius b = sqrt(radius * radius - yshift * yshift);
    float b = _domeDiameter/2.0;
    float radius = sqrt(b * b + yshift * yshift);
    
    ofVec3f d, s = ofVec3f(radius, _domeHeight + yshift, radius);
    for (auto& t : _triangles) {
        d = t.v0 * s;
        d.y -= yshift;
        glNormal3fv(&d[0]);
        glVertex3fv(&d[0]);

        d = t.v1 * s;
        d.y -= yshift;
        glNormal3fv(&d[0]);
        glVertex3fv(&d[0]);

        d = t.v2 * s;
        d.y -= yshift;
        glNormal3fv(&d[0]);
        glVertex3fv(&d[0]);
    }
    glEnd();
    glEndList();
}

void radomeApp::loadFile() {
    ofFileDialogResult result = ofSystemLoadDialog("Load Model", false, "./radome/content");
    
    if (result.bSuccess) {
        auto model = new radomeModel();
        model->loadModel(result.getPath());
        _modelList.push_back(model);
    }
}

void radomeApp::showProjectorWindow() {
    if (_winManager.getNumWindows() == 1) {
        _winManager.createWindow("Projector Output", 400, 300, 750, 200, false);
    }
}

void radomeApp::update() {
    unsigned long long time = ofGetSystemTime();
    _animationTime += (time - _lastSystemTime)/1000.0;
    _lastSystemTime = time;
    
    if (_animationTime >= 1.0) {
        _animationTime = 0.0;
    }
    for (auto iter = _modelList.begin(); iter != _modelList.end(); ++iter) {
        (*iter)->update(_animationTime);
    }
    
    DomeInfo dome;
    dome.height = _domeHeight;
    dome.radius = _domeDiameter/2;
    dome.frameTime = ofGetSystemTime();
    
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {
            plug->update(dome);
        }
    }
    
    updateCubeMap();
    updateProjectorOutput();
    
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
    _pInputUI->setHeight(ofGetHeight());
}

void radomeApp::updateCubeMap() {
    glEnable(GL_DEPTH_TEST);
    _cubeMap.setPosition(0, 0, 0);
    LineWidthAdjuster::factor = 7.0;
    
    for(int i = 0; i < 6; i++) {
        _cubeMap.beginDrawingInto3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofClear(0,0,0,0);
        drawScene();
        ofPopStyle();
        _cubeMap.endDrawingInto3D();
    }
    
    LineWidthAdjuster::factor = 1.0;
}

void radomeApp::updateProjectorOutput() {
    glEnable(GL_DEPTH_TEST);
    for (auto iter = _projectorList.begin(); iter != _projectorList.end(); ++iter) {
        (*iter)->renderBegin();

        beginShader();
        drawDome();
        endShader();

        (*iter)->renderEnd();
    }
}

void radomeApp::beginShader() {
    _shader.begin();
    _cubeMap.bind();

    _shader.setUniform1i("EnvMap", 0);
    _shader.setUniform1i("mixMode", _mixMode);
    _shader.setUniform1i("mappingMode", _mappingMode);
    _shader.setUniform1f("domeDiameter", _domeDiameter);
    _shader.setUniform1f("domeHeight", _domeHeight);
    _shader.setUniform1f("contrast", _contrast);
    _shader.setUniform1f("saturation", _saturation);
    _shader.setUniform1f("brightness", _brightness);
    
    
    if (_showTestPattern) {
        _shader.setUniform1f("videoMix", 0.0);
        _shader.setUniform2f("videoSize", _testPatternImage.getWidth(), _testPatternImage.getHeight());
        _shader.setUniformTexture("video", _testPatternImage.getTextureReference(), 1);
    } else if (_vidOverlay.maybeBind()) {
        _shader.setUniform1f("videoMix", _vidOverlay.getFaderValue());
        _shader.setUniform2f("videoSize", _vidOverlay.getWidth(), _vidOverlay.getHeight());
        _shader.setUniformTexture("video", _vidOverlay.getTexture(), 1);
    } else {
        _shader.setUniform1f("videoMix", -1.0);
        _shader.setUniform2f("videoSize", 0.0, 0.0);
        _shader.setUniformTexture("video", _testPatternImage.getTextureReference(), 1);
    }
}

void radomeApp::endShader() {
    if (_vidOverlay.isBound())
        _vidOverlay.unbind();
    _cubeMap.unbind();
    _shader.end();
}

void radomeApp::draw() {
    if (_winManager.getActiveWindowNo() != 0) {
        if (_projectorList.size()) {
            int w = ofGetWidth() / _projectorList.size();
            int h = ofGetHeight();
            int x = 0;
            for (auto projector : _projectorList) {
                projector->drawFramebuffer(x, 0, w, h);
                x += w;
            }
        }
        return;
    }
    
    switch (_displayMode) {
        case DisplayScene: {
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofClear(0, 0, 0);

            _cam.setDistance(_domeDiameter*1.6);
            _cam.begin();
                        
            ofPushMatrix();
            drawScene();
            ofPopMatrix();

            ofSetColor(128,128,255,128);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            ofSetLineWidth(1);
            drawDome();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            ofSetColor(80,80,192,128);
            drawGroundPlane();

            _cam.end();
            
            ofPopStyle();
        }
        break;
#ifdef SHOW_CUBE_MAP
        case DisplayCubeMap: {
            ofClear(0, 0, 0);
            ofSetColor(200,220,255);
            int margin = 2;
            int w = (ofGetWindowWidth() - SIDEBAR_WIDTH - margin*4) / 3;
            int h = (ofGetWindowHeight() - margin*3) / 2;
            for (int i = 0; i < 6; i++) {
                int x = margin + i%3 * (w + margin) + SIDEBAR_WIDTH;
                int y = margin + i/3 * (h + margin);
                //ofDrawBitmapString(_cubeMap.getDescriptiveStringForFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), x+margin*1.5, y+10+margin*1.5);
                _cubeMap.drawFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i , x, y, w, h);
                ofRect(x-1, y-1, w + margin, h + margin);
            }
        }
        break;
#endif
        case DisplayDome: {

            ofClear(20, 100, 50);
            
            _cam.setDistance(_domeDiameter*1.10);
            _cam.begin();
            
            beginShader();
            drawDome();
            drawGroundPlane();
            endShader();
            
            for (auto iter = _projectorList.begin(); iter != _projectorList.end(); ++iter)
            {
                (*iter)->drawSceneRepresentation();
            }
            
            _cam.end();
        }
        break;
        case DisplayProjectorOutput:
        case LastDisplayMode: {
            ofClear(0, 0, 0);
            ofSetColor(200,220,255);
            int margin = 2;
            int w = (ofGetWindowWidth() - SIDEBAR_WIDTH - margin*4) / 2;
            int h = (ofGetWindowHeight() - margin*3) / 2;
            auto iter = _projectorList.begin();
            for (int i = 0; iter != _projectorList.end(); i++, ++iter) {
                int x = margin + i%2 * (w + margin) + SIDEBAR_WIDTH;
                int y = margin + i/2 * (h + margin);
                (*iter)->drawFramebuffer(x, y, w, h);
                ofRect(x-1, y-1, w + margin, h + margin);
            }
        }
        break;            
    }
    
    
    glDisable(GL_DEPTH_TEST);
    _pUI->draw();
    if (_pCalibrationUI->isVisible()) {
        _pCalibrationUI->draw();
    }
    if (_pInputUI->isVisible()) {
        _pInputUI->draw();
        _vidOverlay.getTexture().draw(SIDEBAR_WIDTH, 260, 290, 290 * _vidOverlay.getHeight() / _vidOverlay.getWidth());
    }
    glEnable(GL_DEPTH_TEST);
}

void radomeApp::drawScene() {
    ofSetColor(180, 192, 192);
    
    // Draw static and animated 3D models
    for (auto model : _modelList)
    {
        model->draw();
    }
    
    DomeInfo dome;
    dome.height = _domeHeight;
    dome.radius = _domeDiameter/2;
    dome.frameTime = ofGetSystemTime();
    
    // Draw 3D scene plugins
    for (auto plug : PluginLibrary::getList()) {
        if (plug->isEnabled()) {

            ofPushStyle();
            ofPushMatrix();
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

            plug->renderScene(dome);
            
            glPopClientAttrib();
            glPopAttrib();
            ofPopMatrix();
            ofPopStyle();
        }
    }
}

void radomeApp::drawDome() {
    double clipPlane[4] = { 0.0, 1.0, 0.0, 0.0 };
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0, clipPlane);
    glCallList(domeDrawIndex);
    glDisable(GL_CLIP_PLANE0);
}

void radomeApp::drawGroundPlane() {
    float size = 300 * 5;
    float ticks = 40.0;
    
    float step = size / ticks;
    float major =  step * 2.0f;
    
    for (float k =- size; k <= size; k += step) {
        if (fabs(k) == size || k == 0)
            ofSetLineWidth(4);
        else if (k / major == floor(k / major))
            ofSetLineWidth(2);
        else
            ofSetLineWidth(1);
        
        ofLine(k, 0, -size, k, 0, size);
        ofLine(-size, 0, k, size, 0, k);
    }
}

void radomeApp::keyPressed(int key) {
    
    if (_pInputUI && _pInputUI->isVisible())
        return;
    
    float accel = 3.0;
    auto model = *(_modelList.rbegin());
    
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
                _winManager.setFullscreen(1, true);
            }
            break;
        case 'p':
            {
                showProjectorWindow();
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
                if (_modelList.size() > 0)
                {
                    auto m = _modelList.back();
                    _modelList.pop_back();
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
        (!_pCalibrationUI || !_pCalibrationUI->isVisible() || x > (SIDEBAR_WIDTH + CALIBRATIONUI_WIDTH + 10))) {
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
    
    if (matchRadioButton(name, _mixModeNames, &_mixMode))
        return;

    if (matchRadioButton(name, _mappingModeNames, &_mappingMode))
        return;
            
    if (name == "XFADE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _vidOverlay.setFaderValue(slider->getValue());
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
            showProjectorWindow();
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
            _vidOverlay.setApplicationName(trim(pInput->getTextString()));
        }
    } else if(name == "SYPHON_SERVER") {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            _vidOverlay.setServerName(trim(pInput->getTextString()));
        }
    } else if (name == "P1 HEIGHT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setHeight(slider->getScaledValue() * 10);
        }
    } else if (name == "P1 HEADING") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setHeading(slider->getScaledValue());
        }
    } else if (name == "P1 DISTANCE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setDistance(slider->getScaledValue() * 10);
        }
    } else if (name == "P1 FOV") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setFOV(slider->getScaledValue());
        }
    } else if (name == "P1 TARGET") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setTargetHeight(slider->getScaledValue() * 10);
        }
    } else if (name == "P1 SHIFT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setLensOffsetY(slider->getScaledValue());
        }
    } else if (name == "P1 ROLL") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[0]->setRoll(slider->getScaledValue());
        }
    } else if (name == "P2 HEIGHT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 1 && _projectorList[1]) {
            _projectorList[1]->setHeight(slider->getScaledValue() * 10);
        }
    } else if (name == "P2 HEADING") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 1 && _projectorList[1]) {
            _projectorList[1]->setHeading(slider->getScaledValue());
        }
    } else if (name == "P2 DISTANCE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 1 && _projectorList[1]) {
            _projectorList[1]->setDistance(slider->getScaledValue() * 10);
        }
    } else if (name == "P2 FOV") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 1 && _projectorList[1]) {
            _projectorList[1]->setFOV(slider->getScaledValue());
        }
    } else if (name == "P2 TARGET") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 1 && _projectorList[1]) {
            _projectorList[1]->setTargetHeight(slider->getScaledValue() * 10);
        }
    } else if (name == "P2 SHIFT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 1 && _projectorList[1]) {
            _projectorList[1]->setLensOffsetY(slider->getScaledValue());
        }
    } else if (name == "P2 ROLL") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[1]->setRoll(slider->getScaledValue());
        }
    } else if (name == "P3 HEIGHT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 2 && _projectorList[2]) {
            _projectorList[2]->setHeight(slider->getScaledValue() * 10);
        }
    } else if (name == "P3 HEADING") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 2 && _projectorList[2]) {
            _projectorList[2]->setHeading(slider->getScaledValue());
        }
    } else if (name == "P3 DISTANCE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 2 && _projectorList[2]) {
            _projectorList[2]->setDistance(slider->getScaledValue() * 10);
        }
    } else if (name == "P3 FOV") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 2 && _projectorList[2]) {
            _projectorList[2]->setFOV(slider->getScaledValue());
        }
    } else if (name == "P3 TARGET") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 2 && _projectorList[2]) {
            _projectorList[2]->setTargetHeight(slider->getScaledValue() * 10);
        }
    } else if (name == "P3 SHIFT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 2 && _projectorList[2]) {
            _projectorList[2]->setLensOffsetY(slider->getScaledValue());
        }
    } else if (name == "P3 ROLL") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider && _projectorList.size() > 0 && _projectorList[0]) {
            _projectorList[2]->setRoll(slider->getScaledValue());
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
            _showTestPattern = pButton->getValue();
        }
    } else if (name == "CONTRAST") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _contrast = slider->getScaledValue();
        }
    } else if (name == "SATURATION") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _saturation = slider->getScaledValue();
        }
    } else if (name == "BRIGHTNESS") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _brightness = slider->getScaledValue();
        }
    } else if (name == "DOME HEIGHT") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _domeHeight = slider->getScaledValue() * 10;
            prepDrawList();
        }
    } else if (name == "DOME DIAMETER") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _domeDiameter = slider->getScaledValue() * 10;
            prepDrawList();
        }
    } else if (name == "DOME SLICE") {
        auto slider = dynamic_cast<ofxUISlider*>(e.widget);
        if (slider) {
            _domeSliceParameterization = slider->getScaledValue();
            prepDrawList();
        }
    }
    for (auto plug : PluginLibrary::getList()) {
        if (typeid(*plug).name() == name) {
            plug->setEnabled(!plug->isEnabled());
        }
    }
}