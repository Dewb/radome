//
//  SwayTestPlugin.cpp
//  radome
//
//  Created by Dewb on 9/25/14.
//
//

#include "radomePlugin.h"
#include "radomeModel.h"

class User {
public:
    User(ofColor c, ofVec3f r, string modelFile, float s) {
        color = c;
        ray = r;
        scale = s;
        string modelPath = "../../content/objects/";
        modelPath.append(modelFile);
        model.loadModel(modelPath);
        lastSeenTime = 0;
    }
    
    ofColor color;
    ofVec3f ray;
    
    float roll;
    float scale;
    
    long long lastSeenTime;
    
    radomeModel model;
};


ofVec3f ngon(int n, int index, float radius) {
    float x = radius * cos(2 * PI * index / n);
    float z = radius * sin(2 * PI * index / n);
    return ofVec3f(x, 0, z);
}


class SwayTestPlugin : public radomePlugin {
public:
    vector<User> users;
    float distance;
    
    ofxOscSender rebroadcast;
    vector<ofLight> lights;
    ofMaterial material;
    
    virtual void initialize() {

        users.push_back(User(ofColor(255,   0,   0), ngon(8, 0, 2), "Space_Invader.obj", 0.1)); // red
        users.push_back(User(ofColor(  0, 255, 255), ngon(8, 1, 2), "martini.stl", 0.1)); // cyan
        users.push_back(User(ofColor(  0, 255,   0), ngon(8, 2, 2), "original.stl", 0.1)); // green
        users.push_back(User(ofColor(  0,   0, 255), ngon(8, 3, 2), "BL_WHALE.OBJ", 0.32)); // blue
        users.push_back(User(ofColor(255,   0, 255), ngon(8, 4, 2), "fighter.obj", 0.1)); // purple
        users.push_back(User(ofColor(255, 128,   0), ngon(8, 5, 2), "hheli.obj", 0.16)); // orange
        users.push_back(User(ofColor(255, 255,   0), ngon(8, 6, 2), "BANANA.stl", 0.1)); // yellow
        users.push_back(User(ofColor(250, 250, 250), ngon(8, 7, 2), "toilet.obj", 0.1)); // olive
        
        users.push_back(User(ofColor(255,   0,   0), ngon(8, 0, 3), "martini.stl", 0.1)); // red
        users.push_back(User(ofColor(128, 128, 128), ngon(8, 1, 3), "martini.stl", 0.1)); // gray
        users.push_back(User(ofColor(  0, 255,   0), ngon(8, 2, 3), "martini.stl", 0.1)); // green
        users.push_back(User(ofColor(  0,   0, 255), ngon(8, 3, 3), "martini.stl", 0.1)); // blue
        users.push_back(User(ofColor(255,   0, 255), ngon(8, 4, 3), "martini.stl", 0.1)); // purple
        users.push_back(User(ofColor(255, 128,   0), ngon(8, 5, 3), "martini.stl", 0.1)); // orange
        users.push_back(User(ofColor(255, 255,   0), ngon(8, 6, 3), "martini.stl", 0.1)); // yellow
        users.push_back(User(ofColor(250, 250, 250), ngon(8, 7, 3), "martini.stl", 0.1)); // olive
        
        users.push_back(User(ofColor(255,   0,   0), ngon(8, 0, 4), "fighter.obj", 0.1)); // red
        users.push_back(User(ofColor(  0, 255, 255), ngon(8, 1, 4), "hheli.obj", 0.1)); // cyan
        users.push_back(User(ofColor(  0, 255,   0), ngon(8, 2, 4), "fighter.obj", 0.1)); // green
        users.push_back(User(ofColor(  0,   0, 255), ngon(8, 3, 4), "hheli.obj", 0.1)); // blue
        users.push_back(User(ofColor(128, 128, 128), ngon(8, 4, 4), "fighter.obj", 0.1)); // gray
        users.push_back(User(ofColor(255, 128,   0), ngon(8, 5, 4), "hheli.obj", 0.1)); // orange
        users.push_back(User(ofColor(255, 255,   0), ngon(8, 6, 4), "fighter.obj", 0.1)); // yellow
        users.push_back(User(ofColor(250, 250, 250), ngon(8, 7, 4), "hheli.obj", 0.1)); // olive
        
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
        
        distance = 8;
        
        rebroadcast.setup("127.0.0.1", 8338);
    }
    
    virtual void renderScene(DomeInfo& dome) {
        
        ofSetSmoothLighting(true);
        ofEnableSeparateSpecularLight();
        ofEnableLighting();
        ofSetGlobalAmbientColor(ofColor(160, 140, 180));
        
        for (auto& light : lights) {
            light.enable();
        }
        
        for (auto& user : users) {
            bool timeout = true;
            if (timeout) {
                if (user.lastSeenTime < dome.frameTime - 30000) {
                    continue;
                }
            }
            
            ofPoint origin(0, 0, 0);
            origin += user.ray * (dome.radius + distance);
            origin.y *= -1;
            
            float s = user.scale;
            
            if (timeout) {
                if (user.lastSeenTime < dome.frameTime - 20000)
                    s *= 1 - (dome.frameTime - user.lastSeenTime - 20000)/10000;
                if (s == 0)
                    continue;
            }
            
            ofSetColor(user.color);
            
            ofPushMatrix();
            ofTranslate(origin);
            ofRotateX(user.roll);
            //ofBox(ofVec3f(0, 0, 0), s);
            ofScale(s, -s, s);
            user.model.draw();
            ofPopMatrix();
        }
        
        
    };
    
    virtual void receiveOscMessage(ofxOscMessage& message) {
        if(message.getAddress().substr(0, 8) == "/orient/") {
            int user = -1 + atoi(message.getAddress().substr(8, 1).c_str());
            if (user > users.size()) {
                return;
            }
            
            users[user].lastSeenTime = ofGetSystemTime();
            
            float yaw = message.getArgAsFloat(0);
            float pitch = message.getArgAsFloat(1);
            float roll = message.getArgAsFloat(2);
            
            if (pitch < 0) pitch = 0;
            
            ofVec3f Znormal(0, 0, 1);
            ofVec3f Xnormal(1, 0, 0);
            ofVec3f Ynormal(0, 1, 0);
            
            ofQuaternion qr (0, Znormal);
            ofQuaternion qp (pitch, Xnormal);
            ofQuaternion qy (yaw, Ynormal);
            ofQuaternion qt;
            
            qt = qr * qp * qy;
            
            users[user].ray = qt * ofVec3f(0, 0, 1);
            users[user].ray.normalize();
            users[user].roll = roll;
            
            rebroadcast.sendMessage(message);
        }
    }
};

SwayTestPlugin testPlugin;

