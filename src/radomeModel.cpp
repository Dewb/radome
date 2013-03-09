//
//  radomeModel.cpp
//  radome
//
//  Created by Michael Dewberry on 3/9/13.
//
//

#include "radomeModel.h"

void radomeModel::update(float t) {
    setNormalizedTime(t);
    if (_rotationIncrement) {
        _rotation[0] += _rotationIncrement * t;
    }
}

void radomeModel::draw() {
    ofPushMatrix();
    ofTranslate(_origin);
    ofTranslate(-_rotationOrigin);
    ofRotate(_rotation[0], _rotation[1], _rotation[2], _rotation[3]);
    ofTranslate(_rotationOrigin);
    drawFaces();
    ofPopMatrix();
}