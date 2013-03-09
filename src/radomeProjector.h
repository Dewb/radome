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
#include "ofxFenster.h"

#include <list>
using std::list;

class radomeProjector {
public:
    radomeProjector(float heading, float distance, float height);
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
    
protected:
    void updateCamera();
    
    ofCamera _camera;
    ofFbo _fbo;

    float _heading;
    float _distance;
    float _height;
};

class radomeProjectorWindowListener : public ofxFensterListener {
public:
    radomeProjectorWindowListener(list<radomeProjector*>* pProjectors);
    void setup();
    void draw();
protected:
    list<radomeProjector*>* _pProjectors;
};

#endif /* defined(__radome__radomeProjector__) */
