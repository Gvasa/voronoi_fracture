#include "Physics.h"

Physics::Physics() {
    mColor = Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f);
    
    mBroadphase = new btDbvtBroadphase();

    // Set up the collision configuration and dispatcher
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

    // The actual physics solver
    mSolver = new btSequentialImpulseConstraintSolver;

    // The world.
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
    mDynamicsWorld->setGravity(btVector3(0, -9.8, 0));

    debugDrawer.setDebugMode(1);
    mDynamicsWorld->setDebugDrawer(&debugDrawer);

    //initExampleWorld();
}


Physics::~Physics() {
    delete mBroadphase;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mDynamicsWorld;
}

void Physics::addGeometry(std::vector<Vector3<float> > vertList, unsigned int type) {

    btCollisionShape* shape;
    btDefaultMotionState* motionState;
    btScalar mass = 0;
    btVector3 inertia(0, 0, 0);

    if(type == DYNAMIC) {
        std::cout << "La till dynamic!" << std::endl;

        btConvexHullShape *bConvex = new btConvexHullShape();
        for(int i = 0; i < vertList.size(); ++i)
              bConvex->addPoint(btVector3(vertList[i][0],vertList[i][1],vertList[i][2]), false);

        bConvex->recalcLocalAabb();
        
        bConvex->setMargin(0.001);//padding
        btShapeHull* hull = new btShapeHull(bConvex);
        btScalar margin = bConvex->getMargin();
        hull->buildHull(margin);
        shape = new btConvexHullShape((const btScalar*)hull->getVertexPointer(), hull->numVertices(), sizeof(btVector3));;// new btSphereShape(1);
       
        mass = 5;
        motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 5, 0)));
        shape->calculateLocalInertia(mass, inertia);
    } else if (type == STATIC) {
        std::cout << "La till static!" << std::endl;
        shape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

        motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -4, 0)));

    }  else {
        std::cout << "Somethign went wrong when adding physical counter part " << std::endl;
    }
    

    btRigidBody::btRigidBodyConstructionInfo shapeRigidBodyCI(mass, motionState, shape, inertia);
    btRigidBody *shapeRigidBody = new btRigidBody(shapeRigidBodyCI);
    
    shapeRigidBody->setRestitution(0.2);   

    mDynamicsWorld->addRigidBody(shapeRigidBody);
    mRigidBodies.push_back(shapeRigidBody);

}

void Physics::stepSimulation() {
    mDynamicsWorld->stepSimulation(1/60.f, 10);
    mDynamicsWorld->debugDrawWorld(); 

}
