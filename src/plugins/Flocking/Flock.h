//
//  Flock.h
//  radome
//
//  Created by Dewb on 9/30/14.
//
//

#ifndef __radome__Flock__
#define __radome__Flock__

#include "ofMain.h"
#include "radomeModel.h"
#include <list>

class Boid {
public:
	Boid(ofVec3f position, ofVec3f velocity);

	void constrainToDome(float radius);
	void update();
	void draw();

	void recordNeighborPosition(ofVec3f pos);
    
	ofVec3f position;
	ofVec3f velocity;
    ofVec3f velocityNormalized;
	ofVec3f acceleration;
    ofVec3f mass;
	
	ofVec3f	neighborPosition;
	int numNeighbors;
    
    ofColor color;
    
	float decay;
	ofVec2f speedRange;
	ofVec2f speedRangeSquared;
	float crowdFactor;
    
    static radomeModel model;
};

class Flock {
public:
    Flock();
    
    void init(int numBoids);
    void update();
    void constrainToDome(float radius);
    void draw();
    void clear();
    
    std::list<Boid> boids;
};


#endif /* defined(__radome__Flock__) */
