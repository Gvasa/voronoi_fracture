#include "Boundingbox.h"

Boundingbox::Boundingbox(std::vector<Vector3<float> > uniqueVerts) {
    mColor = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    calculateBoundingbox(uniqueVerts);
}


Boundingbox::~Boundingbox() {
    
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    mVerts.clear();
    mVerts.shrink_to_fit();

    mBoundingValues.clear();
    mBoundingValues.shrink_to_fit();
}


void Boundingbox::initialize() {

    std::cout << "\nInitializing Boundingbox ...\n";

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );

    // Set names for our uniforms, same as in shaders
    MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
    ColorLoc = glGetUniformLocation(shaderProgram, "color");

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

     std::cout << "\nBoundingbox initialized!\n" << std::endl;
}


void Boundingbox::render(Matrix4x4<float> MVP) {

    glDisable( GL_CULL_FACE );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

     // Use shader
    glUseProgram(shaderProgram);

    // Pass values of our matrices and materials to the GPU via uniforms
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVP(0, 0));
    glUniform4f(ColorLoc, mColor[0], mColor[1], mColor[2], mColor[3]);

    // Rebind the buffer data, vertices are now updated
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

    // Draw geometry    
    if(mWireframe)   //Draw Wireframe
        glDrawArrays(GL_LINES, 0, mVerts.size()); // 3 indices starting at 0 -> 1 triangle
    else                //draw triangles
        glDrawArrays(GL_TRIANGLES, 0, mVerts.size());

    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glDisable( GL_BLEND );
    glEnable( GL_CULL_FACE );
}


void Boundingbox::calculateBoundingbox(std::vector<Vector3<float> > uniqueVerts) {

    std::map<std::string, float> boundingValuesIndex;

    boundingValuesIndex["xMax"] = -(std::numeric_limits<float>::max)();
    boundingValuesIndex["yMax"] = -(std::numeric_limits<float>::max)();
    boundingValuesIndex["zMax"] = -(std::numeric_limits<float>::max)();
    boundingValuesIndex["xMin"] = (std::numeric_limits<float>::max)();
    boundingValuesIndex["yMin"] = (std::numeric_limits<float>::max)();
    boundingValuesIndex["zMin"] = (std::numeric_limits<float>::max)();

    for(unsigned int i = 0; i < uniqueVerts.size(); i++) {

        if(uniqueVerts[i][0] < boundingValuesIndex["xMin"])
            boundingValuesIndex["xMin"] = uniqueVerts[i][0];

        if(uniqueVerts[i][0] > boundingValuesIndex["xMax"])
            boundingValuesIndex["xMax"] = uniqueVerts[i][0];

        if(uniqueVerts[i][1] < boundingValuesIndex["yMin"])
            boundingValuesIndex["yMin"] = uniqueVerts[i][1];

        if(uniqueVerts[i][1] > boundingValuesIndex["yMax"])
            boundingValuesIndex["yMax"] = uniqueVerts[i][1];

        if(uniqueVerts[i][2] < boundingValuesIndex["zMin"])
            boundingValuesIndex["zMin"] = uniqueVerts[i][2];

        if(uniqueVerts[i][2] > boundingValuesIndex["zMax"])
            boundingValuesIndex["zMax"] = uniqueVerts[i][2];
    }


    std::cout << "\nMAX - MIN VALUES: " << std::endl;
    std::cout << "xMin: " << boundingValuesIndex["xMin"] << std::endl;
    std::cout << "xMax: " << boundingValuesIndex["xMax"] << std::endl;
    std::cout << "yMin: " << boundingValuesIndex["yMin"] << std::endl;
    std::cout << "yMax: " << boundingValuesIndex["yMax"] << std::endl;
    std::cout << "zMin: " << boundingValuesIndex["zMin"] << std::endl;
    std::cout << "zMax: " << boundingValuesIndex["zMax"] << std::endl;

    mBoundingValues.resize(8);

    mBoundingValues[XPYPZP] = Vector3<float>(boundingValuesIndex["xMax"], boundingValuesIndex["yMax"], boundingValuesIndex["zMax"]);
    mBoundingValues[XPYPZN] = Vector3<float>(boundingValuesIndex["xMax"], boundingValuesIndex["yMax"], boundingValuesIndex["zMin"]);
    mBoundingValues[XNYPZN] = Vector3<float>(boundingValuesIndex["xMin"], boundingValuesIndex["yMax"], boundingValuesIndex["zMin"]);
    mBoundingValues[XNYPZP] = Vector3<float>(boundingValuesIndex["xMin"], boundingValuesIndex["yMax"], boundingValuesIndex["zMax"]);

    mBoundingValues[XPYNZP] = Vector3<float>(boundingValuesIndex["xMax"], boundingValuesIndex["yMin"], boundingValuesIndex["zMax"]);
    mBoundingValues[XPYNZN] = Vector3<float>(boundingValuesIndex["xMax"], boundingValuesIndex["yMin"], boundingValuesIndex["zMin"]);
    mBoundingValues[XNYNZN] = Vector3<float>(boundingValuesIndex["xMin"], boundingValuesIndex["yMin"], boundingValuesIndex["zMin"]);
    mBoundingValues[XNYNZP] = Vector3<float>(boundingValuesIndex["xMin"], boundingValuesIndex["yMin"], boundingValuesIndex["zMax"]);


    /*std::cout << "\nBOUNDING BOX: " << std::endl;
    for(unsigned int i = 0; i < 8; i++)
        std::cout << mBoundingValues[i] << std::endl;*/

    updateRenderData();
}


