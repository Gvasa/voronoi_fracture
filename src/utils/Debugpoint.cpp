#include "Debugpoint.h"

// Set constants for flaging of edges that are not valid half-edges
const unsigned int Debugpoint::BORDER = (std::numeric_limits<unsigned int>::max)();
const unsigned int Debugpoint::UNINITIALIZED = (std::numeric_limits<unsigned int>::max)()-1;


Debugpoint::Debugpoint(Vector3<float> p, Vector4<float> c)
: mColor(c), mPosition(p) {

    createMesh("lowPolySphere1.0");
    scale(Vector3<float>(0.02f, 0.02f, 0.02f));

    translate(p);
}

Debugpoint::~Debugpoint() {

    std::cout << "\ndelete debugpoint!" << std::endl;

    // Cleanup VBO
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    mEdges.clear();
    mEdges.shrink_to_fit();

    mVerts.clear();
    mVerts.shrink_to_fit();

    mFaces.clear();
    mFaces.shrink_to_fit();

    mOrderedVertexList.clear();
    mOrderedVertexList.shrink_to_fit();
}


void Debugpoint::initialize(Vector3<float> lightPosition) {

    buildRenderData();

    // Update the lists that we draw
    updateRenderData();

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );

    MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
    ColorLoc = glGetUniformLocation(shaderProgram, "color");

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mOrderedVertexList.size() * sizeof(Vector3<float>), &mOrderedVertexList[0], GL_STATIC_DRAW);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                          // attribute 0. I.e. layout 0 in shader
        3,                          // size
        GL_FLOAT,                   // type
        GL_FALSE,                   // normalized?
        0,                          // stride
        reinterpret_cast<void*>(0)  // array buffer offset
    );
}

void Debugpoint::render(std::vector<Matrix4x4<float> > sceneMatrices) {

    // Use shader
    glUseProgram(shaderProgram);
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &sceneMatrices[I_MVP](0, 0));
    glUniform4f(ColorLoc, mColor[0], mColor[1], mColor[2], mColor[3]);

    // Rebind the buffer data, vertices are now updated
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mOrderedVertexList.size() * sizeof(Vector3<float>), &mOrderedVertexList[0], GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, mOrderedVertexList.size());
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
}

bool Debugpoint::addFace(const std::vector<Vector3 <float> > verts) {

    //add the vertices of the face triangle
    unsigned int vertIndex1, vertIndex2, vertIndex3;

    addVertex(verts.at(0), vertIndex1);
    addVertex(verts.at(1), vertIndex2);
    addVertex(verts.at(2), vertIndex3);

   // add all half-edge pairs
   unsigned int innerHalfEdgeIndex1 = 0,
                innerHalfEdgeIndex2 = 0,
                innerHalfEdgeIndex3 = 0,
                outerHalfEdgeIndex1 = 0,
                outerHalfEdgeIndex2 = 0,
                outerHalfEdgeIndex3 = 0;

    //add half edge pairs
    addHalfEdgePair(vertIndex1, vertIndex2, innerHalfEdgeIndex1, outerHalfEdgeIndex1);
    addHalfEdgePair(vertIndex2, vertIndex3, innerHalfEdgeIndex2, outerHalfEdgeIndex2);
    addHalfEdgePair(vertIndex3, vertIndex1, innerHalfEdgeIndex3, outerHalfEdgeIndex3);

    //connect the inner ring
    getEdge(innerHalfEdgeIndex1).next = innerHalfEdgeIndex2;
    getEdge(outerHalfEdgeIndex1).prev = innerHalfEdgeIndex3;

    getEdge(innerHalfEdgeIndex2).next = innerHalfEdgeIndex3;
    getEdge(outerHalfEdgeIndex2).prev = innerHalfEdgeIndex1;

    getEdge(innerHalfEdgeIndex3).next = innerHalfEdgeIndex1;
    getEdge(outerHalfEdgeIndex3).prev = innerHalfEdgeIndex2;

    // Create the face
    Face face;
    mFaces.push_back(face);
    mFaces.back().edge = innerHalfEdgeIndex1;
    mFaces.back().normal = calculateFaceNormal(mFaces.size() - 1);

    // all half-edges share the same left face (previously added)
    getEdge(innerHalfEdgeIndex1).face = mFaces.size() - 1;
    getEdge(innerHalfEdgeIndex2).face = mFaces.size() - 1;
    getEdge(innerHalfEdgeIndex3).face = mFaces.size() - 1;

    return true;
}
void Debugpoint::createMesh(std::string objName) {

    std::vector<std::vector<Vector3<float> > > vertexList = Geometry::mObjectLoader->getMeshVertexList(objName);

    for(unsigned int i = 0; i < vertexList.size(); i++) 
        addFace(vertexList[i]);

}

