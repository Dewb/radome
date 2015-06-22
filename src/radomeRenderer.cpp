//
//  radomeRenderer.cpp
//  radome
//
//  Created by Michael Dewberry on 6/21/15.
//
//

#include "ofxCubeMap.h"
#include "ofxAutoReloadedShader.h"
#include "ofxAssimpModelLoader.h"
#include "icosohedron.h"

#include "radomeRenderer.h"
#include "radomeProjector.h"
#include "radomePlugin.h"
#include "radomeSyphonClient.h"
#include "radomeModel.h"
#include "radomeUtils.h"



class radomeRendererImpl {
public:
    radomeRendererImpl(radomeRenderer* interface);
    ~radomeRendererImpl();

    void setup();
    void render(vector<radomeProjector*>& projectorList);
    void drawScenePreview(DisplayMode mode, ofxTurntableCam& camera, vector<radomeProjector*>& projectorList, int sidebarWidth);

    void updateDomeModel();

protected:
    friend class radomeRenderer;

    void renderToCubeMap();
    void renderProjectorBuffers(vector<radomeProjector*>& projectorList);

    void drawScene();
    void drawDome();
    void drawGroundPlane();

    void beginShader();
    void endShader();

    unsigned int _domeDrawIndex;

    float _animationTime;
    unsigned long long _lastSystemTime;

    vector<icosohedron::Triangle> _triangles;

    ofxCubeMap _cubeMap;
    ofxAutoReloadedShader _shader;
    radomeSyphonClient _vidOverlay;
    ofImage _testPatternImage;
    list<radomeModel*> _modelList;

    radomeRenderer* _interface;

};

radomeRendererImpl::radomeRendererImpl(radomeRenderer* interface)
: _domeDrawIndex(-1)
, _interface(interface)
{
}

radomeRendererImpl::~radomeRendererImpl() {
    deletePointerCollection(_modelList);
}



radomeRenderer::radomeRenderer()
: _impl(new radomeRendererImpl(this))
{
}

radomeRenderer::~radomeRenderer() {
    delete _impl;
}

void radomeRenderer::setup() {
    params.contrast = 1.0;
    params.saturation = 1.0;
    params.brightness = 1.0;

    params.mixMode = 0;
    params.mappingMode = 0;

    params.inputOpacity = 1.0;
    params.showTestPattern = false;

    if (_impl) {
        _impl->setup();
    }
}

void radomeRenderer::render(vector<radomeProjector*>& projectorList) {
    if (_impl) {
        _impl->render(projectorList);
    }
}

void radomeRenderer::drawScenePreview(DisplayMode mode, ofxTurntableCam &camera, vector<radomeProjector*>& projectorList, int sidebarWidth) {
    if (_impl) {
        _impl->drawScenePreview(mode, camera, projectorList, sidebarWidth);
    }
}

void radomeRenderer::updateDomeModel() {
    if (_impl) {
        _impl->updateDomeModel();
    }
}

void radomeRenderer::addModelFromFile(string path) {
    if (_impl) {
        auto model = new radomeModel();
        model->loadModel(path);
        _impl->_modelList.push_back(model);
    }
}

ModelList& radomeRenderer::getModelList() {
    if (_impl) {
        return _impl->_modelList;
    }
}

void radomeRenderer::setInputApplicationName(string app) {
    if (_impl) {
        _impl->_vidOverlay.setApplicationName(app);
    }
}

void radomeRenderer::setInputServerName(string server) {
    if (_impl) {
        _impl->_vidOverlay.setServerName(server);
    }
}

void radomeRenderer::drawInputVideo(int x, int y, int w) {
    if (_impl) {
        float inv_aspect = _impl->_vidOverlay.getHeight() / _impl->_vidOverlay.getWidth();
        _impl->_vidOverlay.getTexture().draw(x, y, w, w * inv_aspect);
    }
}


void radomeRendererImpl::setup() {
    _shader.load("radome");

    _cubeMap.initEmptyTextures(2048, GL_RGBA);
    _cubeMap.setNearFar(ofVec2f(0.01, 8192.0));

    _vidOverlay.initialize("", "");

    _testPatternImage.setUseTexture(true);
    _testPatternImage.loadImage("testpatt.jpg");

    _animationTime = 0.0;
    _lastSystemTime = ofGetSystemTime();

    _triangles = icosohedron::create(4);
}

