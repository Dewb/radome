//
//  radomeUtils.cpp
//  radome
//
//  Created by Michael Dewberry on 3/8/13.
//
//

#include "radomeUtils.h"
#include "ofxUI.h"

void addRadioAndSetFirstItem(ofxUICanvas* pUI, string id, vector<string> options, int orientation, int w, int h)
{
    pUI->addRadio(id, options, orientation, w, h);
    {
        // so silly, ofxUI needs to init radios
        auto pToggle = dynamic_cast<ofxUIToggle*>(pUI->getWidget(options[0]));
        if (pToggle)
            pToggle->setValue(true);
    }
}

bool matchRadioButton(string widgetName, vector<string> names, int* pValue)
{
    for (int ii = 0; ii < names.size(); ii++) {
        if (widgetName == names[ii]) {
            if (pValue) {
                *pValue = ii;
            }
            return true;
        }
    }
    return false;
}

float bindSlider(ofxUIEventArgs& e, string widgetName, void *pFn(float))
{
    float val = 0;
    auto pSlider = dynamic_cast<ofxUISlider*>(e.widget);
    if (pSlider && widgetName == pSlider->getName()) {
        val = pSlider->getValue();
    }
    if (pFn)
        (*pFn)(val);
    return val;
}

vector<ofxUIWidget*> addTextInput(ofxUICanvas* pUI, string name, string text, int width, string inlineLabel)
{
    vector<ofxUIWidget*> widgets;
    
    if (inlineLabel.size())
    {
        auto pLabel = new ofxUILabel(75, inlineLabel, OFX_UI_FONT_SMALL, 24);
        widgets.push_back(pUI->addWidgetDown(pLabel));
    }
    
    auto pInput = new ofxUITextInput(name, text, width, 0, 0, 0, OFX_UI_FONT_SMALL);
    pInput->setAutoClear(false);
    
    if (inlineLabel.size())
    {
        widgets.push_back(pUI->addWidgetRight(pInput));
    }
    else
    {
        widgets.push_back(pUI->addWidgetDown(pInput));
    }
    
    return widgets;
}

float frand_bounded() {
    return (rand()/(1.0*RAND_MAX)-0.5)*2.0;
}