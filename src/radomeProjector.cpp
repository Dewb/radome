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
{
    updateCamera();
    
    _resolution = ofVec2f(1280.0, 1024.0);
    
    _fbo.allocate(_resolution.x, _resolution.y, GL_RGB);
    _fbo.begin();
	ofClear(0,0,0);
    _fbo.end();
}

void radomeProjector::updateCamera() {
    ofVec2f offset(_lensOffsetX, _lensOffsetY);
    _camera.setupPerspective(true, _fov, 0, 0, offset);
    _camera.setPosition(_distance * cos(_heading*3.14159/180.0), _height, _distance * sin(_heading*3.14159/180.0));
    _camera.lookAt(ofVec3f(0.0, _targetHeight, 0.0));
}

void radomeProjector::renderBegin()
{
    _fbo.begin();
	ofClear(0,0,0);
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
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            ofSetColor(40, 191, 80);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            ofSetColor(10, 80, 40);
        }
        
        ofSetLineWidth(2.0);
        ofBox(0.0, _height, 0.0, 30);
        ofSetLineWidth(5.0);
        ofLine(0.0, _height-15.0, 0.0, 0.0, 0.0, 0.0);
    }
    
    ofPopStyle();
    ofPopMatrix();
}


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