void radomeRendererImpl::updateDomeModel()
{
    _domeDrawIndex = glGenLists(1);
    glNewList(_domeDrawIndex, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    float yshift = (1 - _interface->params.domeCurve) * _interface->params.domeHeight;

    // desired slice radius b = sqrt(radius * radius - yshift * yshift);
    float b = _interface->params.domeDiameter/2.0;
    float radius = sqrt(b * b + yshift * yshift);

    ofVec3f d, s = ofVec3f(radius, _interface->params.domeHeight + yshift, radius);
    for (auto& t : _triangles) {
        d = t.v0 * s;
        d.y -= yshift;
        glNormal3fv(&d[0]);
        d.y += _interface->params.domeBaseHeight;
        glVertex3fv(&d[0]);

        d = t.v1 * s;
        d.y -= yshift;
        glNormal3fv(&d[0]);
        d.y += _interface->params.domeBaseHeight;
        glVertex3fv(&d[0]);

        d = t.v2 * s;
        d.y -= yshift;
        glNormal3fv(&d[0]);
        d.y += _interface->params.domeBaseHeight;
        glVertex3fv(&d[0]);
    }
    glEnd();
    glEndList();
}

void radomeRendererImpl::render(vector<radomeProjector*>& projectorList) {

    unsigned long long time = ofGetSystemTime();
    _animationTime += (time - _lastSystemTime)/1000.0;
    _lastSystemTime = time;

    if (_animationTime >= 1.0) {
        _animationTime = 0.0;
    }
    for (auto iter = _modelList.begin(); iter != _modelList.end(); ++iter) {
        (*iter)->update(_animationTime);
    }

    renderToCubeMap();
    renderProjectorBuffers(projectorList);
}

void radomeRendererImpl::renderToCubeMap() {
    glEnable(GL_DEPTH_TEST);
    _cubeMap.setPosition(0, 0, 0);
    LineWidthAdjuster::factor = 4.0;

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

void radomeRendererImpl::renderProjectorBuffers(vector<radomeProjector*>& projectorList) {
    glEnable(GL_DEPTH_TEST);
    for (auto iter = projectorList.begin(); iter != projectorList.end(); ++iter) {
        (*iter)->renderBegin();

        beginShader();
        drawDome();
        endShader();

        (*iter)->renderEnd();
    }
}

void radomeRendererImpl::beginShader() {
    _shader.begin();
    _cubeMap.bind();

    _shader.setUniform1i("EnvMap", 0);
    _shader.setUniform1i("mixMode", _interface->params.mixMode);
    _shader.setUniform1i("mappingMode", _interface->params.mappingMode);
    _shader.setUniform1f("domeDiameter", _interface->params.domeDiameter);
    _shader.setUniform1f("domeHeight", _interface->params.domeHeight);
    _shader.setUniform1f("contrast", _interface->params.contrast);
    _shader.setUniform1f("saturation", _interface->params.saturation);
    _shader.setUniform1f("brightness", _interface->params.brightness);
    _shader.setUniform1f("baseHeight", 0);

    if (_interface->params.showTestPattern) {
        _shader.setUniform1f("videoMix", 0.0);
        _shader.setUniform2f("videoSize", _testPatternImage.getWidth(), _testPatternImage.getHeight());
        _shader.setUniformTexture("video", _testPatternImage.getTextureReference(), 1);
    } else if (_vidOverlay.maybeBind()) {
        _shader.setUniform1f("videoMix", _interface->params.inputOpacity);
        _shader.setUniform2f("videoSize", _vidOverlay.getWidth(), _vidOverlay.getHeight());
        _shader.setUniformTexture("video", _vidOverlay.getTexture(), 1);
    } else {
        _shader.setUniform1f("videoMix", -1.0);
        _shader.setUniform2f("videoSize", 0.0, 0.0);
        _shader.setUniformTexture("video", _testPatternImage.getTextureReference(), 1);
    }
}

void radomeRendererImpl::endShader() {
    if (_vidOverlay.isBound())
        _vidOverlay.unbind();
    _cubeMap.unbind();
    _shader.end();
}

void radomeRendererImpl::drawScene() {
    ofSetColor(180, 192, 192);

    // Draw static and animated 3D models
    for (auto model : _modelList)
    {
        model->draw();
    }

    DomeInfo dome;
    dome.height = _interface->params.domeHeight;
    dome.radius = _interface->params.domeDiameter/2;
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

void radomeRendererImpl::drawDome() {
    double clipPlane[4] = { 0.0, 1.0, 0.0, -_interface->params.domeBaseHeight };
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0, clipPlane);
    glCallList(_domeDrawIndex);
    glDisable(GL_CLIP_PLANE0);
}

void radomeRendererImpl::drawGroundPlane() {
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

void radomeRendererImpl::drawScenePreview(DisplayMode mode, ofxTurntableCam& cam, vector<radomeProjector*>& projectorList, int sidebarWidth) {
    switch (mode) {
        case DisplayScene: {
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofClear(0, 0, 0);

            cam.setDistance(_interface->params.domeDiameter*1.6);
            cam.begin();

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

            cam.end();

            ofPopStyle();
        }
            break;
#ifdef SHOW_CUBE_MAP
        case DisplayCubeMap: {
            ofClear(0, 0, 0);
            ofSetColor(200,220,255);
            int margin = 2;
            int w = (ofGetWindowWidth() - sidebarWidth - margin*4) / 3;
            int h = (ofGetWindowHeight() - margin*3) / 2;
            for (int i = 0; i < 6; i++) {
                int x = margin + i%3 * (w + margin) + sidebarWidth;
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

            cam.setDistance(_interface->params.domeDiameter*1.10);
            cam.begin();

            beginShader();
            _shader.setUniform1f("baseHeight", _interface->params.domeBaseHeight);
            drawDome();
            _shader.setUniform1f("baseHeight", 0);
            drawGroundPlane();
            endShader();

            for (auto iter = projectorList.begin(); iter != projectorList.end(); ++iter) {
                (*iter)->drawSceneRepresentation();
            }

            cam.end();
        }
            break;
        case DisplayProjectorOutput:
        case LastDisplayMode: {
            ofClear(0, 0, 0);
            ofSetColor(200,220,255);
            int margin = 2;
            int w = (ofGetWindowWidth() - sidebarWidth - margin*4) / 2;
            int h = (ofGetWindowHeight() - margin*3) / 2;
            auto iter = projectorList.begin();
            for (int i = 0; iter != projectorList.end(); i++, ++iter) {
                int x = margin + i%2 * (w + margin) + sidebarWidth;
                int y = margin + i/2 * (h + margin);
                (*iter)->drawFramebuffer(x, y, w, h);
                ofRect(x-1, y-1, w + margin, h + margin);
            }
        }
            break;            
    }
}

