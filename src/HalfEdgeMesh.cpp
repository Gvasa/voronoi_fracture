#include "HalfEdgeMesh.h"

// Set constants for flaging of edges that are not valid half-edges
const unsigned int HalfEdgeMesh::BORDER = (std::numeric_limits<unsigned int>::max)();
const unsigned int HalfEdgeMesh::UNINITIALIZED = (std::numeric_limits<unsigned int>::max)()-1;


HalfEdgeMesh::~HalfEdgeMesh() {

    // Cleanup VBO
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);
}

// Add init stuff here, right now its just some random shit for the red ugly triangle
void HalfEdgeMesh::initialize() {

    std::cout << "\nInitializing Half-Edge mesh ...\n\n";

    glGenVertexArrays(1, &vertexArrayID);
    
    glBindVertexArray(vertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );

    MVPLoc = glGetUniformLocation(shaderProgram, "MVP");

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
    };

    verts.push_back(Vector3<float>(-1.0f, -1.0f, 0.0f));
    verts.push_back(Vector3<float>(1.0f, -1.0f, 0.0f));
    verts.push_back(Vector3<float>(0.0f, 1.0f, 0.0f));

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vector3<float>), &verts[0], GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    std::cout << "\nHalf-Edge mesh initialized!\n" << std::endl;
}

// Add draw stuff here, right now its just some random shit for the red ugly triangle
void HalfEdgeMesh::draw(Matrix4x4<float> MVP) {

    // Use our shader
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVP(0, 0));

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vector3<float>), &verts[0], GL_STATIC_DRAW);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, verts.size()); // 3 indices starting at 0 -> 1 triangle
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
}

// This is where we add a face to the half-edge structure
bool HalfEdgeMesh::addFace(std::vector<Vector3 <float> > verts) {
   std::cout << "addFAce" << std::endl;
}

// This is where we add an edge to the half-edge structure
bool HalfEdgeMesh::addEdge() {
    std::cout << "addEdge" << std::endl;
}

// This is where we add a vertex to the half-edge structure
bool HalfEdgeMesh::addVertex() {
    std::cout << "addVertex" << std::endl;
}