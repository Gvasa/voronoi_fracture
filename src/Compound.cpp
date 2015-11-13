#include "Compound.h"

Compound::Compound(Boundingbox* boundingBox, std::vector<Vector3 <float> > voronoiPoints) {

    mColor = Vector4<float>(1.0f, 1.0f, 1.0f, 0.5f);

    mColorScale.push_back(Vector3<float>(166.0f/255.0f,206.0f/255.0f,227.0f/255.0f));
    mColorScale.push_back(Vector3<float>(31.0f/255.0f,120.0f/255.0f,180.0f/255.0f));
    mColorScale.push_back(Vector3<float>(178.0f/255.0f,223.0f/255.0f,138.0f/255.0f));
    mColorScale.push_back(Vector3<float>(51.0f/255.0f,160.0f/255.0f,44.0f/255.0f));
    mColorScale.push_back(Vector3<float>(251.0f/255.0f,154.0f/255.0f,153.0f/255.0f));
    mColorScale.push_back(Vector3<float>(227.0f/255.0f,26.0f/255.0f,28.0f/255.0f));
    mColorScale.push_back(Vector3<float>(253.0f/255.0f,191.0f/255.0f,111.0f/255.0f));
    mColorScale.push_back(Vector3<float>(255.0f/255.0f,127.0f/255.0f,0.0f/255.0f));
    mColorScale.push_back(Vector3<float>(202.0f/255.0f,178.0f/255.0f,214.0f/255.0f));
    mColorScale.push_back(Vector3<float>(106.0f/255.0f,61.0f/255.0f,154.0f/255.0f));
    mColorScale.push_back(Vector3<float>(255.0f/255.0f,255.0f/255.0f,153.0f/255.0f));
    mColorScale.push_back(Vector3<float>(177.0f/255.0f,89.0f/255.0f,40.0f/255.0f));

    calculateVoronoiPattern(boundingBox, voronoiPoints);    
}

Compound::~Compound() {

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    mVerts.clear();
    mVerts.shrink_to_fit();

    mColors.clear();
    mColors.shrink_to_fit();
}

void Compound::initialize() {
    std::cout << "\n-----------Initializing Compound ...----------\n";

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders( "shaders/ColorDebugVertex.glsl", "shaders/ColorDebugFragment.glsl" );

    // Set names for our uniforms, same as in shaders
    MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
    //ColorLoc = glGetUniformLocation(shaderProgram, "color");

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

    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(Vector3<float>), &mColors[0], GL_STATIC_DRAW);
    // 2nd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                          // attribute 1. I.e. layout 1 in shader
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
    //glUniform4f(ColorLoc, mColor[0], mColor[1], mColor[2], mColor[3]);

    // Rebind the buffer data, vertices are now updated
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(Vector3<float>), &mColors[0], GL_STATIC_DRAW);

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

    unsigned int planeCounter = 0;

    for(unsigned int i = 0; i < voronoiPoints.size(); i++) {
        std::vector<Vector3<float> > twoPoints;

        for (unsigned int j = i+1; j < voronoiPoints.size(); j++) {
            twoPoints.push_back(voronoiPoints[i]);
            twoPoints.push_back(voronoiPoints[j]);
            std::cout << std::endl << voronoiPoints[i] << std::endl;
            std::cout << voronoiPoints[j] << std::endl;
            calculateSplittingPlane(boundingBox, twoPoints, planeCounter);
            planeCounter++;
            twoPoints.clear();
            twoPoints.shrink_to_fit();
        }

    }

    std::vector<std::pair<Vector3<float>, Vector3<float> > > intersectionPoints;
    
    for(unsigned int i = 0; i < mSplittingPlanes.size(); i++) {
        for (unsigned int j = i + 1; j < mSplittingPlanes.size(); j++) {

            std::pair<Vector3<float>, Vector3<float> > intersectionPointsPair;
            if(calculatePlaneIntersection(mSplittingPlanes[i], mSplittingPlanes[j], intersectionPointsPair)) {

                mDebugpoints.push_back(new Debugpoint(intersectionPointsPair.first));
                mDebugpoints.push_back(new Debugpoint(intersectionPointsPair.second));
            }
        }
    }
}

