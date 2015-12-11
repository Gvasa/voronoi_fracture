    #include "Rectangle.h"

Rectangle::Rectangle(float w, float h, Vector3<float> p) 
: mPosition(p) {

    std::cout << "\nCreating plane ...\n";

    createVertices(w, h);

    mMaterial.color         = Vector4<float>(0.5f, 0.5f, 0.5f, 1.0f);
    mMaterial.ambient       = Vector4<float>(0.3f, 0.3f, 0.3f, 1.0f);
    mMaterial.diffuse       = Vector4<float>(0.8f, 0.8f, 0.8f, 1.0f);
    mMaterial.specular      = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    mMaterial.specularity   = 50.0f;
    mMaterial.shinyness     = 0.6f;

    mTransMat = glm::mat4(1.0f);

    std::cout << "\nPlane created!\n";
}


Rectangle::~Rectangle() {

    mVerts.clear();             // Clear content
    mVerts.shrink_to_fit();     // Free memory

    mNormals.clear();
    mNormals.shrink_to_fit();
}


void Rectangle::initialize(Vector3<float> lightPosition) {
    
    std::cout << "\nInitializing Rectangle ...\n\n";

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders( "shaders/phongvertexshader.glsl", "shaders/phongfragmentshader.glsl" );

    // Set names for our uniforms, same as in shaders
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
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(Vector3<float>), &mNormals[0], GL_STATIC_DRAW);
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

    std::cout << "\nRectangle initialized!\n" << std::endl;
}


void Rectangle::render(std::vector<Matrix4x4<float> > sceneMatrices) {

    // Use shader
    glUseProgram(shaderProgram);

    // Pass values of our matrices and materials to the GPU via uniforms
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
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

    // Rebind the buffer data, normals are now updated
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(Vector3<float>), &mNormals[0], GL_STATIC_DRAW);

    // Draw geometry
    glDrawArrays(GL_TRIANGLES, 0, mVerts.size()); // 3 indices starting at 0 -> 1 triangle
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
}


void Rectangle::createVertices(float width, float height) {

    Vector3<float> v0 = Vector3<float>(-width, -height, 0.0f);
    Vector3<float> v1 = Vector3<float>(width, height, 0.0f);
    Vector3<float> v2 = Vector3<float>(-width, height, 0.0f);
    Vector3<float> v3 = Vector3<float>(width, -height, 0.0f);

    // Add vertices and normals
    addVertex(v0);
    addNormal(Vector3<float>(0.0f, 0.0f, 1.0f));
    addVertex(v1);
    addNormal(Vector3<float>(0.0f, 0.0f, 1.0f));
    addVertex(v2);
    addNormal(Vector3<float>(0.0f, 0.0f, 1.0f));
    addVertex(v0);
    addNormal(Vector3<float>(0.0f, 0.0f, 1.0f));
    addVertex(v3);
    addNormal(Vector3<float>(0.0f, 0.0f, 1.0f));
    addVertex(v1);
    addNormal(Vector3<float>(0.0f, 0.0f, 1.0f));

    calculateCenterOfMass();
}

void Rectangle::calculateCenterOfMass() {
    for (unsigned int i = 0; i < mVerts.size(); ++i)
        mCenterOfMass += mVerts[i];

    mCenterOfMass /= mVerts.size();

    std::cout << "COM: " << mCenterOfMass << std::endl;
}
void Rectangle::updateCenterOfMass(glm::mat4) {
    glm::vec4 tmpCom(mCenterOfMass[0], mCenterOfMass[1], mCenterOfMass[2], 1.0);

    tmpCom = mTransMat*tmpCom;

    mCenterOfMass = Vector3<float>(tmpCom.x, tmpCom.y, tmpCom.z);
    std::cout << "updaterad COM: " << mCenterOfMass << std::endl;
}
/*
void Rectangle::rotate(Vector3<float> axis, float angle) {

    // Compute rotation matrix
    mTransMat = mTransMat * glm::rotate(glm::mat4(1.f), angle, glm::vec3(axis[0], axis[1], axis[2]));
}


void Rectangle::translate(Vector3<float> p) {
    
    // Compute translation matrix
    mTransMat = mTransMat * glm::translate(glm::mat4(1.f), glm::vec3(p[0], p[1], p[2]));
    updateCenterOfMass(mTransMat);
}


void Rectangle::scale(Vector3<float> s) {

    // Compute scaling matrix
    mTransMat = mTransMat * glm::scale(glm::mat4(1.0f), glm::vec3(s[0], s[1], s[2]));
    updateCenterOfMass(mTransMat);
}

*/
void Rectangle::rotate(Vector3<float> axis, float angle) {

    // Compute rotation matrix
    Matrix4x4<float> rotationMatrix = Matrix4x4<float>::RotationXYZ(
        axis[0] * (angle * M_PI / 180.0f),
        axis[1] * (angle * M_PI / 180.0f),
        axis[2] * (angle * M_PI / 180.0f)
    );

    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply rotation to vertices
        Vector4<float> v = Vector4<float>(mVerts[i][0], mVerts[i][1], mVerts[i][2], 1.0f);
        v = rotationMatrix * v;
        mVerts[i] = Vector3<float>(v[0], v[1], v[2]);

        // Apply rotation to normals
        Vector4<float> n = Vector4<float>(mNormals[i][0], mNormals[i][1], mNormals[i][2], 1.0f);
        n = rotationMatrix * n;
        mNormals[i] = Vector3<float>(n[0], n[1], n[2]).Normalize();
    }

    //updateCenterOfMass(mTransMat);
}
 
// Translate the Mesh
void Rectangle::translate(Vector3<float> p){
   
    // Compute the translation matrix
    Matrix4x4<float> translationMatrix = Matrix4x4<float>::Translation(p[0], p[1], p[2]);
 
    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the rotation to the vertices
        Vector4<float> v = Vector4<float>(mVerts[i][0], mVerts[i][1], mVerts[i][2], 1.0f);
        v = translationMatrix * v;
        mVerts[i] = Vector3<float>(v[0], v[1], v[2]);
    }
   // updateCenterOfMass(mTransMat);
}
 
// Scale the Mesh
void Rectangle::scale(Vector3<float> s){
 
    // Compute the scaling matrix
    Matrix4x4<float> scalingMatrix = Matrix4x4<float>::Scale(s[0], s[1], s[2]);
 
    for(unsigned int i = 0; i < mVerts.size(); i++) {
        // Apply the scaling matrix
        Vector4<float> v = Vector4<float>(mVerts[i][0], mVerts[i][1], mVerts[i][2], 1.0f);
        v = scalingMatrix * v;
        mVerts[i] = Vector3<float>(v[0], v[1], v[2]);
    }
   // updateCenterOfMass(mTransMat);
}
