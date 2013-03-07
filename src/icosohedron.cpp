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
    struct Matrix
    {
        float e[16];
        
        inline float& operator[](int index) {return e[index];}
        inline const float& operator[](int index) const {return e[index];}
    };

    ostream& operator<<(ostream& out, Matrix &m);
    ostream& operator<<(ostream& out, Vector &v);

    Matrix buildidentitymatrix(void);
    Matrix buildrotationmatrix(float theta, float x, float y, float z);
    Matrix buildscalematrix(float x, float y, float z);
    Matrix buildtranslationmatrix(float x, float y, float z);

    Vector buildvector(float x, float y, float z);

    Matrix operator*(const Matrix &a, const Matrix &b);
    void operator*=(Matrix &a, const Matrix &b);
    Vector operator*(const Matrix &a, const Vector &v);
    Vector operator/(const Vector &v, const float &f);

    ostream& operator<<(ostream& out, const Matrix &m)
    {
        out << m[0] << " " << m[4] << " " <<  m[8] << " " <<  m[12] << endl;
        out << m[1] << " " <<  m[5] << " " <<  m[9] << " " <<  m[13] << endl;
        out << m[2] << " " <<  m[6] << " " <<  m[10] << " " <<  m[14] << endl;
        out << m[3] << " " <<  m[7] << " " <<  m[11] << " " <<  m[15];
        return out;
    }

    ostream& operator<<(ostream& out, Vector &v)
    {
        out << "(" << v[0] << ", " << v[1] << ", " <<  v[2] << ")";
        return out;
    }

    Matrix buildidentitymatrix(void)
    {
        Matrix m;
        m[0] = 1;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
        
        m[4] = 0;
        m[5] = 1;
        m[6] = 0;
        m[7] = 0;
        
        m[8] = 0;
        m[9] = 0;
        m[10] = 1;
        m[11] = 0;
        
        m[12] = 0;
        m[13] = 0;
        m[14] = 0;
        m[15] = 1;
        
        return m;
    }

    Matrix buildrotationmatrix(float theta, float x, float y, float z)
    {
        Matrix m;
        float c = cos(theta);
        float s = sin(theta);
        float t = 1 - cos(theta);
        m[0] = t*x*x + c;
        m[1] = t*x*y + s*z;
        m[2] = t*x*z - s*y;
        m[3] = 0;
        
        m[4] = t*x*y - s*z;
        m[5] = t*y*y + c;
        m[6] = t*y*z + s*x;
        m[7] = 0;
        
        m[8] = t*x*z + s*y;
        m[9] = t*y*z - s*x;
        m[10] = t*z*z + c;
        m[11] = 0;
        
        m[12] = 0;
        m[13] = 0;
        m[14] = 0;
        m[15] = 1;
        
        return m;
    }

    Matrix buildscalematrix(float x, float y, float z)
    {
        Matrix m;
        
        m[0] = x;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
        
        m[4] = 0;
        m[5] = y;
        m[6] = 0;
        m[7] = 0;
        
        m[8] = 0;
        m[9] = 0;
        m[10] = z;
        m[11] = 0;
        
        m[12] = 0;
        m[13] = 0;
        m[14] = 0;
        m[15] = 1;
        
        return m;
    }

    Matrix buildtranslationmatrix(float x, float y, float z)
    {
        Matrix m;
        m[0] = 1;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
        
        m[4] = 0;
        m[5] = 1;
        m[6] = 0;
        m[7] = 0;
        
        m[8] = 0;
        m[9] = 0;
        m[10] = 1;
        m[11] = 0;
        
        m[12] = x;
        m[13] = y;
        m[14] = z;
        m[15] = 1;
        return m;
    }

    Matrix operator*(const Matrix &a, const Matrix &b)
    {
        Matrix m;
        
        m[0] = b[0]*a[0] + b[1]*a[4] + b[2]*a[8] + b[3]*a[12];
        m[1] = b[0]*a[1] + b[1]*a[5] + b[2]*a[9] + b[3]*a[13];
        m[2] = b[0]*a[2] + b[1]*a[6] + b[2]*a[10] + b[3]*a[14];
        m[3] = b[0]*a[3] + b[1]*a[7] + b[2]*a[11] + b[3]*a[15];
        
        m[4] = b[4]*a[0] + b[5]*a[4] + b[6]*a[8] + b[7]*a[12];
        m[5] = b[4]*a[1] + b[5]*a[5] + b[6]*a[9] + b[7]*a[13];
        m[6] = b[4]*a[2] + b[5]*a[6] + b[6]*a[10] + b[7]*a[14];
        m[7] = b[4]*a[3] + b[5]*a[7] + b[6]*a[11] + b[7]*a[15];
        
        m[8] = b[8]*a[0] + b[9]*a[4] + b[10]*a[8] + b[11]*a[12];
        m[9] = b[8]*a[1] + b[9]*a[5] + b[10]*a[9] + b[11]*a[13];
        m[10] = b[8]*a[2] + b[9]*a[6] + b[10]*a[10] + b[11]*a[14];
        m[11] = b[8]*a[3] + b[9]*a[7] + b[10]*a[11] + b[11]*a[15];
        
        m[12] = b[12]*a[0] + b[13]*a[4] + b[14]*a[8] + b[15]*a[12];
        m[13] = b[12]*a[1] + b[13]*a[5] + b[14]*a[9] + b[15]*a[13];
        m[14] = b[12]*a[2] + b[13]*a[6] + b[14]*a[10] + b[15]*a[14];
        m[15] = b[12]*a[3] + b[13]*a[7] + b[14]*a[11] + b[15]*a[15];
        
        return m;
    }


    void operator*=(Matrix &a, const Matrix &b)
    {
        a = a*b;
    }

    Vector buildvector(float x, float y, float z)
    {
        Vector v;
        v[0] = x;
        v[1] = y;
        v[2] = z;
        v[3] = 1;
        return v;
    }

    Vector operator*(const Matrix &a, const Vector &v)
    {
        Vector vnew;
        
        vnew[0] = v[0]*a[0] + v[1]*a[4] + v[2]*a[8] + v[3]*a[12];
        vnew[1] = v[0]*a[1] + v[1]*a[5] + v[2]*a[9] + v[3]*a[13];
        vnew[2] = v[0]*a[2] + v[1]*a[6] + v[2]*a[10] + v[3]*a[14];
        vnew[3] = v[0]*a[3] + v[1]*a[7] + v[2]*a[11] + v[3]*a[15];
        
        return vnew;
    }

    Vector operator/(const Vector &v, const float &f)
    {
        return buildvector(v[0]/f, v[1]/f, v[2]/f);
    }

    Triangle::Triangle(Vector v0, Vector v1, Vector v2)
    {
        this->v0 = v0;
        this->v1 = v1;
        this->v2 = v2;
    }

    void Triangle::setnormal(Vector normal)
    {
        this->normal = normal;
    }

    void Triangle::setnormal(float x, float y, float z)
    {
        setnormal(buildvector(x, y, z));
    }

    void subdivide(vector<Triangle>& triangles)
    {
        unsigned origSize = triangles.size();
        for (unsigned i = 0 ; i < origSize ; ++i)
        {
            Triangle& t = triangles[i];
            Vector a = t.v0, b = t.v1, c = t.v2;
            Vector v1 = buildvector(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
            Vector v2 = buildvector(a[0]+c[0], a[1]+c[1], a[2]+c[2]);
            Vector v3 = buildvector(b[0]+c[0], b[1]+c[1], b[2]+c[2]);
            v1.normalize();
            v2.normalize();
            v3.normalize();
            t.v0 = v1; t.v1 = v3; t.v2 = v2; // overwrite the original
            triangles.push_back(Triangle(a, v1, v2));
            triangles.push_back(Triangle(c, v2, v3));
            triangles.push_back(Triangle(b, v3, v1));
        }
    }


    // levels specifies how many levels of detail we will have
    // levels should be 0 or greater
    // there will be 4^(levels+1) faces in there sphere
    vector<Triangle> createsphere(int levels)
    {
        
        vector<Triangle> triangles;
        
        // build an icosahedron
        
        float t = (1 + sqrt(5.0))/2.0;
        float s = sqrt(1 + t*t);
        // create the 12 vertices
        Vector v0 = buildvector(t, 1, 0)/s;
        Vector v1 = buildvector(-t, 1, 0)/s;
        Vector v2 = buildvector(t, -1, 0)/s;
        Vector v3 = buildvector(-t, -1, 0)/s;
        Vector v4 = buildvector(1, 0, t)/s;
        Vector v5 = buildvector(1, 0, -t)/s;
        Vector v6 = buildvector(-1, 0, t)/s;
        Vector v7 = buildvector(-1, 0, -t)/s;
        Vector v8 = buildvector(0, t, 1)/s;
        Vector v9 = buildvector(0, -t, 1)/s;
        Vector v10 = buildvector(0, t, -1)/s;
        Vector v11 = buildvector(0, -t, -1)/s;
        
        // create the 20 triangles
        triangles.push_back(Triangle(v0, v8, v4));
        triangles.push_back(Triangle(v1, v10, v7)); // x
        triangles.push_back(Triangle(v2, v9, v11));
        triangles.push_back(Triangle(v7, v3, v1)); // x
        triangles.push_back(Triangle(v0, v5, v10));
        triangles.push_back(Triangle(v3, v9, v6));
        triangles.push_back(Triangle(v3, v11, v9));
        triangles.push_back(Triangle(v8, v6, v4));
        triangles.push_back(Triangle(v2, v4, v9));
        triangles.push_back(Triangle(v3, v7, v11)); // x
        triangles.push_back(Triangle(v4, v2, v0));
        triangles.push_back(Triangle(v9, v4, v6));
        triangles.push_back(Triangle(v2, v11, v5));
        triangles.push_back(Triangle(v0, v10, v8));
        triangles.push_back(Triangle(v5, v0, v2));
        triangles.push_back(Triangle(v10, v5, v7)); // x
        triangles.push_back(Triangle(v1, v6, v8));
        triangles.push_back(Triangle(v1, v8, v10));
        triangles.push_back(Triangle(v6, v1, v3));
        triangles.push_back(Triangle(v11, v7, v5)); // x
        
        for (int ctr = 0; ctr < levels; ctr++) subdivide(triangles);
        
        return triangles;
    }
}