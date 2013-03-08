#include "turntableCam.h"

#include "ofMath.h"
#include "ofUtils.h"

ofxTurntableCam::ofxTurntableCam() {
    reset();
    ofAddListener(ofEvents().update , this, &ofxTurntableCam::update);
    _mouseDown = false;
    setTarget(ofVec3f(0,0,0));
}

ofxTurntableCam::~ofxTurntableCam() {
    ofRemoveListener(ofEvents().update , this, &ofxTurntableCam::update);
}

void ofxTurntableCam::update(ofEventArgs& args) {
    _rotationFactor = 10.0f * 180 / min(_viewport.width, _viewport.height);
    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
    if (_viewport.inside(mouse.x, mouse.y) && !_mouseDown && ofGetMousePressed()) {
        _mouseStart = mouse;
        _mouseDown = true;
    }
    if (_mouseDown) {
        if (!ofGetMousePressed()) {
            _mouseDown = false;
        } else {            
            float dx = (mouse.x - _mouseStart.x) / ofGetViewportWidth();
            float dy = (mouse.y - _mouseStart.y) / ofGetViewportHeight();
            _mouseStart = mouse;
            setRotation(dx,dy);
        }
    }
}

void ofxTurntableCam::setRotation(float dx, float dy) {
    ofVec3f p = ofCamera::getPosition();
    ofVec3f uy = ofCamera::getUpDir();
    ofVec3f ux = ofCamera::getSideDir();
    
    ofVec3f arcEnd(dx, -dy, -0.5f);
    arcEnd = arcEnd;
    arcEnd.normalize();
    ofQuaternion orientation = this->getGlobalOrientation();
    _rotation.makeRotate(orientation * ofVec3f(0.0f, 0.0f, -1.0f), orientation * arcEnd);
    
    ofQuaternion rotToUp;
    ofVec3f sideDir = ofCamera::getSideDir() * _rotation;
    rotToUp.makeRotate(sideDir, sideDir * ofVec3f(1.0f, 0, 1.0f));
    _rotation *= rotToUp;
    
    this->setOrientation(this->getGlobalOrientation() * _rotation);
    
    ofVec3f t = _target.getPosition();
    ofCamera::setPosition((p - t) * _rotation + t);
}

void ofxTurntableCam::begin(ofRectangle viewport) {
    _viewport = viewport;
    ofCamera::begin(_viewport);
}

void ofxTurntableCam::reset() {
    _target.resetTransform();
    
    _target.setPosition(0,0, 0);
    lookAt(_target);

    resetTransform();
    setPosition(0, 0, _distance);
}

void ofxTurntableCam::setTarget(const ofVec3f& targetPoint){
    _target.setPosition(targetPoint);
    lookAt(_target);
}

ofVec3f ofxTurntableCam::getTarget() {
    return _target.getPosition();
}

void ofxTurntableCam::setDistance(float distance){
    _distance = distance;
    setPosition(_target.getPosition() + (distance * getZAxis()));
}

float ofxTurntableCam::getDistance() const {
    return _target.getPosition().distance(getPosition());
}

