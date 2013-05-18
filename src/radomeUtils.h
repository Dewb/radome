//
//  radomeUtils.h
//  radome
//
//  Created by Michael Dewberry on 3/8/13.
//
//

#ifndef __radome__radomeUtils__
#define __radome__radomeUtils__

#include <list>
#include <vector>
#include <string>

using std::list;
using std::vector;
using std::string;

class ofxUICanvas;
class ofxUIEventArgs;

template <typename T>
void deletePointerCollection(std::vector<T*> v) {
    for (auto iter = v.begin(); iter != v.end(); iter++)
        delete *iter;
    v.clear();
}

template <typename T>
void deletePointerCollection(std::list<T*> v) {
    for (auto iter = v.begin(); iter != v.end(); iter++)
        delete *iter;
    v.clear();
}

void addRadioAndSetFirstItem(ofxUICanvas* pUI, std::string id, vector<string> options, int orientation, int w, int h);
bool matchRadioButton(string widgetName, vector<string> names, int* pValue);
float bindSlider(ofxUIEventArgs& e, string widgetName, void *pFn(float));

float frand_bounded();

#endif /* defined(__radome__radomeUtils__) */
