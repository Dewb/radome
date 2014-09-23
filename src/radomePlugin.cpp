//
//  radomePlugin.cpp
//  radome
//
//  Created by Dewb on 9/22/14.
//
//

#include "ofMain.h"
#include "radomePlugin.h"
#include "ofxOsc.h"

PluginLibrary::~PluginLibrary() {
    for (auto plug : _plugins) {
        delete plug;
    }
}


class User {
public:
    User(ofColor c, ofVec3f r) {
        color = c;
        ray = r;
    }
    
    ofColor color;
    ofVec3f ray;
    float roll;
};

class SwayTestPlugin : public radomePlugin {
public:
    vector<User> users;
    float distance;
    float s;
    
    ofxOscSender rebroadcast;
    
    virtual void initialize() {
        users.push_back(User(ofColor(255, 0, 0), ofVec3f(0, 100, 0)));
        users.push_back(User(ofColor(0, 255, 0), ofVec3f(0, 100, 0)));
        users.push_back(User(ofColor(255, 0, 255), ofVec3f(0, 100, 0)));
        users.push_back(User(ofColor(0, 0, 255), ofVec3f(0, 100, 0)));
        distance = 8;
        s = 20;
        
        rebroadcast.setup("192.168.1.139", 8338);
    }
    
    virtual void renderScene(DomeInfo& dome) {
        for (auto user : users) {
            ofPoint origin(0, 0, 0);
            origin += user.ray * (dome.radius + distance);
            origin.y *= -1;
            ofSetColor(user.color);
            ofPushMatrix();
            ofTranslate(origin);
            ofRotateZ(user.roll);
            ofBox(ofVec3f(0, 0, 0), s);
            ofPopMatrix();
        }
    };
    
    virtual void receiveOscMessage(ofxOscMessage& message) {
        if(message.getAddress().substr(0, 8) == "/orient/") {
            int user = -1 + atoi(message.getAddress().substr(8, 1).c_str());
            if (user > users.size()) {
                return;
            }
            
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


void instantiatePlugins() {
 
    PluginLibrary::addPlugin(new SwayTestPlugin());

}