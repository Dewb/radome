//
//  FlockingPlugin.cpp
//  radome
//
//  Created by Dewb on 9/30/14.
//
//

#include "radomePlugin.h"
#include "Flock.h"


class FlockingPlugin : public radomePlugin {
public:

    vector<ofLight> lights;
    Flock flock;
    
    virtual void initialize() {
        
        Boid::model.loadModel("../../content/objects/Space_Invader.obj");
        flock.init(1000);
        
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
    }
    
    virtual void destroy() {
        lights.clear();
        flock.clear();
    }
    
    virtual void update(DomeInfo& dome) {
        flock.constrainToDome(dome.radius);
        flock.update();
    };
    
    virtual void renderScene(DomeInfo& dome) {
        
        ofSetSmoothLighting(true);
        ofEnableSeparateSpecularLight();
        ofEnableLighting();
        ofSetGlobalAmbientColor(ofColor(160, 140, 180));
        
        for (auto& light : lights) {
            light.enable();
        }
        
        flock.draw();

    };
    
};

FlockingPlugin flockingPlugin;

