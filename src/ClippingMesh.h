/******************************************************************************
 *
 * A helper class that clipps edges from our helfedge mesh, giving us new 
 * topological information that we use to create new halfedge meshes from
 * the original one.
 * 
 *****************************************************************************/

#ifndef CLIPPINGMESH_H
#define CLIPPINGMESH_H

// Libs and headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <limits>

// Classes
#include "HalfEdgeMesh.h"
#include "utils/Utils.h"
#include "utils/SimpleMesh.h"

class ClippingMesh {

public:

    ClippingMesh(HalfEdgeMesh *h) : mHalfEdgeMesh(h) {}

    ~ClippingMesh();

    void print() { std::cout << "\n---- Create ClippingMesh! ----\n"; }

    void initialize();

    bool clipMesh(SimpleMesh *);

private:

    //clipFace(XYZ *p,XYZ n,XYZ p0);

    struct ClippingVertex {
        ClippingVertex(const Vector3<float> & p = Vector3<float>(0, 0, 0),
                       float d = 0,
                       int o = 0,
                       bool v = false)
                       : pos(p),
                       dist(d),
                       occurs(o),
                       visible(v) {}
        Vector3<float> pos;
        float dist;
        int occurs;
        bool visible;
    };

    struct ClippingEdge {
        ClippingEdge(bool v = false) : visible(v) {}
        unsigned int vert1;
        unsigned int vert2;
        unsigned int face;
        bool visible;
    };

    struct ClippingFace {
        ClippingFace(bool v = false) : visible(v) {}
        unsigned int edge;
        bool visible;
    };

    struct Polygon{
        Polygon() {}
        Vector3<float> v0;
        Vector3<float> v1;
        Vector3<float> v2;
        Vector3<float> v3;
        unsigned int numVerts;
        Vector3<float> normal;
    };

    HalfEdgeMesh * mHalfEdgeMesh = nullptr;

    std::vector<ClippingVertex> mVerts;

    std::vector<ClippingEdge> mEdges;

    std::vector<ClippingFace> mFaces;

    std::vector<Polygon> mClippedPolygons;

    int clipFace(Polygon &, Vector3<float>, Vector3<float>);

    bool sortPolygonCounterClockWise(Polygon &);

    bool triangulateQuad(Polygon &, Polygon &);

    bool checkTriangleOrientation(Polygon &, Vector3<float>);
};

#endif // CLIPPINGMESH_H