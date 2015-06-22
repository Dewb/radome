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
{
}

void radomeSyphonClient::initialize(string app, string server) {
    if (!bSetup)
        setup();
    setApplicationName(app);
    setServerName(server);
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
    ofxSyphonClient::bind();
    bBound = true;
}

void radomeSyphonClient::unbind() {
    if (!bBound) {
        return;
    }
    ofxSyphonClient::unbind();
    bBound = false;
}
