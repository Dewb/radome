//
//  icosohedron.cpp
//  radome
//
//  Adapted from http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=9814&lngWId=3
//
//

#include "icosohedron.h"
#include <iostream>

using namespace std;

namespace icosohedron
{
    Triangle::Triangle(ofVec3f v0, ofVec3f v1, ofVec3f v2)
    {
        this->v0 = v0;
        this->v1 = v1;
        this->v2 = v2;
    }

    void subdivide(vector<Triangle>& triangles)
    {
        unsigned origSize = triangles.size();
        for (unsigned i = 0 ; i < origSize ; ++i)
        {
            Triangle& t = triangles[i];
            ofVec3f a = t.v0, b = t.v1, c = t.v2;
            ofVec3f v1(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
            ofVec3f v2(a[0]+c[0], a[1]+c[1], a[2]+c[2]);
            ofVec3f v3(b[0]+c[0], b[1]+c[1], b[2]+c[2]);
            v1.normalize();
            v2.normalize();
            v3.normalize();
            t.v0 = v1; t.v1 = v3; t.v2 = v2; // overwrite the original
            triangles.push_back(Triangle(a, v1, v2));
            triangles.push_back(Triangle(c, v2, v3));
            triangles.push_back(Triangle(b, v3, v1));
        }
    }


    // build an icosahedron
    // levels specifies how many levels of detail we will have
    // levels should be 0 or greater
    // there will be 4^(levels+1) faces in there sphere
    vector<Triangle> create(int levels)
    {
        vector<Triangle> triangles;
        
        float t = (1 + sqrt(5.0))/2.0;
        float s = sqrt(1 + t*t);
        
        // create the 12 vertices
        auto v0  = ofVec3f( t,  1,  0) / s;
        auto v1  = ofVec3f(-t,  1,  0) / s;
        auto v2  = ofVec3f( t, -1,  0) / s;
        auto v3  = ofVec3f(-t, -1,  0) / s;
        auto v4  = ofVec3f( 1,  0,  t) / s;
        auto v5  = ofVec3f( 1,  0, -t) / s;
        auto v6  = ofVec3f(-1,  0,  t) / s;
        auto v7  = ofVec3f(-1,  0, -t) / s;
        auto v8  = ofVec3f( 0,  t,  1) / s;
        auto v9  = ofVec3f( 0, -t,  1) / s;
        auto v10 = ofVec3f( 0,  t, -1) / s;
        auto v11 = ofVec3f( 0, -t, -1) / s;
        
        // create the 20 triangles
        triangles.push_back(Triangle( v0,  v8,  v4));
        triangles.push_back(Triangle( v1, v10,  v7));
        triangles.push_back(Triangle( v2,  v9, v11));
        triangles.push_back(Triangle( v7,  v3,  v1));
        triangles.push_back(Triangle( v0,  v5, v10));
        triangles.push_back(Triangle( v3,  v9,  v6));
        triangles.push_back(Triangle( v3, v11,  v9));
        triangles.push_back(Triangle( v8,  v6,  v4));
        triangles.push_back(Triangle( v2,  v4,  v9));
        triangles.push_back(Triangle( v3,  v7, v11));
        triangles.push_back(Triangle( v4,  v2,  v0));
        triangles.push_back(Triangle( v9,  v4,  v6));
        triangles.push_back(Triangle( v2, v11,  v5));
        triangles.push_back(Triangle( v0, v10,  v8));
        triangles.push_back(Triangle( v5,  v0,  v2));
        triangles.push_back(Triangle(v10,  v5,  v7));
        triangles.push_back(Triangle( v1,  v6,  v8));
        triangles.push_back(Triangle( v1,  v8, v10));
        triangles.push_back(Triangle( v6,  v1,  v3));
        triangles.push_back(Triangle(v11,  v7,  v5));
        
        for (int ctr = 0; ctr < levels; ctr++)
            subdivide(triangles);
        
        return triangles;
    }
}