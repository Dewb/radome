//
//  radomeSyphonClient.h
//  radome
//
//  Created by Michael Dewberry on 3/7/13.
//
//

#ifndef __radome__radomeSyphonClient__
#define __radome__radomeSyphonClient__

#ifdef USE_SYPHON

#include "ofxSyphon.h"

typedef ofxSyphonClient ClientBase;
#else

#include "ofTexture.h"

class ClientBase {
public:
   ofTexture mTex;
   float getWidth() { return 0; }
   float getHeight() { return 0; }
   void setApplicationName(string s) {}
   void setServerName(string s) {}
};

#endif

class radomeSyphonClient : public ClientBase {

public:
    radomeSyphonClient();
    void initialize(string app, string server);
    
    bool isEnabled() const { return bEnabled; }
    void setEnabled(bool b) { bEnabled = b; }
    
    float getFaderValue() const { return fFader; }
    void setFaderValue(float f) { fFader = f; }
    
    bool maybeBind();
    bool isBound() { return bBound; }
    
    void bind();
    void unbind();
    
    ofTexture& getTexture() { return mTex; }
    int getTextureId() { return mTex.getTextureData().textureID; }
    
protected:
    bool bEnabled;
    bool bBound;
    float fFader;
};


#endif /* defined(__radome__radomeSyphonClient__) */
