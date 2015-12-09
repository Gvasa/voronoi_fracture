#include "HalfEdgeMesh.h"

// Set constants for flaging of edges that are not valid half-edges
const unsigned int HalfEdgeMesh::BORDER = (std::numeric_limits<unsigned int>::max)();
const unsigned int HalfEdgeMesh::UNINITIALIZED = (std::numeric_limits<unsigned int>::max)()-1;


HalfEdgeMesh::HalfEdgeMesh() {

    mMaterial.color         = Vector4<float>(0.2f, 0.6f, 0.2f, 0.4f);
    mMaterial.ambient       = Vector4<float>(0.3f, 0.3f, 0.3f, 1.0f);
    mMaterial.diffuse       = Vector4<float>(0.8f, 0.8f, 0.8f, 1.0f);
    mMaterial.specular      = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    mMaterial.specularity   = 50.0f;
    mMaterial.shinyness     = 0.6f;

    mPrevPos = Vector3<float>(0.0, 0.0, 0.0);
    mPrevRotAngle = 0.0;

    mTransMat = glm::mat4(1.0f);
    //mPrevRot = std::make_pair(Vector3<float>(0.0, 0.0, 0.0), 0.0);
}


HalfEdgeMesh::~HalfEdgeMesh() {

    // Cleanup VBO
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    if(mEdges.size() > 0) {
        mEdges.clear();
        mEdges.shrink_to_fit();
    }

    if(mVerts.size() > 0) {
        mVerts.clear();
        mVerts.shrink_to_fit();
    }

    if(mFaces.size() > 0) {
        mFaces.clear();
        mFaces.shrink_to_fit();
    }

    if(mOrderedVertexList.size() > 0) {
        mOrderedVertexList.clear();
        mOrderedVertexList.shrink_to_fit();
    }

    if(mOrderedNormalList.size() > 0) {
        mOrderedNormalList.clear();
        mOrderedNormalList.shrink_to_fit();
    }

    if(mVoronoiPoints.size() > 0) {
        mVoronoiPoints.clear();
        mVoronoiPoints.shrink_to_fit();
    }

    if(mUniqueVerts.size() > 0) {
        mUniqueVerts.clear();
    }

    for(unsigned int i = 0; i < mDebugPoints.size(); ++i)
        delete mDebugPoints[i];

    mDebugPoints.clear();
    mDebugPoints.shrink_to_fit();

    if(mBoundingbox != NULL)
        delete mBoundingbox;

    if(mCompound != NULL)
        delete mCompound;

}

// Add init stuff here, right now its just some random shit for the red ugly triangle
void HalfEdgeMesh::initialize(Vector3<float> lightPosition) {

    std::cout << "\nInitializing Half-Edge mesh ...\n\n";

    mBoundingbox = new Boundingbox(buildVertexData());
    mBoundingbox->initialize();
    mBoundingbox->setWireFrame(true);

    /*for(unsigned int i = 0; i < mDebugPoints.size(); i++)
        mDebugPoints[i]->initialize(lightPosition);*/
    std::cout << "Volume: " << volume() << std::endl << std::endl;

    buildRenderData();

    // Update face normals
    for(unsigned int i = 0; i < mFaces.size(); i++) {
        getFace(i).normal = calculateFaceNormal(i);
        //std::cout << getFace(i).normal << std::endl;
    }

    // Update face normals
    for(unsigned int i = 0; i < mVerts.size(); i++)
        getVert(i).normal = calculateVertNormal(i);

    // Update the lists that we draw
    updateRenderData();

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    shaderProgram = LoadShaders( "shaders/phongvertexshader.glsl", "shaders/phongfragmentshader.glsl" );

    MVPLoc          = glGetUniformLocation(shaderProgram, "MVP");
    MVLoc           = glGetUniformLocation(shaderProgram, "MV");
    MVLightLoc      = glGetUniformLocation(shaderProgram, "MV_light");
    NMLoc           = glGetUniformLocation(shaderProgram, "NM");
    lightPosLoc     = glGetUniformLocation(shaderProgram, "lightPos");
    colorLoc        = glGetUniformLocation(shaderProgram, "color");
    lightAmbLoc     = glGetUniformLocation(shaderProgram, "ambientColor");
    lightDifLoc     = glGetUniformLocation(shaderProgram, "diffuseColor");
    lightSpeLoc     = glGetUniformLocation(shaderProgram, "specularColor");
    specularityLoc  = glGetUniformLocation(shaderProgram, "specularity");
    shinynessLoc    = glGetUniformLocation(shaderProgram, "shinyness");

    glUniform4f(lightPosLoc, lightPosition[0], lightPosition[1], lightPosition[2], 1.0f);

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


    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mOrderedNormalList.size() * sizeof(Vector3<float>), &mOrderedNormalList[0], GL_STATIC_DRAW);
    // 2nd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                          // attribute 1. I.e. layout 1 in shader
        3,                          // size
        GL_FLOAT,                   // type
        GL_FALSE,                   // normalized?
        0,                          // stride
        reinterpret_cast<void*>(0)  // array buffer offset
    );

    std::cout << "\nHalf-Edge mesh initialized!\n" << std::endl;
}

