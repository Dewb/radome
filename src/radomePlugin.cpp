//
//  radomePlugin.cpp
//  radome
//
//  Created by Dewb on 9/22/14.
//
//

#include "radomePlugin.h"

radomePlugin::radomePlugin() {
    _enabled = false;
    _initialized = false;
    PluginLibrary::addPlugin(this);
}

radomePlugin::~radomePlugin() {
    if (_initialized) {
        destroy();
    }
}

void radomePlugin::setEnabled(bool enabled) {
    if (enabled && !_initialized) {
        initialize();
        _initialized = true;
    } else if (!enabled && _initialized) {
        destroy();
        _initialized = false;
    }
    _enabled = enabled;
}

PluginLibrary::~PluginLibrary() {
}


