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

    for(unsigned int i = 0; i < mDebugpoints.size(); i++)
        mDebugpoints[i]->initialize(Vector3<float>(0.0f, 0.0f, 0.0f));

    std::cout << "\nCompount Initialized!\n" << std::endl;
}

void Compound::render(Matrix4x4<float> MVP) {

    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
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
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    std::vector<Matrix4x4<float> > tmp;
    tmp.push_back(MVP);

    for(unsigned int i = 0; i < mDebugpoints.size(); i++)
        mDebugpoints[i]->render(tmp);

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


    for(unsigned int i = 0; i < mSplittingPlanes.size(); i++) {
        
        Vector3<float> difference = mSplittingPlanes[i][0] - mSplittingPlanes[i][1];

        unsigned int index;

        if(difference[0] < EPSILON) 
            index = 0;
        else if(difference[1] < EPSILON)
            index = 1;
        else
            index = 2;

        for(unsigned int j = i + 1; j < mSplittingPlanes.size(); j++) {
            
            if(mSplittingPlanes[i][0][index] - mSplittingPlanes[j][0][index] < EPSILON) {

                unsigned int s = mSplittingPlanes[j].size();
                std::pair<Vector3<float>, Vector3<float> > p1;
                std::pair<Vector3<float>, Vector3<float> > p2;

                p1 = std::make_pair(mSplittingPlanes[i][0], mSplittingPlanes[i][1]);

                if(mSplittingPlanes[j][1][index] - mSplittingPlanes[j][0][index] < EPSILON) {
                    p2 = std::make_pair(mSplittingPlanes[j][0], mSplittingPlanes[j][1]);
                    /* 
                    1. Beräkna skärningspunkt
                    */
                } else {
                    p2 = std::make_pair(mSplittingPlanes[j][s-1], mSplittingPlanes[j][0]);
                    /* 
                    1. Beräkna skärningspunkt
                    */
                }

                // Om skärningspunkter finns, kapa planen som skär varandra med nya skärningspunkter

            }

        }
    }


   /* std::pair<Vector3<float>, Vector3<float> > e1;
    std::pair<Vector3<float>, Vector3<float> > e2;
    std::pair<Vector3<float>, Vector3<float> > e3;
    std::pair<Vector3<float>, Vector3<float> > e4;

    e1 = std::make_pair(Vector3<float>(-0.99f, 0.0f, 0.99f), Vector3<float>(0.99f, 0.0f, 0.99f));
    e2 = std::make_pair(Vector3<float>(0.0f, -0.99f, 0.99f), Vector3<float>(0.0f, 0.99f, 0.99f));

    e3 = std::make_pair(Vector3<float>(-0.99f, 0.0f, -0.99f), Vector3<float>(0.99f, 0.0f, -0.99f));
    e4 = std::make_pair(Vector3<float>(0.0f, -0.99f, -0.99f), Vector3<float>(0.0f, 0.99f, -0.99f));

    Vector3<float> v1;
    Vector3<float> v2;

    calculateLineIntersectionPoint(e1, e2, v1);
    calculateLineIntersectionPoint(e3, e4, v2);

    std::cout << "\nintersection point: " << v1 << std::endl << std::endl;
    std::cout << "\nintersection point: " << v2 << std::endl << std::endl;*/
}

