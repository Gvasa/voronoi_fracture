#include "HalfEdgeMesh.h"

HalfEdgeMesh::HalfEdgeMesh(Vector4<float> c, std::string s)
    : mObjName(s) {

    mMaterial.color         = c;
    mMaterial.ambient       = Vector4<float>(0.3f, 0.3f, 0.3f, 1.0f);
    mMaterial.diffuse       = Vector4<float>(0.8f, 0.8f, 0.8f, 1.0f);
    mMaterial.specular      = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    mMaterial.specularity   = 50.0f;
    mMaterial.shinyness     = 0.6f;

    mPrevPos = Vector3<float>(0.0, 0.0, 0.0);
    mPrevRotAngle = 0.0;

    mTransMat = glm::mat4(1.0f);
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

    std::cout << "\nHalfEdgeMesh deleted\n";
}

// Add init stuff here, right now its just some random shit for the red ugly triangle
void HalfEdgeMesh::initialize(Vector3<float> lightPosition) {

    std::cout << "\nInitializing Half-Edge mesh ...\n\n";

    mBoundingbox = new Boundingbox(buildVertexData());

    mBoundingbox->initialize();

    mBoundingbox->setWireFrame(true);

    buildRenderData();

    //calculateWorldCenterOfMass();

    mTransformedVertexList = mOrderedVertexList;

    // Update face normals
    for(unsigned int i = 0; i < mFaces.size(); i++) {
        getFace(i).normal = calculateFaceNormal(i);
        //std::cout << getFace(i).normal << std::endl;
    }

    // Update vertex normals
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
    glDisable( GL_BLEND );
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

    /*std::cout << "--- Face ---" << std::endl;

    std::cout << "vert 0: " << verts[0] << std::endl;
    std::cout << "vert 1: " << verts[1] << std::endl;
    std::cout << "vert 2: " << verts[2] << std::endl;

    std::cout << "------------\n\n";*/

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
    getEdge(innerHalfEdgeIndex1).prev = innerHalfEdgeIndex3;

    getEdge(innerHalfEdgeIndex2).next = innerHalfEdgeIndex3;
    getEdge(innerHalfEdgeIndex2).prev = innerHalfEdgeIndex1;

    getEdge(innerHalfEdgeIndex3).next = innerHalfEdgeIndex1;
    getEdge(innerHalfEdgeIndex3).prev = innerHalfEdgeIndex2;

    // Create the face, don't forget to set the normal! (which should be normalized of course)
    Face face;
    mFaces.push_back(face);
    mFaces.back().edge = innerHalfEdgeIndex1;
    mFaces.back().normal = calculateFaceNormal(mFaces.size() - 1);

    // all half-edges share the same left face (previously added)
    getEdge(innerHalfEdgeIndex1).face = mFaces.size() - 1;
    getEdge(innerHalfEdgeIndex2).face = mFaces.size() - 1;
    getEdge(innerHalfEdgeIndex3).face = mFaces.size() - 1;

    // Optionally, track the (outer) boundary half-edges
    // to represent non-closed surfaces
    if(getEdge(getEdge(innerHalfEdgeIndex1).pair).face == UNINITIALIZED ||
       getEdge(getEdge(innerHalfEdgeIndex1).pair).face == BORDER)
        MergeOuterBoundaryEdge(innerHalfEdgeIndex1);
    if(getEdge(getEdge(innerHalfEdgeIndex2).pair).face == UNINITIALIZED ||
       getEdge(getEdge(innerHalfEdgeIndex2).pair).face == BORDER)
        MergeOuterBoundaryEdge(innerHalfEdgeIndex2);
    if(getEdge(getEdge(innerHalfEdgeIndex3).pair).face == UNINITIALIZED ||
       getEdge(getEdge(innerHalfEdgeIndex3).pair).face == BORDER)
        MergeOuterBoundaryEdge(innerHalfEdgeIndex3);

    return true;
}

void HalfEdgeMesh::createMesh(std::string objName) {

    mObjName = objName;

    std::vector<std::vector<Vector3<float> > > vertexList = Geometry::mObjectLoader->getMeshVertexList(objName);

    for(unsigned int i = 0; i < vertexList.size(); i++) 
        addFace(vertexList[i]);

    calculateCenterOfMass();
}
/*
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
}*/

    // Rotate the mesh
