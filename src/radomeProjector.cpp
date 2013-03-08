//
//  radomeProjector.cpp
//  radome
//
//  Created by Michael Dewberry on 3/8/13.
//
//

#include "radomeProjector.h"

radomeProjector::radomeProjector(float heading, float distance, float height)
: _heading(heading)
, _distance(distance)
, _height(height)
{
    _image.setUseTexture(true);
}

void radomeProjector::updateCamera() {
    _camera.setPosition(_distance * cos(_heading), _height, _distance * sin(_heading));
}

void radomeProjector::renderToProjector()
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           _image.getTextureReference().getTextureData().textureID, 0 );
}

void radomeProjector::drawSceneRepresentation() {
    ofPushMatrix();
    ofPushStyle();

    ofRotateY(_heading);
    ofTranslate(_distance, 0);
    
    for (int pass = 0; pass < 2; pass++)
    {
        if (pass == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            ofSetColor(40, 191, 80);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            ofSetColor(10, 80, 40);
        }
        
        ofSetLineWidth(2.0);
        ofBox(0.0, _height, 0.0, 40);
        ofSetLineWidth(5.0);
        ofLine(0.0, _height-20.0, 0.0, 0.0, 0.0, 0.0);
    }
    
    ofPopStyle();
    ofPopMatrix();
}