void Boundingbox::updateBoundingBox(glm::mat4 m) {
    
    glm::vec4 tmpPos;
    
    for(unsigned int i = 0; i < mBoundingValues.size(); i++) {
        tmpPos = glm::vec4(mBoundingValues[i][0], mBoundingValues[i][1], mBoundingValues[i][2], 1.0f);

        tmpPos = m*tmpPos;

        mBoundingValues[i] = Vector3<float>(tmpPos.x, tmpPos.y, tmpPos.z) / 2.0f;
        // Apply the rotation to the vertices
    }

    /*std::cout << "\nUPDATED BOUNDING BOX: " << std::endl;
    for(unsigned int i = 0; i < 8; i++)
        std::cout << mBoundingValues[i] << std::endl;*/
}


void Boundingbox::updateRenderData() {

    mVerts.clear();
    mVerts.shrink_to_fit();

    mVerts.push_back(mBoundingValues[XNYNZN]);
    mVerts.push_back(mBoundingValues[XNYPZP]);
    mVerts.push_back(mBoundingValues[XNYPZN]);

    mVerts.push_back(mBoundingValues[XNYNZN]);
    mVerts.push_back(mBoundingValues[XNYNZP]);
    mVerts.push_back(mBoundingValues[XNYPZP]);

    mVerts.push_back(mBoundingValues[XPYPZN]);
    mVerts.push_back(mBoundingValues[XPYPZP]);
    mVerts.push_back(mBoundingValues[XPYPZN]);

    mVerts.push_back(mBoundingValues[XPYNZN]);
    mVerts.push_back(mBoundingValues[XPYNZP]);
    mVerts.push_back(mBoundingValues[XPYPZP]);

    mVerts.push_back(mBoundingValues[XPYNZN]);
    mVerts.push_back(mBoundingValues[XNYNZP]);
    mVerts.push_back(mBoundingValues[XPYNZP]);

    mVerts.push_back(mBoundingValues[XPYNZN]);
    mVerts.push_back(mBoundingValues[XNYNZN]);
    mVerts.push_back(mBoundingValues[XNYNZP]);

    mVerts.push_back(mBoundingValues[XPYPZN]);
    mVerts.push_back(mBoundingValues[XNYPZP]);
    mVerts.push_back(mBoundingValues[XPYPZP]);

    mVerts.push_back(mBoundingValues[XPYPZN]);
    mVerts.push_back(mBoundingValues[XNYPZN]);
    mVerts.push_back(mBoundingValues[XNYPZP]);

    mVerts.push_back(mBoundingValues[XNYNZN]);
    mVerts.push_back(mBoundingValues[XPYNZN]);
    mVerts.push_back(mBoundingValues[XPYPZN]);

    mVerts.push_back(mBoundingValues[XNYNZN]);
    mVerts.push_back(mBoundingValues[XPYPZN]);
    mVerts.push_back(mBoundingValues[XNYPZN]);

    mVerts.push_back(mBoundingValues[XNYNZP]);
    mVerts.push_back(mBoundingValues[XPYNZP]);
    mVerts.push_back(mBoundingValues[XPYPZP]);

    mVerts.push_back(mBoundingValues[XNYNZP]);
    mVerts.push_back(mBoundingValues[XPYPZP]);
    mVerts.push_back(mBoundingValues[XNYPZP]);
}
