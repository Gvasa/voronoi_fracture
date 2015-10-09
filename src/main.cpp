
// Libs


// Headers
#include "Scene.h"
#include "HalfEdgeMesh.h"
#include "debug.h"

GLFWwindow* window;
Geometry *mesh;
Scene *scene;

int initOpenGL();

int main (int argc, char* argv[]) {

    scene = new Scene();

    // Magic
    glewExperimental = GL_TRUE;
    
    // Init GLEW and GLFW
    if(initOpenGL() == -1) {
        return -1;
    }
    
    mesh = new HalfEdgeMesh();
    scene->addGeometry(mesh);

    // Init all geometries
    scene->init();

    // Draw-loop
    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        // Draw all geometries
        scene->draw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Clean-up
    delete scene;

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


int initOpenGL() {
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. You might be having an old GPU\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    std::cout << "Running OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return 0;
}
