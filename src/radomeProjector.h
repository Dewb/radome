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
    void getRoll() const { return _roll; }
    float setRoll(float r) { _roll = r; updateCamera(); }
        
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
};

/*
class radomeProjectorWindowListener : public ofxFensterListener {
public:
    radomeProjectorWindowListener(vector<radomeProjector*>* pProjectors);
    void setup();
    void draw();
    
    void saveScreenshot() { _screenshot = true; }

protected:
    vector<radomeProjector*>* _pProjectors;
    bool _screenshot;
};
 */

#endif /* defined(__radome__radomeProjector__) */
