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

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    std::cout << "\nHalf-Edge mesh initialized!\n" << std::endl;
}

// Add draw stuff here, right now its just some random shit for the red ugly triangle
void HalfEdgeMesh::draw() {

    // Use our shader
    glUseProgram(shaderProgram);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
}

// This is where we add a face to the half-edge structure
bool HalfEdgeMesh::addFace() {
    // Add verts and edges here
}

// This is where we add an edge to the half-edge structure
bool HalfEdgeMesh::addEdge() {
    
}

// This is where we add a vertex to the half-edge structure
bool HalfEdgeMesh::addVertex() {
    
}