//
//  icosohedron.h
//  radome
//
//  Adapted from http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=9814&lngWId=3
//
//

#include <vector>
#include <cmath>

namespace icosohedron
{
    struct Vector
    {
        float e[4];
        
        inline float& operator[](int index) {return e[index];}
        inline const float& operator[](int index) const {return e[index];}
        
        void normalize(void)
        {
            float length = sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
            e[0]/=length;
            e[1]/=length;
            e[2]/=length;
        }
        
    };

    struct Triangle
    {
        Vector v0, v1, v2;
        Vector normal;
        
        Triangle(Vector v0, Vector v1, Vector v2);
        
        void setnormal(Vector normal);
        void setnormal(float x, float y, float z);
        
        Triangle(void) {}
        
    };
    
    std::vector<Triangle> createsphere(int levels);
        
}

