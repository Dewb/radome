//
//  radomePlugin.h
//  radome
//
//  Created by Dewb on 9/22/14.
//
//

#ifndef __radome__radomePlugin__
#define __radome__radomePlugin__

#include <list>


#include "ofMain.h"

class ofxOscMessage;

struct DomeInfo {
    int radius;
    int height;
    long long frameTime;
};

class radomePlugin {
public:
    radomePlugin();
    virtual ~radomePlugin();
    
    virtual void initialize() {};
    virtual void renderScene(DomeInfo& dome) = 0;
    virtual void update(DomeInfo& dome) {};
    virtual void receiveOscMessage(ofxOscMessage& message) {};
    virtual void destroy() {};
    
    bool isEnabled() { return _enabled; }
    void setEnabled(bool enabled = true);
protected:
    bool _enabled;
    bool _initialized;
};

typedef std::list<radomePlugin*> PluginList;

class PluginLibrary {
private:
    PluginLibrary() {};
    ~PluginLibrary();

    PluginLibrary(PluginLibrary const& copy);
    PluginLibrary& operator=(PluginLibrary const& copy);
    
public:
    static PluginLibrary& getInstance()
    {
        static PluginLibrary instance;
        return instance;
    }
    
    static void addPlugin(radomePlugin* plugin) {
        PluginLibrary::getInstance()._plugins.push_back(plugin);
    }
    
    static PluginList getList() { return PluginLibrary::getInstance()._plugins; }
    
protected:
    PluginList _plugins;
    
};

#endif /* defined(__radome__radomePlugin__) */
