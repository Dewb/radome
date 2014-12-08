//
//  icosohedron.h
//  radome
//
//  Adapted from http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=9814&lngWId=3
//
//

#include <vector>
#include "ofMain.h"

namespace icosohedron
{
    struct Triangle
    {
        ofVec3f v0, v1, v2;
        ofVec3f normal;
        
        Triangle(ofVec3f v0, ofVec3f v1, ofVec3f v2);
        Triangle(void) {}
    };
    
    std::vector<Triangle> create(int levels);
        
}

