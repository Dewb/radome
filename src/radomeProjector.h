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

#include <list>
using std::list;

class radomeProjector {
public:
    radomeProjector(float heading, float distance, float height, float fov = 30, float targetHeight = 20);
    void drawSceneRepresentation();
    void drawFramebuffer(int x, int y, int w, int h);
    
    void renderBegin();
    void renderEnd();
    
    void setHeading(float h) { _heading = h; updateCamera(); }
    float getHeading() const { return _heading; }
    void setDistance(float d) { _distance = d; updateCamera(); }
    float getDistance() const { return _distance; }
    void setHeight(float h) { _height = h; updateCamera(); }
    float getHeight() const { return _height; }
    void setFOV(float f) { _fov = f; updateCamera(); }
    float getFOV() const { return _fov; }
    void setTargetHeight(float h) { _targetHeight = h; updateCamera(); }
    float getTargetHeight() const { return _targetHeight; }
    void setLensOffsetX(float x) { _lensOffsetX = x; updateCamera(); }
    float getLensOffsetX() const { return _lensOffsetX; }
    void setLensOffsetY(float y) { _lensOffsetY = y; updateCamera(); }
    float getLensOffsetY() const { return _lensOffsetY; }
    float getRoll() const { return _roll; }
    void setRoll(float r) { _roll = r; updateCamera(); }
    float getYaw() const { return _yaw; }
    void setYaw(float y) { _yaw = y; updateCamera(); }

    bool isJoined() const { return _isJoined; }
    void setJoined(bool b) { _isJoined = b; }
        
protected:
    void updateCamera();
    
    ofCamera _camera;
    ofFbo _fbo;

    ofVec2f _resolution;
    
    float _heading;
    float _distance;
    float _height;
    float _fov;
    float _targetHeight;
    float _lensOffsetX;
    float _lensOffsetY;
    float _roll;
    float _yaw;

    bool _isJoined;
};

#endif /* defined(__radome__radomeProjector__) */
