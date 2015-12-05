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

    //initExampleWorld();
}


Physics::~Physics() {
    delete mBroadphase;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mDynamicsWorld;
}

void Physics::initExampleWorld() {
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 0, 0), 1);
    btCollisionShape* fallShape = new btSphereShape(1);

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    mDynamicsWorld->addRigidBody(groundRigidBody);


    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
    fallShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody *fallRigidBody = new btRigidBody(fallRigidBodyCI);
    mDynamicsWorld->addRigidBody(fallRigidBody);

    mRigidBodies.push_back(fallRigidBody);

}

void Physics::addGeometry(Geometry *G, unsigned int type) {

    btCollisionShape* shape;
    btDefaultMotionState* motionState;
    btScalar mass = 0;
    btVector3 inertia(0, 0, 0);

    if(type == DYNAMIC) {
        std::cout << "La till dynamic!" << std::endl;
        shape = new btSphereShape(1);
        mass = 1;
        motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 20, 0)));
        shape->calculateLocalInertia(mass, inertia);

    } else if (type == STATIC) {
        std::cout << "La till static!" << std::endl;
        shape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

        motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -2, 0)));

    }  else {
        std::cout << "Somethign went wrong when adding physical counter part " << std::endl;
    }
    

    btRigidBody::btRigidBodyConstructionInfo shapeRigidBodyCI(mass, motionState, shape, inertia);
    btRigidBody *shapeRigidBody = new btRigidBody(shapeRigidBodyCI);
    
    shapeRigidBody->setRestitution(0.7);

    mDynamicsWorld->addRigidBody(shapeRigidBody);

    mRigidBodies.push_back(shapeRigidBody);

}

void Physics::stepSimulation() {
    mDynamicsWorld->stepSimulation(1/60.f, 10);
    //btTransform trans;
    //mRigidBodies[1]->getMotionState()->getWorldTransform(trans);
   // std::cout << "sphere height: " << trans.getOrigin().getX() << " " <<  trans.getOrigin().getY() << " " << trans.getOrigin().getZ() << std::endl;
}


/*
btTransform trans;
btTransform prevPos;
fallRigidBody->getMotionState()->getWorldTransform(prevPos);

mesh->translate(Vector3<float>(prevPos.getOrigin().getX(), prevPos.getOrigin().getY(), prevPos.getOrigin().getZ()));



dynamicsWorld->stepSimulation(1 / 60.f, 10);


fallRigidBody->getMotionState()->getWorldTransform(trans);

std::cout << "sphere height diff: " << trans.getOrigin().getX() - prevPos.getOrigin().getX() << " " <<  trans.getOrigin().getY()  - prevPos.getOrigin().getY() << " " << trans.getOrigin().getZ() -  - prevPos.getOrigin().getZ() << std::endl;

mesh->translate(Vector3<float>(trans.getOrigin().getX() - prevPos.getOrigin().getX(), trans.getOrigin().getY()  - prevPos.getOrigin().getY(), trans.getOrigin().getZ() -  - prevPos.getOrigin().getZ()));
prevPos = trans;*/