
#ifndef GL_DEBUG_DRAWER_H
#define GL_DEBUG_DRAWER_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"
#include "../tools/shader.hpp"
#include "LinearMath/btIDebugDraw.h"
#include "Utils.h"

class GLDebugDrawer : public btIDebugDraw
{
    int m_debugMode;
  
public:
  
    GLDebugDrawer();
    
    ~GLDebugDrawer();

    virtual void   drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
  
    virtual void   drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    
    //virtual void   drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& /*n0*/,const btVector3& /*n1*/,const btVector3& /*n2*/,const btVector3& color, btScalar alpha);
    
    virtual void   drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& color, btScalar /*alpha*/);
       
  
    virtual void   reportErrorWarning(const char* warningString);
  
    virtual void   draw3dText(const btVector3& location,const char* textString);
  
    virtual void   setDebugMode(int debugMode);
  
    virtual int    getDebugMode() const { return m_debugMode;}

    void initialize();
    void render(const btVector3&, const btVector3&);

    void setMVP(Matrix4x4<float> m) { mMVP = m; }

private:
	 // Shader data

    GLuint vertexArrayID;
    
    GLuint vertexBuffer;
    
    GLuint shaderProgram;

    GLint MVPLoc; // MVP Matrix
    
    GLint ColorLoc;

    std::vector<Vector3<float> > mVerts;

    Vector4<float> mColor;

    Matrix4x4<float> mMVP;
      
};
#endif