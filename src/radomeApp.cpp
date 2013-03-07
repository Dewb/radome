#include "radomeApp.h"

#define SIDEBAR_WIDTH 150
#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"
#define DOME_DIAMETER 300
#define DOME_HEIGHT 110

void radomeApp::setup() {
    ofSetFrameRate(45);
    ofEnableSmoothing();
    _fullscreen = false;
    
    _cubeMap.initEmptyTextures(2048, GL_RGBA);
    _shader.load("radome");
    _cubeMap.setNearFar(ofVec2f(0.01, 8192.0));
    
    _domeOrigin.x = 0.0;
    _domeOrigin.y = 0.0;
    _domeOrigin.z = 0.0;
    
    _cam.setRotation(0.66, 0.5);
    _cam.setTarget(ofVec3f(0.0, DOME_HEIGHT*0.5, 0.0));

    _displayMode = DisplayScene;
    _mixMode = 0;
    _mappingMode = 0;
    
    _triangles = icosohedron::createsphere(4);
    
	_vidOverlay.initialize(DEFAULT_SYPHON_APP, DEFAULT_SYPHON_SERVER);
    _vidOverlay.setFaderValue(0.75);
    
    _animationTime = 0.0;
    
    initGUI();
}

namespace
{
    void addRadioAndSetFirstItem(ofxUICanvas* pUI, string id, vector<string> options, int orientation, int w, int h)
    {
        ofxUIToggle* pToggle;
        pUI->addRadio(id, options, orientation, w, h);
        {
            // so silly, ofxUI needs to init radios
            auto pToggle = dynamic_cast<ofxUIToggle*>(pUI->getWidget(options[0]));
            if (pToggle)
                pToggle->setValue(true);
        }
    }
}