// Add draw stuff here, right now its just some random shit for the red ugly triangle
void HalfEdgeMesh::render(std::vector<Matrix4x4<float> > sceneMatrices) {

    // Use shader
    glUseProgram(shaderProgram);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );  
    
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &sceneMatrices[I_MVP](0, 0));
    glUniformMatrix4fv(MVLoc, 1, GL_FALSE, &sceneMatrices[I_MV](0, 0));
    glUniformMatrix4fv(MVLightLoc, 1, GL_FALSE, &sceneMatrices[I_MV_LIGHT](0, 0));
    glUniformMatrix4fv(NMLoc, 1, GL_FALSE, &sceneMatrices[I_NM](0, 0));
    glUniform4f(colorLoc, mMaterial.color[0], mMaterial.color[1], mMaterial.color[2], mMaterial.color[3]);
    glUniform4f(lightAmbLoc, mMaterial.ambient[0], mMaterial.ambient[1], mMaterial.ambient[2], mMaterial.ambient[3]);
    glUniform4f(lightDifLoc, mMaterial.diffuse[0], mMaterial.diffuse[1], mMaterial.diffuse[2], mMaterial.diffuse[3]);
    glUniform4f(lightSpeLoc, mMaterial.specular[0], mMaterial.specular[1], mMaterial.specular[2], mMaterial.specular[3]);
    glUniform1f(specularityLoc, mMaterial.specularity);
    glUniform1f(shinynessLoc, mMaterial.shinyness);

    // Rebind the buffer data, vertices are now updated
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mOrderedVertexList.size() * sizeof(Vector3<float>), &mOrderedVertexList[0], GL_STATIC_DRAW);

    // Rebind the buffer data, normals are now updated
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mOrderedNormalList.size() * sizeof(Vector3<float>), &mOrderedNormalList[0], GL_STATIC_DRAW);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, mOrderedVertexList.size()); // 3 indices starting at 0 -> 1 triangle
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glDisable( GL_BLEND );

    mBoundingbox->render(sceneMatrices[I_MVP]);
    
    if(mCompoundIsComputed)
        mCompound->render(sceneMatrices[I_MVP]);

    if(mDebugMode) {
        for(unsigned int i = 0; i < mDebugPoints.size(); i++)
            mDebugPoints[i]->render(sceneMatrices);
    }

    //mTransMat = Matrix4x4<float>();

}

