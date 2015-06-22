//
//  radomeRenderer.h
//  radome
//
//  Created by Michael Dewberry on 6/21/15.
//
//

#ifndef __radome__radomeRenderer__
#define __radome__radomeRenderer__

#define SHOW_CUBE_MAP

#include <ofMain.h>
#include "radomeProjector.h"
#include "radomeModel.h"
#include "turntableCam.h"
#include "radomeSyphonClient.h"


enum DisplayMode {
    DisplayScene = 0,
#ifdef SHOW_CUBE_MAP
    DisplayCubeMap,
#endif
    DisplayDome,
    DisplayProjectorOutput,
    LastDisplayMode,
};

class radomeRendererImpl;

typedef list<radomeModel*> ModelList;

struct RendererParams {
    int mixMode;
    int mappingMode;

    float inputOpacity;

    float contrast;
    float saturation;
    float brightness;

    float domeDiameter;
    float domeHeight;
    float domeCurve;
    float domeBaseHeight;

    bool showTestPattern;
};

class radomeRenderer {
public:
    radomeRenderer();
    ~radomeRenderer();

    void setup();
    void render(vector<radomeProjector*>& projectorList);
    void drawScenePreview(DisplayMode mode, ofxTurntableCam& camera, vector<radomeProjector*>& projectorList, int sidebarWidth);

    void updateDomeModel();
    void addModelFromFile(string path);

    void setInputApplicationName(string app);
    void setInputServerName(string server);
    void drawInputVideo(int x, int y, int w);

    ModelList& getModelList();

    RendererParams params;

protected:
    radomeRendererImpl* _impl;
};

#endif /* defined(__radome__radomeRenderer__) */
