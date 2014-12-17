//
//  radomeSyphonClient.cpp
//  radome
//
//  Created by Michael Dewberry on 3/7/13.
//
//

#include "radomeSyphonClient.h"


radomeSyphonClient::radomeSyphonClient()
: bEnabled(true)
, bBound(false)
, fFader(1.0)
{
}

void radomeSyphonClient::initialize(string app, string server) {
#ifdef USE_SYPHON
   if (!bSetup)
        setup();
    setApplicationName(app);
    setServerName(server);
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
#ifdef USE_SYPHON
    ofxSyphonClient::bind();
    bBound = true;
#endif
}

void radomeSyphonClient::unbind() {
    if (!bBound) {
        return;
    }
#ifdef USE_SYPHON
    ofxSyphonClient::unbind();
    bBound = false;
#endif
}