// Rotate the mesh
void Debugpoint::rotate(Vector3<float> axis, float angle){
    
    //Compute the rotation matrix
    std::cout << std::endl << "Rotating..." << std::endl;

    Matrix4x4<float>  rotationMatrix = Matrix4x4<float>::RotationXYZ(
        axis[0] * (angle * M_PI / 180.0f),
        axis[1] * (angle * M_PI / 180.0f),
        axis[2] * (angle * M_PI / 180.0f)  
    );

    for(unsigned int i = 0; i < mVerts.size(); i++){
        // Apply the rotation to the vertices
        Vector4<float> v = Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);
        v = rotationMatrix * v;
        mVerts[i].pos = Vector3<float>(v[0], v[1], v[2]);

        //Apply the rotation matrix
        Vector4<float> n = Vector4<float>(mVerts[i].normal[0], mVerts[i].normal[1], mVerts[i].normal[2], 1.0f);
        n = rotationMatrix * n;
        mVerts[i].normal = Vector3<float>(n[0], n[1], n[2]).Normalize();
    }

}

// Translate the mesh
void Debugpoint::translate(Vector3<float> p){
    
    // Compute the translation matrix
    Matrix4x4<float> translationMatrix = Matrix4x4<float>::Translation(p[0], p[1], p[2]);

    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the translation matrix
        Vector4<float> v = Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);
        v = translationMatrix * v;
        mVerts[i].pos = Vector3<float>(v[0], v[1], v[2]);
    }
}

// Scale the mesh 
void Debugpoint::scale(Vector3<float> s){

    // Compute the scaling matrix
    Matrix4x4<float> scalingMatrix = Matrix4x4<float>::Scale(s[0], s[1], s[2]);

    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the scaling matrix
        Vector4<float> v = Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);
        v = scalingMatrix * v;
        mVerts[i].pos = Vector3<float>(v[0], v[1], v[2]);
    }
}


void Debugpoint::updatePosition(Vector3<float> dp) {

    mPosition += dp;

    translate(dp);

    updateRenderData();
}

// This is where we add a vertex to the half-edge structure
bool Debugpoint::addVertex(const Vector3<float> &v, unsigned int &index) {

    //search through uniqueVerts and see if the vertice already is in our list
    std::map<Vector3<float>, unsigned int>::iterator it = mUniqueVerts.find(v);

    //if we find an existing vertices, set our index to that value (hence the map above)
    if(it != mUniqueVerts.end()) {
        index = (*it).second;
        return false;
    }

    //The vertices does not exist! add it
    mUniqueVerts[v] = index = getNumVerts(); //get the size of the existing list, give the last pos at index to the vertices being added
    Vertex vert;
    vert.pos = v;
    mVerts.push_back(vert); //add it to the list

    return true;
}

bool Debugpoint::addHalfEdgePair(unsigned int vert1, unsigned int vert2, unsigned int &index1, unsigned int &index2) {

    // check if the pair to be added already exists
    std::map<OrderedPair, unsigned int>::iterator it = mUniqueEdgePairs.find(OrderedPair(vert1, vert2));

    // if it exists
    if(it != mUniqueEdgePairs.end()) {

        index1 = it->second;                // assign the first index to the place where its found in mUniqueEdgePairs
        index2 = getEdge(it->second).pair;  // assign the pair of the first edge to the second index
        
        if(vert1 != getEdge(index1).vert ) {    // check that both index got assigned correctly
            std::swap(index1, index2);          // otherwise swap
        }

        return false;
    }

    // if not found, calculate the indices 
    index1 = mEdges.size();
    index2 = index1+1;

    // Create edges and set pair index
    HalfEdge edge1, edge2;
    edge1.pair = index2;
    edge2.pair = index1;

    // connect the edges to the verst
    edge1.vert = vert1;
    edge2.vert = vert2;

    // connect the verts to the edges
    getVert(vert1).edge = index1;
    getVert(vert2).edge = index2;

    // store the edges in mEdges
    mEdges.push_back(edge1);
    mEdges.push_back(edge2);

    OrderedPair op(vert1, vert2);
    mUniqueEdgePairs[op] = index1; // [ ] constructs a new entry in the map, ordering not important

    return true;
}

