//
//  Flock.cpp
//  radome
//
//  Created by Dewb on 9/30/14.
//
//

#include "Flock.h"
#include "radomeModel.h"


Boid::Boid(ofVec3f pos, ofVec3f vel, radomeModel* pMod) {
    position = pos;
    velocity = vel;
    velocity = vel.normalize();
    acceleration = ofVec3f(0,0,0);
    
	neighborPosition = ofVec3f(0,0,0);
	numNeighbors = 0;
    
	decay = 0.99;
	crowdFactor	= 1.0;
    speedRange = ofVec2f(ofRandom(1.0, 1.5), ofRandom(2.5, 4.0));
    speedRangeSquared = ofVec2f(speedRange[0] * speedRange[0], speedRange[1] * speedRange[1]);
    
    pModel = pMod;
}

void Boid::constrainToDome(float radius) {
    return;
    
    float f = 0.003;
    float l = position.length();
    if (l < radius) {
        ofVec3f delta = position.normalize() * (radius - l) * f;
        velocity -= delta;
    } else if (l > 2 * radius) {
        ofVec3f delta = position.normalize() * (2 * radius - l) * f;
        velocity -= delta;
    }
}

void Boid::update() {
    crowdFactor -= (crowdFactor - (1.0 - numNeighbors * 0.02)) * 0.1;
	crowdFactor = ofClamp(crowdFactor, 0.5, 1.0);
	
	velocity += acceleration;
	velocityNormalized = velocity.normalized();
    

    float maxSpeed = speedRange[0] + crowdFactor;
    float maxSpeedSqrd = maxSpeed * maxSpeed;
        
    float velocitySqrd = velocity.lengthSquared();
    if (velocitySqrd > maxSpeedSqrd) {
        velocity = velocityNormalized * maxSpeed;
        
    } else if(velocitySqrd < speedRangeSquared[1]) {
        velocity = velocityNormalized * speedRange[1];
    }
		
	position += velocity;
	
	velocity *= decay;
		
	acceleration = ofVec3f(0,0,0);
	neighborPosition = ofVec3f(0,0,0);
	numNeighbors = 0;
}

void Boid::draw() {
    ofPushMatrix();
    ofTranslate(position);
    //ofBox(position, 10);
    ofScale(0.03, -0.03, 0.03);
    ofSetColor(255, 0, 0);
    if (pModel) {
        pModel->draw();
    }
    ofPopMatrix();
}

void Boid::recordNeighborPosition(ofVec3f pos) {
    neighborPosition += pos;
    numNeighbors++;
}

// -----------

Flock::Flock() {
}

void Flock::init(int numBoids, radomeModel* pModel) {

    for(int i=0; i < numBoids; i++)
	{
		ofVec3f position = ofVec3f(ofRandom(-100.0, 100.0), ofRandom(-100.0, 100.0), ofRandom(-100.0, 100.0));
		ofVec3f velocity = ofVec3f(ofRandom(-200.0, 200.0), ofRandom(-200.0, 200.0), ofRandom(-200.0, 200.0));
				
		boids.push_back(Boid(position, velocity, pModel));
	}
}

void Flock::clear() {
    boids.clear();
}

void Flock::update() {
    
    float zoneRadius = 40.0;
    float separationThreshold = 0.5;
    float alignmentThreshold = 0.8;
    float repelStrength = 0.004;
    float alignStrength = 0.010;
    float attractStrength = 0.020;
    
    for(list<Boid>::iterator boid1 = boids.begin(); boid1 != boids.end(); ++boid1) {
        list<Boid>::iterator boid2 = boid1;
		for(++boid2; boid2 != boids.end(); ++boid2) {
            ofVec3f deltaVec = boid1->position - boid2->position;
			float distSqrd = deltaVec.lengthSquared();
			float zoneRadiusSqrd = zoneRadius * boid1->crowdFactor * zoneRadius * boid2->crowdFactor;
			
			if (distSqrd < zoneRadiusSqrd) {
                
				boid1->recordNeighborPosition(boid2->position);
                boid2->recordNeighborPosition(boid1->position);
                
                float d = distSqrd / zoneRadiusSqrd;
				if(d < separationThreshold) {
					float force = (separationThreshold/d - 1.0) * repelStrength;
					deltaVec.normalize();
					deltaVec *= force;
                    
					boid1->acceleration += deltaVec;
					boid2->acceleration -= deltaVec;
                    
				} else if(d < alignmentThreshold) {
					float s	= (d - separationThreshold)/(alignmentThreshold - separationThreshold);
					float force = (1.0 - (cos(s * 2 * PI) * -0.5 + 0.5)) * alignStrength;
                    
					boid1->acceleration += boid2->velocityNormalized * force;
					boid2->acceleration += boid1->velocityNormalized * force;
					
				} else {
					float s	= (d - alignmentThreshold)/(1.0 - alignmentThreshold);
					float force	= (1.0 - (cos(s * 2 * PI) * -0.5 + 0.5)) * attractStrength;
                    
					deltaVec.normalize();
					deltaVec *= force;
                    
					boid1->acceleration -= deltaVec;
					boid2->acceleration += deltaVec;
				}
            }
        }
    }
    
    for (auto& boid : boids) {
        boid.update();
    }
}


void Flock::draw() {
    for (auto& boid : boids) {
        boid.draw();
    }
}

void Flock::constrainToDome(float radius) {
    for (auto& boid : boids) {
        boid.constrainToDome(radius);
    }
}