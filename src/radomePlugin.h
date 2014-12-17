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

class LineWidthAdjuster {
public:
    static void setLineWidth(float w) { ofSetLineWidth(factor * w); }
    static float factor;
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
    
    virtual void keyPressed(int key) {};
    virtual void mousePressed(int x, int y, int button) {};
    virtual void mouseMoved(int x, int y ) {};
    virtual void mouseReleased(int x, int y, int button) {};
    virtual void mouseDragged(int x, int y, int button) {};
    
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
