//
//  BleepoutPlugin.cpp
//  radome
//
//  Created by Dewb on 12/7/14.
//
//

#include "radomePlugin.h"

ofVec3f sphericalToCartesian(float theta, float phi, float r = 1.0) {
    float r_sin_theta = r * sin(theta);
    return ofVec3f(
        r_sin_theta * cos(phi),
        r_sin_theta * sin(phi),
        r * cos(theta)
    );
}

class CircleTrajectory {
public:
    CircleTrajectory(float radius, ofVec3f u, ofVec3f v, float speed = 0.02) {
        _u = u.normalized();
        _w = _u.getCrossed(v.normalized()).getCrossed(_u).normalized();
        _speed = speed;
        _t = 0;
        _r = radius;
        position = _u * radius;
    }
    void tick() {
        _t += _speed;
        position = _r * (_u * cos(_t) + _w * sin(_t));
        if (position.y <= 0.0) {
            _u.y *= -1;
            _w.y *= -1;
        }
    }
    
    float _r;
    ofVec3f _u;
    ofVec3f _w;
    float _t;
    float _speed;
    
    ofVec3f position;
};

class Body {
public:
    Body(CircleTrajectory path, ofColor color) : _path(path), _color(color) {}
    void tick() {
        _path.tick();
    }
    ofVec3f draw() {
        ofSetColor(_color);
        ofSphere(_path.position, 8);
    }
protected:
    CircleTrajectory _path;
    ofColor _color;
};

class OrbitalPhysics {
public:
    void addBody(Body body) {
        bodies.push_back(body);
    }
    void update() {
        for (auto& body : bodies) {
            body.tick();
        }
    }
    void draw() {
        for (auto& body : bodies) {
            body.draw();
        }
    }
    std::vector<Body> bodies;
};

class BleepoutPlugin : public radomePlugin {
public:
    
    vector<ofLight> lights;
    OrbitalPhysics physics;
    
    virtual void initialize() {
        
        ofLight light1, light2, light3;
        light1.setPointLight();
        light1.setPosition(0, 0, 0);
        light1.setDiffuseColor(ofColor(255.0, 255.0, 255.0));
        lights.push_back(light1);
        
        light2.setDirectional();
        light2.setOrientation(ofVec3f(45, 45, 45));
        light2.setDiffuseColor(ofColor(0.0, 180.0, 255.0));
        light2.setSpecularColor(ofColor(0.0, 180.0, 255.0));
        lights.push_back(light2);
        
        light3.setDirectional();
        light3.setOrientation(ofVec3f(-45, 20, -20));
        light3.setDiffuseColor(ofColor(250.0, 0.0, 0.0));
        light3.setSpecularColor(ofColor(250.0, 0.0, 0.0));
        lights.push_back(light3);
        
        int numBodies = 6;
        for (int ii = 0; ii < numBodies; ii++) {
            float s = ii / (numBodies*1.0);
            ofVec3f randomVec(ofRandom(1), ofRandom(1), ofRandom(1));
            physics.addBody(Body(
                CircleTrajectory(200, sphericalToCartesian(s * 2 * PI, 0), randomVec),
                ofColor(s * 255, 0, (1 - s) * 255)
            ));
        }
    }
    
    virtual void destroy() {
        lights.clear();
    }
    
    virtual void update(DomeInfo& dome) {
        physics.update();
    };
    
    virtual void renderScene(DomeInfo& dome) {
        
        ofSetSmoothLighting(true);
        ofEnableSeparateSpecularLight();
        ofEnableLighting();
        ofSetGlobalAmbientColor(ofColor(160, 140, 180));
        
        for (auto& light : lights) {
            light.enable();
        }
        
        physics.draw();
        
    };
    
};

BleepoutPlugin bleepoutPlugin;

