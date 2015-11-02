// Libs
#include <sstream>
#include <math.h>

// Headers
#include "HalfEdgeMesh.h"
#include "Scene.h"
#include "debug.h"

GLFWwindow* window;
Geometry *mesh;
Scene *scene;
std::string windowTitle = "Voronoi Fracture";

int initializeOpenGL();
void initializeScene();
double calcFPS(double, std::string);



int main (int argc, char* argv[]) {

    scene = new Scene();

    // Magic
    glewExperimental = GL_TRUE;
    
    // Init GLEW and GLFW
    if(initializeOpenGL() == -1) {
        return -1;
    }
    
    // Create geometries and add them to the scene
    mesh = new HalfEdgeMesh();
    scene->addGeometry(mesh);

    initializeScene();

    // Draw-loop
    do{
        calcFPS(1.0, windowTitle);
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

/*
 * Initialize OpenGL stuff here
 */
int initializeOpenGL() {
    
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

    window = glfwCreateWindow( 1024, 768, windowTitle.c_str(), NULL, NULL);
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

/*
 * Initialize scene stuff here, e.g. all geometries and scene
 */
void initializeScene() {

    // Init all geometries
    scene->initialize();
}

    
double calcFPS(double timeInterval = 1.0, std::string windowTitle = "NONE") {

    // Static values which only get initialised the first time the function runs
    static double startTime  =  glfwGetTime(); // Set the initial time to now
    static double fps        =  0.0;           // Set the initial FPS value to 0.0
 
    // Set the initial frame count to -1.0 (it gets set to 0.0 on the next line). Because
    // we don't have a start time we simply cannot get an accurate FPS value on our very
    // first read if the time interval is zero, so we'll settle for an FPS value of zero instead.
    static double frameCount =  -1.0;
 
    // Here again? Increment the frame count
    frameCount++;
 
    // Ensure the time interval between FPS checks is sane (low cap = 0.0 i.e. every frame, high cap = 10.0s)
    if (timeInterval < 0.0)
    {
        timeInterval = 0.0;
    }
    else if (timeInterval > 10.0)
    {
        timeInterval = 10.0;
    }
 
    // Get the duration in seconds since the last FPS reporting interval elapsed
    // as the current time minus the interval start time
    double duration = glfwGetTime() - startTime;
 
    // If the time interval has elapsed...
    if (duration > timeInterval)
    {
        // Calculate the FPS as the number of frames divided by the duration in seconds
        fps = round(frameCount / duration);
 
        // If the user specified a window title to append the FPS value to...
        if (windowTitle != "NONE")
        {
            // Convert the fps value into a string using an output stringstream
            std::ostringstream stream;
            stream << fps;
            std::string fpsString = stream.str();
 
            // Append the FPS value to the window title details
            windowTitle += " | FPS: " + fpsString;
 
            // Convert the new window title to a c_str and set it
            const char* pszConstString = windowTitle.c_str();
            glfwSetWindowTitle(window, pszConstString);
        }
        else // If the user didn't specify a window to append the FPS to then output the FPS to the console
        {
            std::cout << "FPS: " << fps << std::endl;
        }
 
        // Reset the frame count to zero and set the initial time to be now
        frameCount        = 0.0;
        startTime = glfwGetTime();
    }
 
    // Return the current FPS - doesn't have to be used if you don't want it!
    return fps;
}
