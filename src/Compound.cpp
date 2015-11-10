#include "Compound.h"

Compound::Compound(Boundingbox* boundingBox, std::vector<Vector3 <float> > voronoiPoints) {

    mColor = Vector4<float>(1.0f, 1.0f, 1.0f, 0.5f);
    calculateVoronoiPattern(boundingBox, voronoiPoints);
}


Compound::~Compound() {

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    mVerts.clear();
    mVerts.shrink_to_fit();
}

void Compound::initialize() {
    std::cout << "\nInitializing Compound ...\n";

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

     std::cout << "\nCompount Initialized!\n" << std::endl;
}

void Compound::render(Matrix4x4<float> MVP) {

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
    
    //draw triangles
    glDrawArrays(GL_TRIANGLES, 0, mVerts.size());

    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glDisable( GL_BLEND );
    glEnable( GL_CULL_FACE );

}

void Compound::calculateVoronoiPattern(Boundingbox* boundingBox, std::vector<Vector3<float> > voronoiPoints) {

    for(unsigned int i = 0; i < voronoiPoints.size(); i++) {
        std::vector<Vector3<float> > twoPoints;

        for (unsigned int j = i+1; j < voronoiPoints.size(); j++) {
            twoPoints.push_back(voronoiPoints[i]);
            twoPoints.push_back(voronoiPoints[j]);
            std::cout << std::endl << voronoiPoints[i] << std::endl;
            std::cout << voronoiPoints[j] << std::endl;
            calculateSplittingPlane(boundingBox, twoPoints);
            twoPoints.clear();
            twoPoints.shrink_to_fit();
        }

    }
}

