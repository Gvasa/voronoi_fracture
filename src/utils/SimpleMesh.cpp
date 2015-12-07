#include "SimpleMesh.h"

SimpleMesh::SimpleMesh() {

    std::cout << "\nCreating SimpleMesh...";

    mMaterial.color         = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    mMaterial.ambient       = Vector4<float>(0.3f, 0.3f, 0.3f, 1.0f);
    mMaterial.diffuse       = Vector4<float>(0.8f, 0.8f, 0.8f, 1.0f);
    mMaterial.specular      = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    mMaterial.specularity   = 50.0f;
    mMaterial.shinyness     = 0.6f;

    std::cout << "SimpleMesh created!\n";
}


SimpleMesh::~SimpleMesh() {

    mVerts.clear();
    mVerts.shrink_to_fit();
}


void SimpleMesh::initialize(Vector3<float> lightPosition) {

    std::cout << "\nInitializing Simple Mesh...\n\n";

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );

    MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
    ColorLoc = glGetUniformLocation(shaderProgram, "color");

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

    std::cout << "\nSimple Mesh initialized!\n" << std::endl;
}


void SimpleMesh::render(std::vector<Matrix4x4<float> > sceneMatrices) {

    // Use shader
    glUseProgram(shaderProgram);
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &sceneMatrices[I_MVP](0, 0));
    glUniform4f(ColorLoc, mMaterial.color[0], mMaterial.color[1], mMaterial.color[2], mMaterial.color[3]);

    // Rebind the buffer data, vertices are now updated
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, mVerts.size()); // 3 indices starting at 0 -> 1 triangle
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
}


bool SimpleMesh::addFace(const std::vector<Vector3<float> >verts) {

    if(verts.size() != 3) {
        std::cout << "\nInvalid amount of vertices in the polygon, please pass 3!\n";
        std::cout << "Passed " << verts.size() << " vertices" << std::endl;
        return false;
    }

    for(unsigned int i = 0; i < verts.size(); i++)
        mVerts.push_back(verts[i]);

    return true;
}