void HalfEdgeMesh::rotate(Vector3<float> axis, float angle) {
   
    //Compute the rotational matrix
    std::cout << std::endl << "Rotating..." << std::endl;
    mTransMat = mTransMat * glm::rotate(glm::mat4(1.f), angle, glm::vec3(axis[0], axis[1], axis[2]));
 
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
 
        //Apply the rotation to the normals
        Vector4<float> n = Vector4<float>(mVerts[i].normal[0], mVerts[i].normal[1], mVerts[i].normal[2], 1.0f);
        n = rotationMatrix * n;
        mVerts[i].normal = Vector3<float>(n[0], n[1], n[2]).Normalize();
    }
    //updateCenterOfMass(mTransMat);
    calculateCenterOfMass();

}
 
// Translate the Mesh
void HalfEdgeMesh::translate(Vector3<float> p){
   
    // Compute the translation matrix
    mTransMat = mTransMat * glm::translate(glm::mat4(1.f),  glm::vec3(p[0], p[1], p[2]));
    Matrix4x4<float> translationMatrix = Matrix4x4<float>::Translation(p[0], p[1], p[2]);
 
    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the rotation to the vertices
        Vector4<float> v = Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);
        v = translationMatrix * v;
        mVerts[i].pos = Vector3<float>(v[0], v[1], v[2]);
    }

    glm::vec4 tmpPos;

    for(unsigned int i = 0; i < mVoronoiPoints.size(); i++) {
        
        tmpPos = glm::vec4(mVoronoiPoints[i][0], mVoronoiPoints[i][1], mVoronoiPoints[i][2], 1.0f);

        tmpPos = mTransMat*tmpPos;

        mVoronoiPoints[i] = Vector3<float>(tmpPos.x, tmpPos.y, tmpPos.z);
        std::cout << "mVoronoiPoints[" << i << "]" << mVoronoiPoints[i] << std::endl;
    }

    if(mDebugPoints.size() > 0) {
        for (unsigned int i = 0; i < mDebugPoints.size(); i++) {
            mDebugPoints[i]->updatePosition(p);
        }
    }

    calculateCenterOfMass();

}
 
// Scale the Mesh
void HalfEdgeMesh::scale(Vector3<float> s){
 
    // Compute the translation matrix
    mTransMat = mTransMat * glm::scale(glm::mat4(1.f),  glm::vec3(s[0], s[1], s[2]));
    Matrix4x4<float> translationMatrix = Matrix4x4<float>::Scale(s[0], s[1], s[2]);
 
    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the rotation to the vertices
        Vector4<float> v = Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);
        v = translationMatrix * v;
        mVerts[i].pos = Vector3<float>(v[0], v[1], v[2]);
    }

    glm::vec4 tmpPos;

    for(unsigned int i = 0; i < mVoronoiPoints.size(); i++) {
        
        tmpPos = glm::vec4(mVoronoiPoints[i][0], mVoronoiPoints[i][1], mVoronoiPoints[i][2], 1.0f);

        tmpPos = mTransMat*tmpPos;

        mVoronoiPoints[i] = Vector3<float>(tmpPos.x, tmpPos.y, tmpPos.z);
        std::cout << "mVoronoiPoints[" << i << "]" << mVoronoiPoints[i] << std::endl;
    }

    calculateCenterOfMass();
}


float HalfEdgeMesh::volume() const {

    float volume = 0.0f;
    float area = 0.0f;
    Vector3<float> normal;
    Vector3<float> v1, v2, v3;
    unsigned int edgeIndex;

    for(unsigned int i = 0; i < mFaces.size(); i++) {
        
        //if(getEdge(getFace(i).edge).face == BORDER)
          //  continue;

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


void HalfEdgeMesh::updateVoronoiPoints() {

    Matrix4x4<float> transformMatrix = toMatrix4x4(mTransMat);
 
    for(unsigned int i = 0; i < mVoronoiPoints.size(); i++) {
        // Apply the rotation to the vertices
        Vector4<float> v = transformMatrix * Vector4<float>(mVoronoiPoints[i][0], mVoronoiPoints[i][1], mVoronoiPoints[i][2], 1.0f);
        mVoronoiPoints[i] = Vector3<float>(v[0], v[1], v[2]);
    }

 
}

/*
void HalfEdgeMesh::updateVoronoiPattern() {

    Trans getWorldCenterOfMass() - mInitialWorldCenterOfMass;
}*/


void HalfEdgeMesh::computeVoronoiPattern() {

    if(!mCompoundIsComputed) {
       // updateVoronoiPattern();
        mCompound = new Compound(mBoundingbox, mVoronoiPoints);
        mCompoundIsComputed = true;
    }
}

void HalfEdgeMesh::calculateCenterOfMass() {

    Vector4<float>tmpCenterOfMass = Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < mVerts.size(); ++i)
        tmpCenterOfMass += Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);

    tmpCenterOfMass = toMatrix4x4(glm::inverse(mTransMat)) * tmpCenterOfMass;
    tmpCenterOfMass /= mVerts.size();

    mCenterOfMass = Vector3<float>(tmpCenterOfMass[0], tmpCenterOfMass[1], tmpCenterOfMass[2]);

    std::cout << "COM: " << mCenterOfMass << std::endl;
}

