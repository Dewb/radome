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