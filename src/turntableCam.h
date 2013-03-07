#pragma once

#include "ofCamera.h"
#include "ofEvents.h"

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
	
protected:
    void update(ofEventArgs& args);
    
    float _rotationFactor;
    bool _mouseDown;
    ofVec2f _mouseStart;
    
    float _distance;
	ofNode _target;
	ofQuaternion _rotation;
	ofRectangle _viewport;
};
    