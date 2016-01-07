#include "GLDebugDrawer.h"

GLDebugDrawer::GLDebugDrawer()
:m_debugMode(0) {
   
   std::cout << "aconstruct " << std::endl;
   mColor = Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f);
   initialize();
}

GLDebugDrawer::~GLDebugDrawer() {
   glDeleteBuffers(1, &vertexBuffer);
   glDeleteVertexArrays(1, &vertexArrayID);
   glDeleteProgram(shaderProgram);

   mVerts.clear();
   mVerts.shrink_to_fit();
}

void GLDebugDrawer::initialize() {

   std::cout << "\nInitializing GLDebugDrawer ...\n";
   mVerts.resize(2);
   debug


   glGenVertexArrays(1, &vertexArrayID);
   debug
   glBindVertexArray(vertexArrayID);
   debug
   shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );
   debug
    // Set names for our uniforms, same as in shaders
   MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
   ColorLoc = glGetUniformLocation(shaderProgram, "color");
   debug
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
   debug
   std::cout << "\nGLDebugDrawer initialized!\n" << std::endl;

}

void GLDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color) {
   
   render(from, to);
}

void GLDebugDrawer::render(const btVector3& from,const btVector3& to) {

   mVerts[0] = Vector3<float>(from.getX(), from.getY(), from.getZ());
   mVerts[1] = Vector3<float>(to.getX(), to.getY(), to.getZ());
   glDisable( GL_CULL_FACE );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   // Use shader
   glUseProgram(shaderProgram);

   // Pass values of our matrices and materials to the GPU via uniforms
   glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &mMVP(0, 0));
   glUniform4f(ColorLoc, mColor[0], mColor[1], mColor[2], mColor[3]);

   // Rebind the buffer data, vertices are now updated
   glBindVertexArray(vertexArrayID);
   glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
   glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

   // Draw geometry    
   glDrawArrays(GL_LINES, 0, mVerts.size()); // 3 indices starting at 0 -> 1 triangle

   // Unbind
   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glDisableVertexAttribArray(0);

   glDisable( GL_BLEND );
   glEnable( GL_CULL_FACE );

   mVerts.clear();
   mVerts.resize(2);
}

void GLDebugDrawer::drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& color, btScalar /*alpha*/) {
   std::cout << "i draw triangle" << std::endl;
   drawLine(v0,v1,color);
   drawLine(v1,v2,color);
   drawLine(v2,v0,color);   
}

void GLDebugDrawer::setDebugMode(int debugMode) {
   
   m_debugMode = debugMode;
}

void GLDebugDrawer::draw3dText(const btVector3& location,const char* textString) {
   //glRasterPos3f(location.x(),  location.y(),  location.z());
   //BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void GLDebugDrawer::reportErrorWarning(const char* warningString) {
   
   std::cout << warningString << std::endl;
}

void GLDebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {
   {
      btVector3 to=pointOnB+normalOnB*distance;
      const btVector3&from = pointOnB;
      glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);   

      GLDebugDrawer::drawLine(from, to, color);
   }
}  