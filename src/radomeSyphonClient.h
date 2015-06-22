//
//  radomeSyphonClient.h
//  radome
//
//  Created by Michael Dewberry on 3/7/13.
//
//

#ifndef __radome__radomeSyphonClient__
#define __radome__radomeSyphonClient__

#include "ofxSyphon.h"

class radomeSyphonClient : public ofxSyphonClient {
public:
    radomeSyphonClient();
    void initialize(string app, string server);
    
    bool isEnabled() const { return bEnabled; }
    void setEnabled(bool b) { bEnabled = b; }

    bool maybeBind();
    bool isBound() { return bBound; }
    
    void bind();
    void unbind();
    
    ofTexture& getTexture() { return mTex; }
    int getTextureId() { return mTex.getTextureData().textureID; }
    
protected:
    bool bEnabled;
    bool bBound;
};

#endif /* defined(__radome__radomeSyphonClient__) */
