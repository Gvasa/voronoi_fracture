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
#include <algorithm>

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

    HalfEdgeMesh * clipMesh(Vector3<float>);

private:

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

    std::vector<Vector3<float> > mVerts;

    //std::vector<Vector3<float> > mFaces;

    std::vector<Polygon> mClippedPolygons;

    int clipFace(Polygon &, Vector3<float>, Vector3<float>, std::vector<Vector3<float> > &);

    bool sortPolygonCounterClockWise(Polygon &);

    bool sortPolygonCounterClockWise(std::vector<Vector3<float> > &V);

    bool triangulateQuad(Polygon &, Polygon &);

    bool triangulateArbPolygon(std::vector<Vector3<float> > &, std::vector<Vector3<float> > &);

    bool triangulateConvexPolygon(std::vector<Vector3<float> > &, std::vector<Vector3<float> > &);

    bool isInsidePolygon(const std::vector<Vector3<float> > &, const Vector3<float> &);

    bool sameSide(Vector3<float>, Vector3<float>, Vector3<float>, Vector3<float>);

    bool checkTriangleOrientation(Polygon &, Vector3<float>);

    bool findVertex(std::vector<Vector3<float> > &, Vector3<float>);
};

#endif // CLIPPINGMESH_H