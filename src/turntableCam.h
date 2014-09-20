#pragma once

// ofxTurntableCam: an ofEasyCam-inspired camera that does turntable rotation

#include "ofCamera.h"
#include "ofEvents.h"

//#define USE_OFXFENSTER

class ofxTurntableCam : public ofCamera {
public:
    ofxTurntableCam();
    ~ofxTurntableCam();

    virtual void begin(ofRectangle viewport = ofGetCurrentViewport());
    void reset();

    void setTarget(const ofVec3f& target);
    ofVec3f getTarget();

    void setDistance(float distance);
    float getDistance() const;
    
    void setRotation(float dx, float dy);

    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

protected:
    float _rotationFactor;
    bool _cameraDragging;
    ofVec2f _mouseStart;
    
    float _distance;
    ofNode _target;
    ofQuaternion _rotation;
    ofRectangle _viewport;
};
    