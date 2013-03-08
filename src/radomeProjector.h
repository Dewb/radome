//
//  radomeProjector.h
//  radome
//
//  Created by Michael Dewberry on 3/8/13.
//
//

#ifndef __radome__radomeProjector__
#define __radome__radomeProjector__

#include "ofMain.h"

class radomeProjector {
public:
    radomeProjector(float heading, float distance, float height);
    void drawSceneRepresentation();
    
    void renderToProjector();
    
    ofTexture& getOutputTextureReference() { return _image.getTextureReference(); }
    
    void setHeading(float h) { _heading = h; updateCamera(); }
    float getHeading() const { return _heading; }
    void setDistance(float d) { _distance = d; updateCamera(); }
    float getDistance() const { return _distance; }
    void setHeight(float h) { _height = h; updateCamera(); }
    float getHeight() const { return _height; }
    
protected:
    void updateCamera();
    
    ofCamera _camera;
    ofFbo _fbo;
    ofImage _image;
    float _heading;
    float _distance;
    float _height;
};

#endif /* defined(__radome__radomeProjector__) */