//! Compute and return the normal at face at faceIndex
Vector3<float> Debugpoint::calculateFaceNormal(unsigned int faceIndex) const {

    unsigned int index = getFace(faceIndex).edge;
    const EdgeIterator it = getEdgeIterator(index);

    const Vector3<float> &point1 = getVert(it.getEdgeVertexIndex()).pos;
    const Vector3<float> &point2 = getVert(it.next().getEdgeVertexIndex()).pos;
    const Vector3<float> &point3 = getVert(it.next().getEdgeVertexIndex()).pos;

    const Vector3<float> edge1 = point2 - point1;
    const Vector3<float> edge2 = point3 - point1;

    return Cross(edge1, edge2).Normalize();
}

std::vector<Vector3<float> > Debugpoint::buildVertexData() {

    std::vector<Vector3<float> > vertexData;

    for(unsigned int i = 0; i < mVerts.size(); i++) {
        vertexData.push_back(mVerts[i].pos);
    }

    return vertexData;
}


void Debugpoint::buildRenderData() {

    for(int i = 0; i < mFaces.size(); i++ ){
        Face &face = getFace(i);

        HalfEdge* edge = &getEdge(face.edge);

        Vertex &v1 = getVert(edge->vert);
        edge = &getEdge(edge->next);

        Vertex &v2 = getVert(edge->vert);
        edge = &getEdge(edge->next);

        Vertex &v3 = getVert(edge->vert);
        
        // Add vertices to our drawing list
        mOrderedVertexList.push_back(v1.pos);    
        mOrderedVertexList.push_back(v2.pos);
        mOrderedVertexList.push_back(v3.pos);
    }   
}


void Debugpoint::updateRenderData() {

    unsigned int vertIndex = 0;
    for(int i = 0; i < mFaces.size(); i++ ){
        Face &face = getFace(i);

        HalfEdge* edge = &getEdge(face.edge);

        Vertex &v1 = getVert(edge->vert);
        edge = &getEdge(edge->next);

        Vertex &v2 = getVert(edge->vert);
        edge = &getEdge(edge->next);

        Vertex &v3 = getVert(edge->vert);
        
        // Add vertices to our drawing list
        mOrderedVertexList[vertIndex]     = v1.pos;    
        mOrderedVertexList[vertIndex + 1] = v2.pos;
        mOrderedVertexList[vertIndex + 2] = v3.pos;

        vertIndex += 3;
    }
}


std::vector<unsigned int> Debugpoint::findNeighborVertices(unsigned int vertIndex) const {

    // Collected vertices, sorted counter clockwise!
    std::vector<unsigned int> oneRing;

    // Save the first vertex for comparison
    unsigned int firstEdge = getEdge(getVert(vertIndex).edge).next;
    unsigned int tmpEdge   = firstEdge;

    while(true) {
        
        tmpEdge = getEdge(getEdge(getEdge(tmpEdge).next).pair).next;
        
        if(tmpEdge == firstEdge)
            break;

        oneRing.push_back(getEdge(tmpEdge).vert);
    }
    return oneRing;
}


std::vector<unsigned int> Debugpoint::findNeighborFaces(unsigned int vertIndex) const {

    // Collected faces, sorted counter clockwise
    std::vector<unsigned int> foundFaces;

    unsigned int firstEdge = getVert(vertIndex).edge;
    unsigned int tmpEdge   = firstEdge;

    foundFaces.push_back(getEdge(firstEdge).face);

    while(true) {

        tmpEdge = getEdge(getEdge(tmpEdge).prev).pair;

        if(firstEdge == tmpEdge)
            break;

        foundFaces.push_back(getEdge(tmpEdge).face);
    }

    return foundFaces;
}

