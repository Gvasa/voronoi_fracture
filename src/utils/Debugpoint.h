/******************************************************************************
 *
 * A class for handling a Half-Edge mesh
 * A Half-Edge mesh inherits form Geometry
 *
 *****************************************************************************/

#ifndef DEBUGPOINT_H
#define DEBUGPOINT_H

// Libs and headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// Classes
#include "../Geometry.h"
#include "../tools/shader.hpp"
#include "Utils.h"

class LoadObj;

class Debugpoint : public Geometry {

public:

    Debugpoint(Vector3<float>, Vector4<float> c = Vector4<float>(1.0f, 0.0f, 1.0f, 1.0f));

    ~Debugpoint();

    void initialize(Vector3<float>);

    void render(std::vector<Matrix4x4<float> >);

    //Adds a triangle (face) to the mesh    
    bool addFace(std::vector<Vector3 <float> >);

    void createMesh(std::string);

    void rotate(Vector3<float>, float);
    
    void translate(Vector3<float>);
    
    void scale(Vector3<float>);

    void setColor(Vector4<float> c) { mColor = c; }

    unsigned int getType() { return DEBUGPOINT; }

    void updatePosition(Vector3<float>);

    Vector3<float> getPosition() { return mPosition; }


    /*
     * CLASS EDGEITERATOR, HELPS OUT WITH HANDLING EDGES!
     */ 
    class EdgeIterator {
        friend class Debugpoint; 

    protected:
        Debugpoint const *mHem;
        mutable unsigned int mIndex;

        //constructor
        EdgeIterator(Debugpoint const *hem, unsigned int index) {
            mHem = hem;
            
            //protext against outside access
            assert(index < mHem->getNumEdges());
            mIndex = index;
        }
    public:

        EdgeIterator& next() { mIndex = mHem->getEdge(mIndex).next; return *this; }
        EdgeIterator& prev() { mIndex = mHem->getEdge(mIndex).prev; return *this; }
        EdgeIterator& pair() { mIndex = mHem->getEdge(mIndex).pair; return *this; }

        const EdgeIterator& next() const { mIndex = mHem->getEdge(mIndex).next; return *this; }
        const EdgeIterator& prev() const { mIndex = mHem->getEdge(mIndex).prev; return *this; }
        const EdgeIterator& pair() const { mIndex = mHem->getEdge(mIndex).pair; return *this; }

        unsigned int getEdgeIndex() const { return mIndex; }
        unsigned int getEdgeVertexIndex() const { return mHem->getEdge(mIndex).vert; }
        unsigned int getEdgeFaceIndex() const { return mHem->getEdge(mIndex).face; }

        const bool operator == (const EdgeIterator &eIt) { return this->mIndex == eIt.mIndex; }
        const bool operator != (const EdgeIterator &eIt) { return this->mIndex != eIt.mIndex; }

    };

    EdgeIterator getEdgeIterator(unsigned int i) { return EdgeIterator(this, i); }
    const EdgeIterator getEdgeIterator(unsigned int i) const { return EdgeIterator(this, i); }

private:

    // Shader data
    GLuint vertexArrayID;
    
    GLuint vertexBuffer;
    
    GLuint shaderProgram;

    // Shader indices for Matrices
    GLint MVPLoc;           // MVP matrix
    
    GLint ColorLoc;
    
    Vector4<float> mColor;
    
    Vector3<float> mPosition;

    LoadObj *objLoader;

    // Denotes a reference to a border, only for face pointers
    const static unsigned int BORDER;
    // Denotes a reference to a non-existing object
    const static unsigned int UNINITIALIZED;


    /*
     * STRUCTS
     */

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
    struct Vertex : public Geometry::Vertex {
        Vertex() : edge(UNINITIALIZED) { }
        unsigned int edge;     // index into mEdges
    };

      // A face has a normal and an index to an edge
    struct Face : public Geometry::Face {
        Face() : edge(UNINITIALIZED) { }
        unsigned int edge; //!< index into mEdges
    };

    //A struct to keep track of inner and outer pairs
    struct OrderedPair {
        unsigned int p1, p2;
        OrderedPair(unsigned int index1, unsigned int index2) {
            p1 = std::min(index1, index2);
            p2 = std::max(index1, index2);
        }
        bool operator < (const OrderedPair & rhs) const {
            if(this->p1 < rhs.p1) {
                return true;
            } else if(this->p1 == rhs.p1) {
                if(this->p2 < rhs.p2) {
                    return true;
                }
            }
            return false;
        }
    };


    /*
     * MEMBER VARIABLES
     */    
    // The edges of the mesh
    std::vector<HalfEdge> mEdges;
    // The vertices in the mesh
    std::vector<Vertex> mVerts;
    // The faces in the mesh
    std::vector<Face> mFaces;
    // Vertex list in drawing order
    std::vector< Vector3<float> > mOrderedVertexList;

    /*
     * MEMBER FUNCTIONS
     */

    //Adds a vertex to the mesh
    bool addVertex(const Vector3<float> &v, unsigned int &index);
    
    //Add a half edge pair, from vertex 1 to vertex2, to the mesh.
    bool addHalfEdgePair(unsigned int vert1, unsigned int vert2, unsigned int &index1, unsigned int &index2);

    //! Compute and return the normal at a face at faceIndex
    Vector3<float> calculateFaceNormal(unsigned int faceIndex) const;

    std::vector<Vector3<float> > buildVertexData();

    void buildRenderData();

    void updateRenderData();

    std::vector<unsigned int> findNeighborVertices(unsigned int) const;

    std::vector<unsigned int> findNeighborFaces(unsigned int) const;

    /*
     * UTILITY
     */

    // Unique vertices, this might not be needed, idk
    std::map<Vector3<float>, unsigned int> mUniqueVerts;

    //! A utility data structure to speed up removal of redundant edges
    std::map<OrderedPair, unsigned int> mUniqueEdgePairs;

     //! Return the edge at index i
    HalfEdge& getEdge(unsigned int i) { return mEdges.at(i); }
    const HalfEdge& getEdge(unsigned int i) const { return mEdges.at(i); }
    //! Return the face at index i
    Face& getFace(unsigned int i) { return mFaces.at(i); }
    const Face& getFace(unsigned int i) const { return mFaces.at(i); }
    //! Return the Vertex at index i
    Vertex& getVert(unsigned int i) { return mVerts.at(i); }
    const Vertex getVert(unsigned int i) const { return mVerts.at(i); }
    //Return number of vertices
    unsigned int getNumVerts() const {  return mVerts.size(); }
    //Return number of faces
    unsigned int getNumFaces() const {  return mFaces.size(); }
    //Return number of Edges
    unsigned int getNumEdges() const {  return mEdges.size(); }

};

#endif // DEBUGPOINT_H