// This is where we add a face to the half-edge structure
bool HalfEdgeMesh::addFace(const std::vector<Vector3 <float> > verts) {

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

    // Create the face, don't forget to set the normal! (which should be normalized ofcourse)
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

void HalfEdgeMesh::createMesh(std::string objName) {

    loadVoronoiPoints(objName);

    std::vector<std::vector<Vector3<float> > > vertexList = Geometry::mObjectLoader->getMeshVertexList(objName);

    for(unsigned int i = 0; i < vertexList.size(); i++) 
        addFace(vertexList[i]);

    calculateCenterOfMass();
}

// Rotate the mesh
void HalfEdgeMesh::rotate(Vector3<float> axis, float angle) {

    mTransMat = mTransMat * glm::rotate(glm::mat4(1.f), angle, glm::vec3(axis[0], axis[1], axis[2]));
}

// Translate the Mesh
void HalfEdgeMesh::translate(Vector3<float> p){

    mTransMat = mTransMat * glm::translate(glm::mat4(1.f),  glm::vec3(p[0], p[1], p[2]));
    updateCenterOfMass(mTransMat);

}

// Scale the Mesh 
void HalfEdgeMesh::scale(Vector3<float> s){

    mTransMat = mTransMat * glm::scale(glm::mat4(1.0f), glm::vec3(s[0], s[1], s[2]));
    updateCenterOfMass(mTransMat);
}


float HalfEdgeMesh::volume() const {

    float volume = 0.0f;
    float area = 0.0f;
    Vector3<float> normal;
    Vector3<float> v1, v2, v3;
    unsigned int edgeIndex;

    for(unsigned int i = 0; i < mFaces.size(); i++) {
        
        edgeIndex = getFace(i).edge;
        v1 = getVert(getEdge(edgeIndex).vert).pos;
        v2 = getVert(getEdge(getEdge(edgeIndex).next).vert).pos;
        v3 = getVert(getEdge(getEdge(edgeIndex).prev).vert).pos;

        area = (Cross(v2 - v1, v3 - v1).Length()) / 2.0f;

        normal = getFace(i).normal;

        volume += ((v1 + v2 + v3) / 3.0f) * (normal * area);
    }
    return volume / 3.0f;
}


void HalfEdgeMesh::updateVoronoiPoint(Vector3<float> dp, unsigned int index) {

    mVoronoiPoints[index] += dp;

    mDebugPoints[index]->updatePosition(dp);
}


void HalfEdgeMesh::computeVoronoiPattern() {

    mCompound = new Compound(mBoundingbox, mVoronoiPoints);
    mCompound->initialize();

    mCompoundIsComputed = true;
}

void HalfEdgeMesh::calculateCenterOfMass() {
    for (unsigned int i = 0; i < mVerts.size(); ++i)
        mCenterOfMass += mVerts[i].pos;

    mCenterOfMass /= mVerts.size();

    std::cout << "COM: " << mCenterOfMass << std::endl;
}

void HalfEdgeMesh::updateCenterOfMass(glm::mat4) {
    glm::vec4 tmpCom(mCenterOfMass[0], mCenterOfMass[1], mCenterOfMass[2], 1.0);

    tmpCom = mTransMat*tmpCom;

    mCenterOfMass = Vector3<float>(tmpCom.x, tmpCom.y, tmpCom.z);
    //hstd::cout << "updaterad COM: " << mCenterOfMass << std::endl;
}

std::vector<Vector3<float> > HalfEdgeMesh::getVertexList() {

    std::vector<Vector3<float> > vertexList;

    for(unsigned int i = 0; i < mVerts.size(); i++) {
        vertexList.push_back(mVerts[i].pos);
    }

    return vertexList;
}


// This is where we add a vertex to the half-edge structure
bool HalfEdgeMesh::addVertex(const Vector3<float> &v, unsigned int &index) {

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

//inserts a half edge pair between halfedgemesh point to by vert1 and vert2 
// the first HalfEdgeMesh::HalfEdge (vert1 -> vert2) is the inner one
// the second (vert2->vert1) is the outer one
bool HalfEdgeMesh::addHalfEdgePair(unsigned int vert1, unsigned int vert2, unsigned int &index1, unsigned int &index2) {

    //check if the pair to be added already exists
    std::map<OrderedPair, unsigned int>::iterator it = mUniqueEdgePairs.find(OrderedPair(vert1, vert2));

    //it it exists
    if(it != mUniqueEdgePairs.end()) {

        index1 = it->second;                //assign the first index to the place where its found in mUniqueEdgePairs
        index2 = getEdge(it->second).pair;  //assign the pair of the first edge to the second index
        
        if(vert1 != getEdge(index1).vert ) {    // check that both index got assigned correctly
            std::swap(index1, index2);          // otherwise swap
        }

        return false;
    }

    //if not found, calculate the indices 
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

void HalfEdgeMesh::addVoronoiPoint(Vector3<float> v) {

    mDebugPoints.push_back(new Debugpoint(v, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f)));
    mVoronoiPoints.push_back(v);

    mDebugPoints[mDebugPoints.size()-1]->initialize(Vector3<float>(0.0f, 0.0f, 0.0f));

}

//! Compute and return the normal at face at faceIndex
Vector3<float> HalfEdgeMesh::calculateFaceNormal(unsigned int faceIndex) const {

    unsigned int index = getFace(faceIndex).edge;
    const EdgeIterator it = getEdgeIterator(index);

    const Vector3<float> &point1 = getVert(it.getEdgeVertexIndex()).pos;
    const Vector3<float> &point2 = getVert(it.next().getEdgeVertexIndex()).pos;
    const Vector3<float> &point3 = getVert(it.next().getEdgeVertexIndex()).pos;

    const Vector3<float> edge1 = point2 - point1;
    const Vector3<float> edge2 = point3 - point1;

    return Cross(edge1, edge2).Normalize();
}


Vector3<float> HalfEdgeMesh::calculateVertNormal(unsigned int vertIndex) const {

    Vector3<float> normal(0.0f, 0.0f, 0.0f);

    std::vector<unsigned int> faces = findNeighborFaces(vertIndex);

    for(unsigned int i = 0; i < faces.size(); i++) {
        normal += getFace(faces[i]).normal;
    }

    return normal.Normalize();
}


std::vector<Vector3<float> > HalfEdgeMesh::buildVertexData() {

    std::vector<Vector3<float> > vertexData;

    for(unsigned int i = 0; i < mVerts.size(); i++) {
        vertexData.push_back(mVerts[i].pos);
    }
    return vertexData;
}


void HalfEdgeMesh::buildRenderData() {

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

        // Add normals to our drawing list
        mOrderedNormalList.push_back(v1.normal);
        mOrderedNormalList.push_back(v2.normal);
        mOrderedNormalList.push_back(v3.normal);
    }   
}


void HalfEdgeMesh::updateRenderData() {

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

        // Add normals to our drawing list
        mOrderedNormalList[vertIndex]     = v1.normal;
        mOrderedNormalList[vertIndex + 1] = v2.normal;
        mOrderedNormalList[vertIndex + 2] = v3.normal;

        vertIndex += 3;
    }

    mBoundingbox->updateBoundingBox(buildVertexData());

}


std::vector<unsigned int> HalfEdgeMesh::findNeighborVertices(unsigned int vertIndex) const {

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


std::vector<unsigned int> HalfEdgeMesh::findNeighborFaces(unsigned int vertIndex) const {

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

