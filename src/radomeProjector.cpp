//
//  radomeProjector.cpp
//  radome
//
//  Created by Michael Dewberry on 3/8/13.
//
//

#include "radomeProjector.h"

radomeProjector::radomeProjector(float heading, float distance, float height, float fov, float targetHeight)
: _heading(heading)
, _distance(distance)
, _height(height)
, _fov(fov)
, _targetHeight(targetHeight)
, _lensOffsetX(0)
, _lensOffsetY(0)
, _roll(0)
, _isJoined(true)
{
    _camera.setupPerspective();
    updateCamera();
    
    _resolution = ofVec2f(1280.0, 1024.0);
    
    _fbo.allocate(_resolution.x, _resolution.y, GL_RGB);
    _fbo.begin();
	ofClear(0,0,0);
    _fbo.end();
}

void radomeProjector::updateCamera() {
    ofVec2f offset(_lensOffsetX, _lensOffsetY);
    
    //_camera.setupPerspective(true, _fov, 1.0, 10000.0, offset);
    //_camera.enableOrtho();
    _camera.setFov(_fov);
    _camera.setNearClip(1.0);
    _camera.setFarClip(10000.0);
    _camera.setLensOffset(offset);
    
    float cameraRadians = (_heading) * PI / 180.0;
    ofVec3f cameraPos(_distance * cos(cameraRadians), _height, _distance * sin(cameraRadians));
    ofVec3f lookAt(0, _targetHeight, 0);
    ofVec3f lookVec = cameraPos - lookAt;

    ofVec3f upVec(0, -1, 0);
    ofQuaternion q;
    q.makeRotate(_roll, lookVec);
    upVec = q * upVec;
    
    _camera.setPosition(cameraPos);
    _camera.lookAt(ofVec3f(0.0, _targetHeight, 0.0), upVec);
}

void radomeProjector::renderBegin()
{
    _fbo.begin();
	ofClear(0,0,0,1);
    _camera.begin();
}

void radomeProjector::renderEnd()
{
    _camera.end();
    _fbo.end();
}

void radomeProjector::drawFramebuffer(int x, int y, int w, int h) {
    _fbo.draw(x, y, w, h);
}

void radomeProjector::drawSceneRepresentation() {
    ofPushMatrix();
    ofPushStyle();

    ofRotateY(_heading);
    ofTranslate(_distance, 0);
    
    for (int pass = 0; pass < 2; pass++)
    {
        if (pass == 0) {
            ofNoFill();
            ofSetColor(40, 191, 80);
        } else {
            ofFill();
            ofSetColor(10, 80, 40);
        }
        
        ofSetLineWidth(2.0);
        ofDrawBox(0.0, _height, 0.0, 20, 10, 20);

        // draw tripod
        ofSetLineWidth(5.0);
        float r = 40;
        for (int i = 0; i < 3; i++) {
            float theta = PI * (2 * i/3.0 + 1);
            ofLine(0.0, _height - 5.0, 0.0, r * cos(theta), 0.0, r * sin(theta));
        }
    }
    
    ofPopStyle();
    ofPopMatrix();
}

/*
radomeProjectorWindowListener::radomeProjectorWindowListener(vector<radomeProjector*>* pProjectors) {
    _pProjectors = pProjectors;
    _screenshot = false;
}

void radomeProjectorWindowListener::setup() {
    
}

void radomeProjectorWindowListener::draw() {
    if (_pProjectors) {
        int w = ofGetWidth() / _pProjectors->size();
        int h = ofGetHeight();
        int x = 0;
        for (auto iter = _pProjectors->begin(); iter != _pProjectors->end(); ++iter) {
            (*iter)->drawFramebuffer(x, 0, w, h);
            x += w;
        }
        if (_screenshot) {
            ofSaveScreen("outputMask.jpg");
            _screenshot = false;
        }
    }
}
*/
