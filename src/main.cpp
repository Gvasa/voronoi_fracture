// Libs
#include <sstream>
#include <istream>
#include <math.h>

// Headers
#include "HalfEdgeMesh.h"
#include "Rectangle.h"
#include "Scene.h"
#include "Controls.h"
#include "utils/Utils.h"
//#include "LoadObj.h"
#include "utils/Debugpoint.h"

GLFWwindow* window = nullptr;
Scene *scene = nullptr;
Geometry *mesh = nullptr;
Geometry *floor_rect = nullptr;
Geometry *wall_rect = nullptr;
Utils *utilHandler = nullptr;

std::string windowTitle = "Voronoi Fracture";

bool voronoiPatternIsComputed = false;
const unsigned int maximumVoronoiPoints = 7;
unsigned int currentNumberOfVoronoiPoints = 1;
int currentVoronoiIndex = 0;
float stepSize = 0.05f;

int initializeOpenGL();
void initializeScene();
void mouseButton(GLFWwindow* window, int button, int andction, int mods);
void mouseMotion(GLFWwindow* window, double x, double y);
void mouseScroll(GLFWwindow* window, double x, double y);
void keyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods);
double calcFPS(double, std::string);

int main (int argc, char* argv[]) {

    srand (static_cast<unsigned>(time(0)));


    // Magic
    glewExperimental = GL_TRUE;
    
    // Init GLEW and GLFW
    if(initializeOpenGL() == -1) {
        return -1;
    }

    scene = new Scene();
    utilHandler = new Utils();

    createPreDefinedVoronoiPoints();
    
    // Create geometries and add them to the scene

    // Floor
    floor_rect = new Rectangle(1.0f, 1.0f, Vector3<float>(0.0f, 0.0f, 0.0f));
    floor_rect->rotate(Vector3<float>(1.0f, 0.0f, 0.0f), 90.0f);
    floor_rect->translate(Vector3<float>(0.0f, -1.0f, 0.0f));
    floor_rect->scale(Vector3<float>(1.5f, 1.0f, 1.0f));

    // Wall
    wall_rect = new Rectangle(1.0f, 1.0f, Vector3<float>(0.0f, 0.0f, 0.0f));
    wall_rect->translate(Vector3<float>(0.0f, 0.0f, -1.0f));
    wall_rect->scale(Vector3<float>(1.5f, 1.0f, 1.0f));

    // HalfEdge mesh
    mesh = new HalfEdgeMesh(Vector4<float>(0.2f, 0.8f, 0.2f, 0.4f));
    mesh->addVoronoiPoint(Vector3<float>(0.0f, 0.0f, 0.0f));
    mesh->setDebugMode(true);

    //mesh->createMesh("lowPolySphere1.0");
    //mesh->createMesh("sphere1.0");
    //mesh->createMesh("icosphere");
    mesh->createMesh("icosphere");
    //mesh->createMesh("Sphere1.0_hole");
    //mesh->createMesh("bunnySmall");
    //mesh->createMesh("cube");
    //mesh->createMesh("cube_hole");s
    //mesh->createMesh("cow");
    //mesh->scale(Vector3<float>(0.5f, 0.5f, 0.5f));
    //mesh->translate(Vector3<float>(0.5f, -0.5f, 0.0f));
    mesh->translate(Vector3<float>(0.0f, 3.0f, 0.0f));

    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));
    scene->addGeometry(floor_rect, STATIC);
    //scene->addGeometry(wall_rect, STATIC);
    scene->addGeometry(mesh, DYNAMIC);

    initializeScene();

    //scene->setInitialVelocity(1, Vector3<float>(1.0f, 5.0f, 0.0f));

    //Set functions to handle mouse input
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, mouseMotion);
    glfwSetScrollCallback(window, mouseScroll);
    glfwSetKeyCallback(window, keyboardInput);

    // render-loop
    do{
        calcFPS(1.0, windowTitle);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render all geometries
        scene->render();
        scene->stepSimulation();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Clean-up
    //delete floor_rect;
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

    window = glfwCreateWindow( WIDTH, HEIGHT, windowTitle.c_str(), NULL, NULL);
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

// Function that handles input from mouse, sends the position to scene 
void mouseButton(GLFWwindow* window, int button, int action, int mods) {
    if(button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    switch(action) {
        case GLFW_PRESS:
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            scene->mouseButtonClick(x, y);
            break;
        case GLFW_RELEASE:
            scene->mouseButtonRelease();
            break;
        default:
            break;
    }
}

// handles mouse movement, send updated positions to the scene
void mouseMotion(GLFWwindow* window, double x, double y) {
    scene->updateCameraPosition(x, y);
}

void mouseScroll(GLFWwindow* window, double x, double y) {
    scene->updateCameraZoom(x, y);
}
    
void keyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) {

    std::vector<Vector3<float> > voronoiPattern;

    if(action == GLFW_PRESS) {
        
        switch(key) {
            
            case GLFW_KEY_SPACE:
                scene->resetCamera();
                break;

            case GLFW_KEY_W:
                if(!voronoiPatternIsComputed)
                    mesh->updateVoronoiPoint(Vector3<float>(0.0f, 0.0f, -stepSize), currentVoronoiIndex);
                break;

            case GLFW_KEY_S:
                if(!voronoiPatternIsComputed)
                    mesh->updateVoronoiPoint(Vector3<float>(0.0f, 0.0f, stepSize), currentVoronoiIndex);
                break;

            case GLFW_KEY_A:
                if(!voronoiPatternIsComputed)
                    mesh->updateVoronoiPoint(Vector3<float>(-stepSize, 0.0f, 0.0f), currentVoronoiIndex);
                break;

            case GLFW_KEY_D:
                if(!voronoiPatternIsComputed)
                    mesh->updateVoronoiPoint(Vector3<float>(stepSize, 0.0f, 0.0f), currentVoronoiIndex);
                break;

            case GLFW_KEY_E:
                if(!voronoiPatternIsComputed)
                    mesh->updateVoronoiPoint(Vector3<float>(0.0f, stepSize, 0.0f), currentVoronoiIndex);
                break;

            case GLFW_KEY_Q:
                if(!voronoiPatternIsComputed)
                    mesh->updateVoronoiPoint(Vector3<float>(0.0f, -stepSize, 0.0f), currentVoronoiIndex);
                break;

            case GLFW_KEY_ENTER:
                if(currentNumberOfVoronoiPoints >= 2) {
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));
                    mesh->computeVoronoiPattern();
                    scene->splitMesh(dynamic_cast<HalfEdgeMesh*>(mesh));
                    voronoiPatternIsComputed = true;
                } else {
                    std::cout << "\nError!\n";
                    std::cout << "Create atleast 2 voronoi points in order to compute the pattern!\n\n";
                }
                break;

            case GLFW_KEY_UP:
                if(!voronoiPatternIsComputed && currentVoronoiIndex == maximumVoronoiPoints - 1) {
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));
                    currentVoronoiIndex = 0;
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));
                } else {
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));
                    currentVoronoiIndex++;
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));
                }
                break;

            case GLFW_KEY_DOWN:
                if(!voronoiPatternIsComputed && currentVoronoiIndex == 0) {
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));
                    currentVoronoiIndex = maximumVoronoiPoints - 1;
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));
                } else {
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));
                    currentVoronoiIndex--;
                    mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));
                }
                break;

            case GLFW_KEY_C:
                    if(!voronoiPatternIsComputed && currentNumberOfVoronoiPoints < maximumVoronoiPoints) {
                        currentNumberOfVoronoiPoints++;
                        mesh->addVoronoiPoint(Vector3<float>(0.0f, 0.0f, 0.0f));
                        currentVoronoiIndex = currentNumberOfVoronoiPoints-1;
                        mesh->markCurrentVoronoiPoint(currentVoronoiIndex-1, Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));
                        mesh->markCurrentVoronoiPoint(currentVoronoiIndex, Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));
                    } else {
                        std::cout << "\nMaximum number of points already added!\n";
                    }
                break;

            case GLFW_KEY_P:

                if(!voronoiPatternIsComputed) {

                    voronoiPattern = getVoronoiPattern(mesh->getObjName());
                    
                    if(voronoiPattern.size() == 0) {
                        std::cout << "No pre-defined setup for " << mesh->getObjName() << "!!!" << std::endl;
                        break;
                    }

                    dynamic_cast<HalfEdgeMesh*>(mesh)->deleteLastVoronoiPoint();

                    for(unsigned int i = 0; i < voronoiPattern.size(); i++)
                        mesh->addVoronoiPoint(voronoiPattern[i]);

                    mesh->updateVoronoiPoints();
                    mesh->computeVoronoiPattern();
                    scene->splitMesh(dynamic_cast<HalfEdgeMesh*>(mesh));
                    voronoiPatternIsComputed = true;

                    voronoiPattern.clear();
                    voronoiPattern.shrink_to_fit();
                }
            
                break;

            default:
                break;
        }
    }
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
        frameCount  = 0.0;
        startTime   = glfwGetTime();
    }
 
    // Return the current FPS - doesn't have to be used if you don't want it!
    return fps;
}