void Compound::calculateSplittingPlane(Boundingbox* boundingBox, std::vector<Vector3<float> > voronoiPoints) {
        
    mBoundingValues = boundingBox->getBoundingValues();
  /*  
    for(unsigned int i = 0; i < mBoundingValues.size(); i++)
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

    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMIN][1] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[0], mBoundingValues[YMIN][1], mBoundingValues[ZMIN][2]));
    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMIN][1] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[0], mBoundingValues[YMIN][1], mBoundingValues[ZMAX][2]));
    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMAX][1] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[0], mBoundingValues[YMAX][1], mBoundingValues[ZMAX][2]));
    xPoints.push_back(Vector3<float>((normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMAX][1] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[0], mBoundingValues[YMAX][1], mBoundingValues[ZMIN][2]));   

    yPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], (-normal[0]*mBoundingValues[XMIN][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[1], mBoundingValues[ZMIN][2]));
    yPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], (-normal[0]*mBoundingValues[XMIN][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[1], mBoundingValues[ZMAX][2]));
    yPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], (-normal[0]*mBoundingValues[XMAX][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMAX][2] + normal[2]*mittPunkt[2]) / normal[1], mBoundingValues[ZMAX][2]));
    yPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], (-normal[0]*mBoundingValues[XMAX][0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*mBoundingValues[ZMIN][2] + normal[2]*mittPunkt[2]) / normal[1], mBoundingValues[ZMIN][2])); 

    zPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], mBoundingValues[YMIN][1], (-normal[0]*mBoundingValues[XMIN][0] + normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMIN][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));
    zPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], mBoundingValues[YMAX][1], (-normal[0]*mBoundingValues[XMIN][0] + normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMAX][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));
    zPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], mBoundingValues[YMAX][1], (-normal[0]*mBoundingValues[XMAX][0] + normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMAX][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));
    zPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], mBoundingValues[YMIN][1], (-normal[0]*mBoundingValues[XMAX][0] + normal[0]*mittPunkt[0] - normal[1]*mBoundingValues[YMIN][1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2]));


  //  xPoints.push_back(Vector3<float>( normal[0]*mi ));

    std::vector<Vector3<float> > okPoints;

    for(unsigned int i = 0; i < xPoints.size(); i++) {
        if(xPoints[i][0] > mBoundingValues[XMIN][0] && xPoints[i][0] < mBoundingValues[XMAX][0])
            okPoints.push_back(xPoints[i]);

        if(yPoints[i][1] > mBoundingValues[YMIN][1] && yPoints[i][1] < mBoundingValues[YMAX][1])
            okPoints.push_back(yPoints[i]);

        if(zPoints[i][2] > mBoundingValues[ZMIN][2] && zPoints[i][2] < mBoundingValues[ZMAX][2])
            okPoints.push_back(zPoints[i]);
    }

    for(unsigned int i = 0; i < okPoints.size(); i++) {
        std::cout << "okPoints: " << okPoints[i] << std::endl;
    }

    okPoints = sortVertices(okPoints, normal);

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
            mVerts.push_back(okPoints[1]);
            mVerts.push_back(okPoints[2]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[2]);
            mVerts.push_back(okPoints[3]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[3]);
            mVerts.push_back(okPoints[4]);

            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[4]);
            mVerts.push_back(okPoints[5]);

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


bool Compound::calculateLineIntersectionPoint(  std::pair<Vector3<float>, Vector3<float> > edge1, 
                                                std::pair<Vector3<float>, Vector3<float> > edge2, 
                                                Vector3<float> &intersectionPoint) {

    Vector3<float> dv1 = edge1.second - edge1.first;
    Vector3<float> dv2 = edge2.second - edge2.first;
    Vector3<float> dv3 = edge2.first  - edge1.first;

    float angle = dv3 * Cross(dv1, dv2);

    if(angle > EPSILON || angle < -EPSILON)
        return false;
 
    float s = (Cross(dv3, dv2) * Cross(dv1, dv2)) / (Cross(dv1, dv2).Length() * Cross(dv1, dv2).Length());

    if (s >= 0.0f && s <= 1.0f) {

        intersectionPoint = edge1.first + dv1.EntryMult(Vector3<float>(s,s,s));

        mDebugpoints.push_back(new Debugpoint(intersectionPoint));

        return true;
    }

    return false;
}

std::vector<Vector3<float> > Compound::sortVertices(std::vector<Vector3<float> > plane, Vector3<float> normal) {

    Vector3<float> centerPoint = Vector3<float>(0.0f, 0.0f, 0.0f);

    for(unsigned int i = 0; i < plane.size(); i++) {
        centerPoint += plane[i];
    }

    centerPoint /= plane.size();

    // Projected coordinates
    unsigned int s, t;

    // Which plane do we want to project the polygon onto?
    if(normal * Vector3<float>(1.0f, 0.0f, 0.0f) > EPSILON || normal * Vector3<float>(1.0f, 0.0f, 0.0f) < -EPSILON ) {
        s = 1;
        t = 2;
    } else if (normal * Vector3<float>(0.0f, 1.0f, 0.0f) > EPSILON || normal * Vector3<float>(0.0f, 1.0f, 0.0f) < -EPSILON ) {
        s = 0;
        t = 2;
    } else {
        s = 0;
        t = 1;
    }


    std::vector<std::pair<float, Vector3<float> > > vertices;

    for(unsigned int i = 0; i < plane.size(); i++) {
        
        Vector3<float> v = plane[i] - centerPoint;

        float angle = atan2(v[s], v[t]);

        vertices.push_back(std::make_pair(angle, plane[i]));
    }

    std::sort(
        vertices.begin(), 
        vertices.end(), 
        [](const std::pair<float, Vector3<float> > p1, const std::pair<float, Vector3<float> > p2) { 
            return p1.first < p2.first; 
        } );

    std::vector<Vector3<float>> sortedVertices;

    for(unsigned int i = 0; i < vertices.size(); i++) {
        sortedVertices.push_back(vertices[i].second);
    }

    return sortedVertices;
}