void HalfEdgeMesh::updateCenterOfMass(glm::mat4) {
    glm::vec4 tmpCom(mCenterOfMass[0], mCenterOfMass[1], mCenterOfMass[2], 1.0);

    tmpCom = mTransMat*tmpCom;

    mCenterOfMass = Vector3<float>(tmpCom.x, tmpCom.y, tmpCom.z);
    //hstd::cout << "updaterad COM: " << mCenterOfMass << std::endl;
}

std::vector<Vector3<float> > HalfEdgeMesh::getUniqueVertexList() {

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


void HalfEdgeMesh::MergeOuterBoundaryEdge(unsigned int innerEdge) {
    
    // 1. Merge first loop (around innerEdge->vert)
    // 2. Find leftmost edge, last edge counter clock-wise
    // 3. Test if there's anything to merge
    // 3a. If so merge the gap
    // 3b. And set border flags
    // 4. Merge second loop (around innerEdge->pair->vert)
    
    unsigned int tmpEdge = innerEdge;
    unsigned int tmpPrev;

    // Fetch the last edge counter clock-wise, i.e. the halfedge with a face that is UNINITIALIZED or BORDER
    while(true) {
        if(getEdge(getEdge(getEdge(tmpEdge).prev).pair).face == UNINITIALIZED ||
           getEdge(getEdge(getEdge(tmpEdge).prev).pair).face == BORDER) {
            tmpEdge = getEdge(getEdge(tmpEdge).prev).pair;
            tmpPrev = getEdge(tmpEdge).prev;
            break;
        }
        tmpEdge = getEdge(getEdge(tmpEdge).prev).pair;
    }

    // This case should always happen, or something is very very very wrong.
    if(tmpEdge != innerEdge) {
        getEdge(getEdge(innerEdge).pair).next = tmpEdge;
        getEdge(tmpEdge).prev = getEdge(innerEdge).pair;
        getEdge(getEdge(innerEdge).pair).face = BORDER;
        getEdge(tmpEdge).face = BORDER;
    }

    // Reset edge iterator
    tmpEdge = innerEdge;
    unsigned int tmpNext;
    // Fetch the last edge clock-wise, i.e. the halfedge with a face that is UNINITIALIZED or BORDER
    while(true) {
        if(getEdge(getEdge(getEdge(tmpEdge).next).pair).face == UNINITIALIZED ||
           getEdge(getEdge(getEdge(tmpEdge).next).pair).face == BORDER) {
            tmpEdge = getEdge(getEdge(tmpEdge).next).pair;
            tmpNext = getEdge(tmpNext).next;
            break;
        }
        tmpEdge = getEdge(getEdge(tmpEdge).next).pair;
    }

    if(tmpEdge != innerEdge) {
        getEdge(getEdge(innerEdge).pair).prev = tmpEdge;
        getEdge(tmpEdge).next = getEdge(innerEdge).pair;
        getEdge(getEdge(innerEdge).pair).face = BORDER;
        getEdge(tmpEdge).face = BORDER;
    }


}


void HalfEdgeMesh::addVoronoiPoint(Vector3<float> v) {

    mDebugPoints.push_back(new Debugpoint(v, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f)));
    mVoronoiPoints.push_back(v);

    mDebugPoints[mDebugPoints.size()-1]->initialize(Vector3<float>(0.0f, 0.0f, 0.0f));

}


void HalfEdgeMesh::deleteLastVoronoiPoint() {

    delete mDebugPoints.back();
    mDebugPoints.erase(mDebugPoints.end()-1);
    mVoronoiPoints.erase(mVoronoiPoints.end()-1);
}