void radomeApp::initGUI() {
    _pUI = new ofxUICanvas(0, 0, SIDEBAR_WIDTH, ofGetHeight());
    _pUI->setWidgetSpacing(5.0);
    _pUI->setDrawBack(true);
    
    _pUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
	_pUI->addWidgetDown(new ofxUILabel("RADOME 0.1", OFX_UI_FONT_LARGE));
    _pUI->addSpacer(0, 12);
    
    _displayModeNames.push_back("3D Scene");
    _displayModeNames.push_back("Cube Map");
    _displayModeNames.push_back("Dome Preview");
    _displayModeNames.push_back("Output Preview");
	addRadioAndSetFirstItem(_pUI, "DISPLAY MODE", _displayModeNames, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addSpacer(0, 12);

	_pUI->addWidgetDown(new ofxUILabel("PROJECTORS", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUILabelButton("Windows...", false, 0, 30, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addSpacer(0, 12);

	_pUI->addWidgetDown(new ofxUILabel("CONTENT", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUILabelButton("Add 3D Model...", false, 0, 30, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ofxUILabelButton("Select 2D Source...", false, 0, 30, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ofxUILabelButton("Plugins...", false, 0, 30, 0, 0, OFX_UI_FONT_SMALL));
    _pUI->addSpacer(0, 12);

    _pUI->addWidgetDown(new ofxUILabel("MIXER", OFX_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ofxUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-20, 30, 0, 100, _vidOverlay.getFaderValue()*100.0, "XFADE", "2D", "3D", OFX_UI_FONT_MEDIUM));
    
    _mixModeNames.push_back("Underlay");
    _mixModeNames.push_back("Overlay");
    _mixModeNames.push_back("Mask");
	addRadioAndSetFirstItem(_pUI, "BLEND MODE", _mixModeNames, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addSpacer(0, 12);
    
    _mappingModeNames.push_back("Lat/Long");
    _mappingModeNames.push_back("Quadrants");
    _mappingModeNames.push_back("Geodesic");
    _mappingModeNames.push_back("Rectangles");
    _mappingModeNames.push_back("Cinematic");
	addRadioAndSetFirstItem(_pUI, "MAPPING MODE", _mappingModeNames, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addSpacer(0, 12);
    
    ofBackground(40, 20, 32);

    ofAddListener(_pUI->newGUIEvent, this, &radomeApp::guiEvent);
}

void radomeApp::loadFile() {
    ofFileDialogResult result = ofSystemLoadDialog("Load Model", false, "");
    if (result.bSuccess) {
        ofxAssimpModelLoader model;
        model.loadModel(result.getPath());
        _modelList.push_back(model);
    }
}

void radomeApp::update() {
    _animationTime += ofGetLastFrameTime();
    if (_animationTime >= 1.0) {
        _animationTime = 0.0;
    }
    for (auto iter = _modelList.begin(); iter != _modelList.end(); ++iter)
    {
        iter->setNormalizedTime(_animationTime);
    }
}

void radomeApp::updateCubeMap() {
    _cubeMap.setPosition(_domeOrigin);
    for(int i = 0; i < 6; i++)
    {
        _cubeMap.beginDrawingInto3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        ofClear(0,0,0,0);
        drawScene();
        _cubeMap.endDrawingInto3D();
    }
}

void radomeApp::draw() {
    glEnable(GL_DEPTH_TEST);

    switch (_displayMode) {
        case DisplayScene: {
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);

            _cam.setDistance(DOME_DIAMETER*3.3);
            _cam.begin();
            
            ofPushMatrix();
            ofTranslate(-_domeOrigin);
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
        case DisplayCubeMap: {
            updateCubeMap();
            ofSetColor(200,220,255);
            int margin = 2;
            int w = (ofGetWindowWidth() - SIDEBAR_WIDTH - margin*4) / 3;
            int h = (ofGetWindowHeight() - margin*3) / 2;
            for (int i = 0; i < 6; i++) {
                int x = margin + i%3 * (w + margin) + SIDEBAR_WIDTH;
                int y = margin + i/3 * (h + margin);
                ofDrawBitmapString(_cubeMap.getDescriptiveStringForFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), x+margin*1.5, y+10+margin*1.5);
                _cubeMap.drawFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i , x, y, w, h);
                ofRect(x-1, y-1, w + margin, h + margin);
            }
        }
        break;
        case DisplayDome: {
            updateCubeMap();

            ofClear(20, 100, 50);
            
            _shader.begin();

            _cubeMap.bind();
            _shader.setUniform1i("EnvMap", 0);
            
            if (_vidOverlay.maybeBind()) {
                _shader.setUniform1i("mixMode", _mixMode);
                _shader.setUniform1i("mappingMode", _mappingMode);
                _shader.setUniform1f("domeDiameter", DOME_DIAMETER*1.0);
                _shader.setUniform1f("domeHeight", DOME_HEIGHT*1.0);
                _shader.setUniform1f("videoMix", _vidOverlay.getFaderValue());
                _shader.setUniform2f("videoSize", _vidOverlay.getWidth(), _vidOverlay.getHeight());
                _shader.setUniformTexture("video",
                                                 _vidOverlay.getTexture(),
                                                 _vidOverlay.getTextureId());
            } else {
                _shader.setUniform1f("videoMix", 0.0);
            }
                        
            _cam.setDistance(DOME_DIAMETER*2.05);
            _cam.begin();
            drawDome();
            drawGroundPlane();
            _cam.end();

            _vidOverlay.unbind();
            _cubeMap.unbind();
            
            _shader.end();
            
        }
        break;
        case DisplayProjectorOutput:
        case LastDisplayMode: {
            
        }
        break;            
    }
    
    glDisable(GL_DEPTH_TEST);
}

void radomeApp::drawScene() {
    ofSetColor(180, 192, 192);
    for (auto iter = _modelList.begin(); iter != _modelList.end(); ++iter)
    {
        iter->drawFaces();
    }
}

void radomeApp::drawDome() {
    double clipPlane[4] = { 0.0, 1.0, 0.0, 0.0 };
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0, clipPlane);
    
    if (0) {
        ofSphere(0.0, 0.0, 0.0, 300.0);
    } else {
        
        // TODO: move this to a VBO, performance is terrible
        
        std::vector<icosohedron::Triangle>::iterator i = _triangles.begin();
        glBegin(GL_TRIANGLES);
        int sx = DOME_DIAMETER, sy = DOME_HEIGHT*2, sz = DOME_DIAMETER;
        while (i != _triangles.end())
        {
            icosohedron::Triangle& t = *i++;

            float dx, dy, dz;

            dx = t.v0[0] * sx;
            dy = t.v0[1] * sy;
            dz = t.v0[2] * sz;
            //glTexCoord2f(0.5 + atan2(dz, dx)/(2*3.14159), 0.5 - 2*asin(dy)/(2*3.14159));
            glNormal3f(dx, dy, dz);
            glVertex3f(dx, dy, dz);
            
            dx = t.v1[0] * sx;
            dy = t.v1[1] * sy;
            dz = t.v1[2] * sz;
            //glTexCoord2f(0.5 + atan2(dz, dx)/(2*3.14159), 0.5 - 2*asin(dy)/(2*3.14159));
            glNormal3f(dx, dy, dz);
            glVertex3f(dx, dy, dz);
            
            dx = t.v2[0] * sx;
            dy = t.v2[1] * sy;
            dz = t.v2[2] * sz;
            //glTexCoord2f(0.5 + atan2(dz, dx)/(2*3.14159), 0.5 - 2*asin(dy)/(2*3.14159));
            glNormal3f(dx, dy, dz);
            glVertex3f(dx, dy, dz);
        }
        glEnd();
    }
    glDisable(GL_CLIP_PLANE0);
}

void radomeApp::drawGroundPlane() {
    float size = DOME_DIAMETER * 3.3;
    float ticks = 20.0;
    
    float step = size / ticks;
    float major =  step * 2.0f;
    
    for (float k =- size; k <= size; k += step)
    {
        if (fabs(k) == size || k == 0)
            ofSetLineWidth(4);
        else if (k / major == floor(k / major) )
            ofSetLineWidth(2);
        else
            ofSetLineWidth(1);
        
        ofLine(k, 0, -size, k, 0, size);
        ofLine(-size, 0, k, size, 0, k);
    }
}

void radomeApp::keyPressed(int key) {
    float accel = 2.0;
    
    switch (key) {
        case 'w': _domeOrigin.y -= accel; break;
        case 's': _domeOrigin.y += accel; break;
        case 'a': _domeOrigin.x += accel; break;
        case 'd': _domeOrigin.x -= accel; break;
        case 'z': _domeOrigin.z -= accel; break;
        case 'x': _domeOrigin.z += accel; break;
        case 'W': _domeOrigin.y -= accel * 10; break;
        case 'S': _domeOrigin.y += accel * 10; break;
        case 'A': _domeOrigin.x += accel * 10; break;
        case 'D': _domeOrigin.x -= accel * 10; break;
        case 'Z': _domeOrigin.z -= accel * 10; break;
        case 'X': _domeOrigin.z += accel * 10; break;
        case 'l': loadFile(); break;
        case 'm':
            {
                DisplayMode mode = getDisplayMode();
                mode++;
                changeDisplayMode(mode);
            }
            break;
        case 'f':
            _fullscreen = !_fullscreen;
            ofSetFullscreen(_fullscreen);
    }
}

void radomeApp::keyReleased(int key) {
    switch (key) {
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

namespace {
    bool matchRadioButton(string widgetName, vector<string> names, int* pValue)
    {
        for (int ii = 0; ii < names.size(); ii++) {
            if (widgetName == names[ii]) {
                if (pValue) {
                    *pValue = ii;
                }
                return true;
            }
        }
        return false;
    }
}

void radomeApp::guiEvent(ofxUIEventArgs &e) {
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
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
        if (pButton && pButton->getValue())
        {
            loadFile();
        }
    }
}