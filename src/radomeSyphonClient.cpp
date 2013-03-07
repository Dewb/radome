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
, fFader(1.0)
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
        return true;
    } else {
        unbind();
        return false;
    }
}