//! Compute and return the normal at face at faceIndex
Vector3<float> HalfEdgeMesh::calculateFaceNormal(unsigned int faceIndex) const {

    if(getEdge(getFace(faceIndex).edge).face == BORDER) {
        return Vector3<float>(0.0f, 1.0f, 0.0f);
    }

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
        if(getEdge(getFace(faces[i]).edge).face != BORDER && getEdge(getFace(faces[i]).edge).face != UNINITIALIZED) {
            normal += getFace(faces[i]).normal;
        }
    }
    //std::cout << "\nvert normal: " << normal.Normalize() << std::endl;
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

    std::cout << "\n--------- buildRenderData ---------" << std::endl;

    for(int i = 0; i < mFaces.size(); i++ ){
        
        Face face = getFace(i);

        HalfEdge edge = getEdge(face.edge);

        Vertex v1 = getVert(edge.vert);
        edge = getEdge(edge.next);

        Vertex v2 = getVert(edge.vert);
        edge = getEdge(edge.next);

        Vertex v3 = getVert(edge.vert);

        // Add vertices to our drawing list
        mOrderedVertexList.push_back(v1.pos);    
        mOrderedVertexList.push_back(v2.pos);
        mOrderedVertexList.push_back(v3.pos);

        //std::cout << "\ninnan normal bestäms" << std::endl;
        Vector3<float> faceNormal = getFace(i).normal;//Vector3<float>(0.0001f, 0.0001f, 0.0001f);
     /*   
        if(getEdge(v1.edge).face != BORDER && getEdge(v1.edge).face != UNINITIALIZED) {
            //std::cout << "v1.normal" << std::endl;
            faceNormal = getFace(getEdge(v1.edge).face).normal;
        }
        else if(getEdge(v2.edge).face != BORDER && getEdge(v2.edge).face != UNINITIALIZED) {
            //std::cout << "v2.normal" << std::endl;
            faceNormal = getFace(getEdge(v2.edge).face).normal;
        }
        else if(getEdge(v3.edge).face != BORDER && getEdge(v3.edge).face != UNINITIALIZED) {
            //std::cout << "v3.normal" << std::endl;
            faceNormal = getFace(getEdge(v3.edge).face).normal;
        }
        else if(getEdge(getEdge(v1.edge).pair).face != BORDER && getEdge(getEdge(v1.edge).pair).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(getEdge(v1.edge).pair).face).normal;
        else if(getEdge(getEdge(v2.edge).pair).face != BORDER && getEdge(getEdge(v2.edge).pair).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(getEdge(v2.edge).pair).face).normal;
        else if(getEdge(getEdge(v3.edge).pair).face != BORDER && getEdge(getEdge(v3.edge).pair).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(getEdge(v3.edge).pair).face).normal;
        //std::cout << "efter normal bestäms" << std::endl;
*/
        // Add normals to our drawing list
        mOrderedNormalList.push_back(faceNormal);
        mOrderedNormalList.push_back(faceNormal);
        mOrderedNormalList.push_back(faceNormal);
    }
    std::cout << "\n--------- buildRenderData COMPLETE !!! ---------" << std::endl;
}


