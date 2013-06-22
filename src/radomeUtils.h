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
class ofxUIWidget;

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
vector<ofxUIWidget*> addTextInput(ofxUICanvas* pUI, string name, string text, int width, string inlineLabel = "");

float frand_bounded();
std::string trim(const std::string& str,
                 const std::string& whitespace = " \t");


#endif /* defined(__radome__radomeUtils__) */
