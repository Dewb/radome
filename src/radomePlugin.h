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

class ofxOscMessage;

struct DomeInfo {
    int radius;
    int height;
};

class radomePlugin {
public:
    radomePlugin() {};
    virtual ~radomePlugin() { destroy(); }
    
    virtual void initialize() {};
    virtual void renderScene(DomeInfo& dome) = 0;
    virtual void receiveOscMessage(ofxOscMessage& message) {};
    virtual void destroy() {};
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

void instantiatePlugins();

#endif /* defined(__radome__radomePlugin__) */