void Compound::calculateSplittingPlane(Boundingbox* boundingBox, std::vector<Vector3<float> > voronoiPoints) {
        
    std::vector<Vector3<float> > boundingValues = boundingBox->getBoundingValues();
  /*  
    for(unsigned int i = 0; i < boundingValues.size(); i++)
        std::cout << "boundingValues " << boundingValues[i] << std::endl;

    for(unsigned int i = 0; i < voronoiPoints.size(); i++)
        std::cout << "voronoiPoints " << voronoiPoints[i] << std::endl;
*/
    Vector3<float> mittPunkt = voronoiPoints[0] + (voronoiPoints[1] - voronoiPoints[0]) / 2.0f;  
    Vector3<float> normal = (voronoiPoints[1] - voronoiPoints[0]).Normalize();

    std::cout << "mittPunkt: " << mittPunkt << std::endl;
    std::cout << "normal: " << normal << std::endl;

    std::vector<Vector3<float> >    xPoints;
    std::vector<Vector3<float> >    yPoints;
    std::vector<Vector3<float> >    zPoints;

    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMIN][1] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[0], boundingValues[YMIN][1], boundingValues[ZMIN][2]));
    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMIN][1] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[0], boundingValues[YMIN][1], boundingValues[ZMAX][2]));
    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMAX][1] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[0], boundingValues[YMAX][1], boundingValues[ZMAX][2]));
    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMAX][1] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[0], boundingValues[YMAX][1], boundingValues[ZMIN][2]));   

    yPoints.push_back(Vector3<float>(boundingValues[XMIN][0], (-normal[0]*boundingValues[XMIN][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[1], boundingValues[ZMIN][2]));
    yPoints.push_back(Vector3<float>(boundingValues[XMIN][0], (-normal[0]*boundingValues[XMIN][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[1], boundingValues[ZMAX][2]));
    yPoints.push_back(Vector3<float>(boundingValues[XMAX][0], (-normal[0]*boundingValues[XMAX][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[1], boundingValues[ZMAX][2]));
    yPoints.push_back(Vector3<float>(boundingValues[XMAX][0], (-normal[0]*boundingValues[XMAX][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*boundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[1], boundingValues[ZMIN][2])); 

    zPoints.push_back(Vector3<float>(boundingValues[XMIN][0], boundingValues[YMIN][1], (-normal[0]*boundingValues[XMIN][0] + normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMIN][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));
    zPoints.push_back(Vector3<float>(boundingValues[XMIN][0], boundingValues[YMAX][1], (-normal[0]*boundingValues[XMIN][0] + normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMAX][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));
    zPoints.push_back(Vector3<float>(boundingValues[XMAX][0], boundingValues[YMAX][1], (-normal[0]*boundingValues[XMAX][0] + normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMAX][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));
    zPoints.push_back(Vector3<float>(boundingValues[XMAX][0], boundingValues[YMIN][1], (-normal[0]*boundingValues[XMAX][0] + normal[0]*mittPunkt[0] - normal[1]*boundingValues[YMIN][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));


  //  xPoints.push_back(Vector3<float>( normal[0]*mi ));

    std::vector<Vector3<float> > okPoints;

    for(unsigned int i = 0; i < xPoints.size(); i++) {
        if(xPoints[i][0] > boundingValues[XMIN][0] && xPoints[i][0] < boundingValues[XMAX][0])
            okPoints.push_back(xPoints[i]);

        if(yPoints[i][1] > boundingValues[YMIN][1] && yPoints[i][1] < boundingValues[YMAX][1])
            okPoints.push_back(yPoints[i]);

        if(zPoints[i][2] > boundingValues[ZMIN][2] && zPoints[i][2] < boundingValues[ZMAX][2])
            okPoints.push_back(zPoints[i]);
    }


    for(unsigned int i = 0; i < okPoints.size(); i++) {
        std::cout << "okPoints: " << okPoints[i] << std::endl;
    }

    std::vector<Vector3<float> > points;

    switch(okPoints.size()) {
        case 3:
            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[1]);
            mVerts.push_back(okPoints[2]);

            
            points.push_back(okPoints[0]);
            points.push_back(okPoints[1]);
            points.push_back(okPoints[2]);

            mSplittingPlanes.push_back(points);

            points.clear();
            points.shrink_to_fit();

            for(unsigned int i = 0; i < mSplittingPlanes[0].size(); i++) {
                std::cout << "asdjhgasj,dkhasdjkhasdkjhaskdj: " << mSplittingPlanes[0][i] << std::endl;
            }

            break;
        case 4:
            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[1]);
            mVerts.push_back(okPoints[2]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[2]);
            mVerts.push_back(okPoints[3]);

            points.push_back(okPoints[0]);
            points.push_back(okPoints[1]);
            points.push_back(okPoints[2]);
            points.push_back(okPoints[3]);

            mSplittingPlanes.push_back(points);

            points.clear();
            points.shrink_to_fit();

            break;
        case 5:
            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[1]);
            mVerts.push_back(okPoints[2]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[2]);
            mVerts.push_back(okPoints[3]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[3]);
            mVerts.push_back(okPoints[4]);

            points.push_back(okPoints[0]);
            points.push_back(okPoints[1]);
            points.push_back(okPoints[2]);
            points.push_back(okPoints[3]);
            points.push_back(okPoints[4]);

            mSplittingPlanes.push_back(points);

            points.clear();
            points.shrink_to_fit();

            break;
        case 6:
            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[2]);
            mVerts.push_back(okPoints[1]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[5]);
            mVerts.push_back(okPoints[2]);

            mVerts.push_back(okPoints[5]);
            mVerts.push_back(okPoints[3]);
            mVerts.push_back(okPoints[2]);

            mVerts.push_back(okPoints[5]);
            mVerts.push_back(okPoints[4]);
            mVerts.push_back(okPoints[3]);

            points.push_back(okPoints[0]);
            points.push_back(okPoints[1]);
            points.push_back(okPoints[2]);
            points.push_back(okPoints[3]);
            points.push_back(okPoints[4]);
            points.push_back(okPoints[5]);

            mSplittingPlanes.push_back(points);

            points.clear();
            points.shrink_to_fit();

            break;
        default:
            break;
    }
}


bool Compound::calculateLineIntersectionPoint(
    std::pair<Vector3<float>, Vector3<float> > edge1, 
    std::pair<Vector3<float>, Vector3<float> > edge2, 
    Vector3<float> &intersectionPoint) {

    return true;
}





