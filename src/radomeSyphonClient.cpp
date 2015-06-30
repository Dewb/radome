//
//  radomeSyphonClient.cpp
//  radome
//
//  Created by Michael Dewberry on 3/7/13.
//
//

#include "radomeSyphonClient.h"

#if defined(USE_SPOUT)
#define CONSOLE_DEBUG
#include "Spout2/Spout.h"

void ClientBase::setup() {
	unsigned int w = width, h = height;

	if(receiver && receiver->CreateReceiver(const_cast<char*>(senderName.c_str()), w, h, true)) {
		if(width != w || height != h ) {
			// Sender dimensions have changed
			width = w;
			height = h;
			mTex.allocate(width, height, GL_RGBA);
		}
		bSetup = true;
	}
}

#endif

radomeSyphonClient::radomeSyphonClient()
: bEnabled(true)
, bBound(false)
{
}

void radomeSyphonClient::initialize(string app, string server) {
#if defined(USE_SYPHON)
   if (!bSetup)
        setup();
    setApplicationName(app);
    setServerName(server);

#elif defined(USE_SPOUT)
	if (receiver) {
		receiver->ReleaseReceiver();
		delete receiver;
		receiver = NULL;
	}
	receiver = new SpoutReceiver();
	setServerName(server);
	bSetup = false;
	width = 320;
	height = 240;
#endif

}

// Binds only if the client is actually connected to a server and receiving frames.
bool radomeSyphonClient::maybeBind() {
    if (!isEnabled())
        return false;
    
    bind();
    
    if (getTexture().getWidth() > 0 && getTexture().getHeight() > 0) {
        bBound = true;
    } else {
        unbind();
        bBound = false;
    }
    
    return bBound;
}

void radomeSyphonClient::bind() {
#if defined(USE_SYPHON)
    ofxSyphonClient::bind();

#elif defined(USE_SPOUT)
	if (!bSetup) {
		setup();
	}

	unsigned int w = width, h = height;

	if(receiver && receiver->ReceiveTexture(const_cast<char*>(senderName.c_str()), w, h,
											mTex.getTextureData().textureID, mTex.getTextureData().textureTarget)) {
		if(width != w || height != h ) {
			// Sender dimensions have changed
			width = w;
			height = h;
			mTex.allocate(width, height, GL_RGBA);
		}
		bSetup = true;
	} else if (receiver) {
		receiver->ReleaseReceiver();
		bSetup = false;
		width = 0;
		height = 0;
	}

	mTex.bind();

#endif

	bBound = true;
}

void radomeSyphonClient::unbind() {
    if (!bBound) {
        return;
    }

#if defined(USE_SYPHON)
    ofxSyphonClient::unbind();

#elif defined(USE_SPOUT)
	mTex.unbind();
#endif

	bBound = false;
}