void HalfEdgeMesh::updateRenderData() {

    unsigned int vertIndex = 0;
    for(int i = 0; i < mFaces.size(); i++ ){

        //if(getEdge(getFace(i).edge).face == BORDER)
          //  continue;

        Face face = getFace(i);

        HalfEdge edge = getEdge(face.edge);

        Vertex v1 = getVert(edge.vert);
        edge = getEdge(edge.next);

        Vertex v2 = getVert(edge.vert);
        edge = getEdge(edge.next);

        Vertex v3 = getVert(edge.vert);
        
        // Add vertices to our drawing list
        mOrderedVertexList[vertIndex]     = v1.pos;   
        mOrderedVertexList[vertIndex + 1] = v2.pos;
        mOrderedVertexList[vertIndex + 2] = v3.pos;

        //Vector3<float> faceNormal = Vector3<float>(0.0001f, 0.0001f, 0.0001f);
        Vector3<float> faceNormal = getFace(i).normal;
        /*if(getEdge(v1.edge).face != BORDER && getEdge(v1.edge).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(v1.edge).face).normal;
        else if(getEdge(v2.edge).face != BORDER && getEdge(v2.edge).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(v2.edge).face).normal;
        else if(getEdge(v3.edge).face != BORDER && getEdge(v3.edge).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(v3.edge).face).normal;
        else if(getEdge(getEdge(v1.edge).pair).face != BORDER && getEdge(getEdge(v1.edge).pair).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(getEdge(v1.edge).pair).face).normal;
        else if(getEdge(getEdge(v2.edge).pair).face != BORDER && getEdge(getEdge(v2.edge).pair).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(getEdge(v2.edge).pair).face).normal;
        else if(getEdge(getEdge(v3.edge).pair).face != BORDER && getEdge(getEdge(v3.edge).pair).face != UNINITIALIZED)
            faceNormal = getFace(getEdge(getEdge(v3.edge).pair).face).normal;
*/
        // Add normals to our drawing list
        mOrderedNormalList[vertIndex]     = faceNormal;
        mOrderedNormalList[vertIndex + 1] = faceNormal;
        mOrderedNormalList[vertIndex + 2] = faceNormal;

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

    if(getEdge(firstEdge).face != UNINITIALIZED && getEdge(firstEdge).face != BORDER && getEdge(firstEdge).face < mFaces.size())
        foundFaces.push_back(getEdge(firstEdge).face);

    while(true) {

        tmpEdge = getEdge(getEdge(tmpEdge).prev).pair;

        if(firstEdge == tmpEdge||getEdge(tmpEdge).face == BORDER) {
            break;
        }

        if(getEdge(tmpEdge).face != UNINITIALIZED && getEdge(tmpEdge).face != BORDER && getEdge(tmpEdge).face < mFaces.size()){
            foundFaces.push_back(getEdge(tmpEdge).face);
        }
    }

    return foundFaces;
}


void HalfEdgeMesh::printMesh() {

    for(unsigned int i = 0; i < mFaces.size(); i++) {
        std::cout << "\n--- face " << i << " ---" << std::endl;
        std::cout << "v0: " << getVert(getEdge(getFace(i).edge).vert).pos << std::endl;
        std::cout << "v1: " << getVert(getEdge(getEdge(getFace(i).edge).next).vert).pos << std::endl;
        std::cout << "v2: " << getVert(getEdge(getEdge(getEdge(getFace(i).edge).next).next).vert).pos << std::endl;
    }
}


unsigned int HalfEdgeMesh::getEdge(Vector3<float> vertPos) {

    for(unsigned int i = 0; i < mEdges.size(); i++) {
        if(getVert(getEdge(i).vert).pos == vertPos && getEdge(i).face == BORDER)
            return i;
    }
}


Matrix4x4<float> HalfEdgeMesh::toMatrix4x4(glm::mat4 m) {
    float M[4][4] = {
        {m[0][0], m[1][0], m[2][0], m[3][0]},
        {m[0][1], m[1][1], m[2][1], m[3][1]},
        {m[0][2], m[1][2], m[2][2], m[3][2]},
        {m[0][3], m[1][3], m[2][3], m[3][3]}
    };
    return Matrix4x4<float>(M);
}


Vector3<float> HalfEdgeMesh::getWorldCenterOfMass() {

    Vector4<float>WCM = toMatrix4x4(mTransMat) * Vector4<float>(mCenterOfMass[0], mCenterOfMass[1], mCenterOfMass[2], 1.0f); 

    return Vector3<float>(WCM[0], WCM[1], WCM[2]);
}


void HalfEdgeMesh::updateMesh(glm::mat4 m) {

    mTransMat = m;

    Matrix4x4<float> transformMatrix = toMatrix4x4(mTransMat);
 
    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the rotation to the vertices
        Vector4<float> v = transformMatrix * Vector4<float>(mVerts[i].pos[0], mVerts[i].pos[1], mVerts[i].pos[2], 1.0f);
        mVerts[i].pos = Vector3<float>(v[0], v[1], v[2]);
    }

    for(unsigned int i = 0; i < mOrderedVertexList.size(); i++) {
        // Apply the rotation to the vertices
        Vector4<float> v = transformMatrix * Vector4<float>(mOrderedVertexList[i][0], mOrderedVertexList[i][1], mOrderedVertexList[i][2], 1.0f);
        mTransformedVertexList[i] = Vector3<float>(v[0], v[1], v[2]);
    }    

    /*std::cout << " asdasd ----------------" << std::endl;
    updateVoronoiPatterns(mObjName, transformMatrix);
    std::cout << " ---------- \n" << std::endl;*/
}

/*
void HalfEdgeMesh::calculateWorldCenterOfMass() {

    for(unsigned int i = 0; i < mVerts.size(); i++)
        mInitialWorldCenterOfMass += mVerts[i].pos;

    mInitialWorldCenterOfMass /= static_cast<float>(mVerts.size());
}*/
