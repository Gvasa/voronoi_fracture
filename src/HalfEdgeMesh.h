/******************************************************************************
 *
 * A class for handling a Half-Edge mesh
 * A Half-Edge mesh inherits form Geometry
 *
 *****************************************************************************/

#ifndef HALFEDGEMESH_H
#define HALFEDGEMESH_H

// Libs and headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Classes
#include "Geometry.h"
#include "tools/shader.hpp"
#include "math/Vector3.h"
#include "math/Vector4.h"

class HalfEdgeMesh : public Geometry {

public:

    HalfEdgeMesh() { };
    ~HalfEdgeMesh();

    void initialize();
    void draw();

    bool addFace();
    bool addEdge();
    bool addVertex();

private:

    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;

    // Denotes a reference to a border, only for face pointers
    const static unsigned int BORDER;
    // Denotes a reference to a non-existing object
    const static unsigned int UNINITIALIZED;

    // A HalfEdge has a vertex, a face and 3 halfedges for mesh traversal
    struct HalfEdge {
        HalfEdge() : vert(UNINITIALIZED), face(UNINITIALIZED), next(UNINITIALIZED),
                     prev(UNINITIALIZED), pair(UNINITIALIZED) { }
        unsigned int vert;  // index into mVerts (the origin vertex)
        unsigned int face;  // index into mFaces
        unsigned int next;  // index into mEdges
        unsigned int prev;  // index into mEdges
        unsigned int pair;  // index into mEdges
    };

    // A vertex is a point and an edge index
    struct Vertex {
        Vertex() : edge(UNINITIALIZED) { }
        unsigned int edge;     // index into mEdges
    };

      // A face has a normal and an index to an edge
    struct Face {
        Face() : edge(UNINITIALIZED) { }
        unsigned int edge; //!< index into mEdges
    };

    // The edges of the mesh
    std::vector<HalfEdge> mEdges;
    // The vertices in the mesh
    std::vector<Vertex> mVerts;
    // The faces in the mesh
    std::vector<Face> mFaces;

    // Unique vertices, this might not be needed, idk
    //std::map<Vector3<float>, unsigned int> mUniqueVerts;

};

#endif // HALFEDGEMESH_H