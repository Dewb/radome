//
//  radomeModel.h
//  radome
//
//  Created by Michael Dewberry on 3/9/13.
//
//

#ifndef __radome__radomeModel__
#define __radome__radomeModel__

#include "ofxAssimpModelLoader.h"

class radomeModel : public ofxAssimpModelLoader {
public:
    void update(float t);
    void draw();

    ofVec3f getOrigin() const { return _origin; }
    void setOrigin(ofVec3f o) { _origin = o; }

    ofVec4f getRotation() const { return _rotation; }
    void setRotation(ofVec4f r) { _rotation = r; }

    ofVec3f getRotationOrigin() const { return _rotationOrigin; }
    void setRotationOrigin(ofVec3f o) { _rotationOrigin = o; }

    float getRotationIncrement() const { return _rotationIncrement; }
    void setRotationIncrement(float f) { _rotationIncrement = f; }
    
    ofVec3f _origin;

protected:
    ofVec4f _rotation;
    ofVec3f _rotationOrigin;
    float _rotationIncrement;
};

#endif /* defined(__radome__radomeModel__) */