void Compound::calculateSplittingPlane(Boundingbox* boundingBox, std::vector<Vector3<float> > voronoiPoints, unsigned int planeIndex) {
        
    mBoundingValues = boundingBox->getBoundingValues();

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

    std::vector<Vector3<float> > okPoints;

    for(unsigned int i = 0; i < xPoints.size(); i++) {
        if(xPoints[i][0] > mBoundingValues[XMIN][0] && xPoints[i][0] < mBoundingValues[XMAX][0])
            okPoints.push_back(xPoints[i]);

        if(yPoints[i][1] > mBoundingValues[YMIN][1] && yPoints[i][1] < mBoundingValues[YMAX][1])
            okPoints.push_back(yPoints[i]);

        if(zPoints[i][2] > mBoundingValues[ZMIN][2] && zPoints[i][2] < mBoundingValues[ZMAX][2])
            okPoints.push_back(zPoints[i]);
    }

    okPoints = sortVertices(okPoints, normal);

    std::vector<Vector3<float> > points;

    switch(okPoints.size()) {
        case 3:
            mVerts.push_back(okPoints[0]);
            mVerts.push_back(okPoints[1]);
            mVerts.push_back(okPoints[2]);

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            
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

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

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

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

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

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);
            mColors.push_back(mColorScale[planeIndex]);

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

bool Compound::calculatePlaneIntersection( std::vector<Vector3<float> > plane1, std::vector<Vector3<float> > plane2, std::pair<Vector3<float> , Vector3<float> > &intersectionPair) {
   
    std::vector<Vector3<float> > iPoints;
    Vector3<float> tempPoint;

    // Iterate over all lines in the first plane
    for(unsigned int i = 0; i < plane1.size(); i++) {
        unsigned int index = 0, i1 = 0, i2 = 0; //i1 and i2 used as indices in make_pair in second for loop

        // Check which points we want to evaluate, i.e. which edge        
        Vector3<float> difference;
        if(i == plane1.size()-1){
            difference = plane1[i] - plane1[0];
            i1 = i; i2 = 0;
        } else{
            difference = plane1[i] - plane1[i+1];
            i1 = i; i2 = i+1;
        }

        // Check which plane on the boundingbox that we are evaluating
        if((difference[0] < EPSILON && difference[0] > -EPSILON) && (plane1[i][0] < mBoundingValues[XMIN][0] + EPSILON || plane1[i][0] > mBoundingValues[XMAX][0] - EPSILON))
            index = 0;
        else if((difference[1] < EPSILON && difference[1] > -EPSILON) && (plane1[i][1] < mBoundingValues[YMIN][1] + EPSILON || plane1[i][1] > mBoundingValues[YMAX][1] - EPSILON))
            index = 1;
        else
            index = 2;

        // Iterate over all lines in the second plane
        for(unsigned int j = 0; j < plane2.size(); j++) {

            // Are the lines in the same plane?
            if(plane1[i][index] - plane2[j][index] < EPSILON && plane1[i][index] - plane2[j][index] > -EPSILON ) {

                std::pair<Vector3<float>, Vector3<float> > pair1;
                std::pair<Vector3<float>, Vector3<float> > pair2;

                // We have a match, so we add the first edge
                pair1 = std::make_pair(plane1[i1], plane1[i2]);

                // Resolve the case when we want to check size-1 and 0
                if(j == 0){
                    if(plane2[0][index] - plane2[1][index] < EPSILON && plane2[0][index] - plane2[1][index] > -EPSILON)
                        pair2 = std::make_pair(plane2[0], plane2[1]);
                    else
                        pair2 = std::make_pair(plane2[0], plane2[plane2.size()-1]);
                } else {
                    pair2 = std::make_pair(plane2[j], plane2[j+1]);        
                }
                
                // Check if an intersection is found between the two edges
                if(calculateLineIntersectionPoint(pair1, pair2, tempPoint)) {
                    iPoints.push_back(tempPoint);
                    break;
                }
            }  
        }
    }

    // We want two intersection points per pair of planes, so that they create an edge
    if(iPoints.size() == 2) {
        intersectionPair = std::make_pair(iPoints[0], iPoints[1]);
        return true;
    } 

    return false;
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

    if (s >= -EPSILON && s <= 1.0f + EPSILON) {
        std::cout << "S: " << s << std::endl;
        debug
        intersectionPoint = edge1.first + dv1.EntryMult(Vector3<float>(s,s,s));

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



