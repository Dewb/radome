//
//  radomeSyphonClient.h
//  radome
//
//  Created by Michael Dewberry on 3/7/13.
//
//

#ifndef __radome__radomeSyphonClient__
#define __radome__radomeSyphonClient__

#ifdef _WIN32
#define USE_SPOUT
#else
#define USE_SYPHON
#endif

#if defined(USE_SYPHON)

#include "ofxSyphon.h"

typedef ofxSyphonClient ClientBase;
#elif defined(USE_SPOUT)

#include "ofTexture.h"
class SpoutReceiver;

class ClientBase {
public:
	ClientBase() : receiver(NULL) {};
	ofTexture mTex;
	float getWidth() { return width; }
	float getHeight() { return height; }
	void setApplicationName(string s) {}
	void setServerName(string s) { senderName = s; }
protected:
	void setup();
	SpoutReceiver *receiver;
	bool bSetup;
	int width;
	int height;
	string senderName;
};

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
