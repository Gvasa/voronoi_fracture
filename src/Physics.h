#ifndef PHYSICS_H
#define PHYSICS_H

#include <map>
#include <iomanip>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "tools/shader.hpp"
#include "utils/Utils.h"
//#include "OpenGLMotionState.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "Geometry.h"
#include "utils/GLDebugDrawer.h"


class Physics {

public:

    Physics();
    
    ~Physics(); 
    
    void addGeometry(std::vector<Vector3<float> >, Vector3<float>, unsigned int);
    void stepSimulation(Matrix4x4<float>);
    void setWireFrame(bool w) { mWireframe = w; };
    void removeGeometry(unsigned int);
    btRigidBody* getRigidBodyAt(int i) { return mRigidBodies[i]; }


private:

    // Member functions

    void calculateBoundingbox(std::vector<Vector3<float> >);
    
    // Shader data

    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;
    GLint MVPLoc; // MVP Matrix
    GLint ColorLoc;
    
    // Member variables

    Vector4<float> mColor;

    std::vector<Vector3<float> > mVerts;

    bool mWireframe = false;

    btBroadphaseInterface* mBroadphase;
    btDefaultCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mDispatcher;
    btSequentialImpulseConstraintSolver* mSolver;
    btDiscreteDynamicsWorld* mDynamicsWorld;


    std::vector<btRigidBody* > mRigidBodies;

    GLDebugDrawer mDebugDrawer;

    double prevTime;



};

#endif // PHYSICS_H