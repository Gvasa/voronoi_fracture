#include "Splittingplane.h"

Splittingplane::Splittingplane(
    std::vector<Vector3<float> > vertices,
    std::vector<Vector3<float> > b, 
    std::pair<Vector3<float>, Vector3<float> > v,
    Vector3<float> n, 
    Vector4<float> c)
    : mUniqueVerts(vertices), mBoundingValues(b), mVoronoiPoints(v), mNormal(n), mColor(c) {

    buildRenderData();
}


Splittingplane::~Splittingplane() {

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    mVerts.clear();
    mVerts.shrink_to_fit();

    mUniqueVerts.clear();
    mUniqueVerts.shrink_to_fit();
}


void Splittingplane::initialize() {

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );

    // Set names for our uniforms, same as in shaders
    MVPLoc   = glGetUniformLocation(shaderProgram, "MVP");
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

    for(unsigned int i = 0; i < mDebugPoints.size(); i++)
        mDebugPoints[i]->initialize(Vector3<float>(0.0f, 0.0f, 0.0f));

    std::cout << "\nCompount Initialized!\n" << std::endl;
}


void Splittingplane::render(Matrix4x4<float> MVP) {

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
    
    //draw triangles
    glDrawArrays(GL_TRIANGLES, 0, mVerts.size());

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    std::vector<Matrix4x4<float> > v;
    v.push_back(MVP);

    for(unsigned int i = 0; i < mDebugPoints.size(); i++)
        mDebugPoints[i]->render(v);
}


void Splittingplane::resolveIntersection(std::pair<Vector3<float>, Vector3<float> > intersectionEdge) {


    for(auto it = mIntersectedPoints.begin(); it != mIntersectedPoints.end(); ++it) {

        float diff1 = ((*it).first - intersectionEdge.first).Length();
        float diff2 = ((*it).second - intersectionEdge.second).Length();

        if((diff1 > -EPSILON && diff1 < EPSILON) && (diff2 > -EPSILON && diff2 < EPSILON)) {
            std::cout << "DU FINNS JU REDAN!" << std::endl;
            return;
        }
    }

    mIntersectedPoints.push_back(intersectionEdge);

    std::vector<Vector3<float> > resolvedVertices;

    resolvedVertices.push_back(intersectionEdge.first);
    resolvedVertices.push_back(intersectionEdge.second);
/*
    for(unsigned int i = 0; i < mUniqueVerts.size(); i++) {

        Vector3<float> intersectionPoint;
        
        if((intersectionEdge.first - mUniqueVerts[i]).Length() < (intersectionEdge.second - mUniqueVerts[i]).Length()) {
            intersectionPoint = intersectionEdge.first;
            std::cout << "-------------  IP = " << intersectionPoint << " --------------" << std::endl;
        } else {
            intersectionPoint = intersectionEdge.second;
            std::cout << "-------------  IP = " << intersectionPoint << " --------------" << std::endl;
        }

        std::cout << "p1: " << mUniqueVerts[i] << "\tp2: " << intersectionPoint << std::endl;

        if(calculatePointLineIntersection(intersectionPoint)) {
            calculateClosestDistance(intersectionPoint, resolvedVertices);
            debug
            break;
        }

        if(calculateLineIntersectionPoint(std::make_pair(mUniqueVerts[i], intersectionPoint))) {
            debug
            resolvedVertices.push_back(mUniqueVerts[i]);
        }
    }
*/    
    std::cout << "VPUNKTERS: " << mVoronoiPoints.first << ", " << mVoronoiPoints.second << std::endl; 
    Vector3<float> difference = mVoronoiPoints.first - mVoronoiPoints.second;
    std::cout << "DIFFERENCE: " << difference << std::endl;

    unsigned int i1 = 0;
    unsigned int i2 = 0;
    unsigned int i3 = 0;

    unsigned int projection; 

    if(fabs(difference[0]) > fabs(difference[1] + EPSILON)) {
        if(fabs(difference[0]) > fabs(difference[2] + EPSILON)) {
            i1 = 2;
            i2 = 1;
            i3 = 0;
            std::cout << "projicerar på X" << std::endl;
            projection = XPROJECTION;
        } else {
            i1 = 0;
            i2 = 1;
            i3 = 2;
            std::cout << "projicerar på Z" << std::endl;
            projection = ZPROJECTION;
        }
    } else {
        i1 = 2;
        i2 = 0;
        i3 = 1;

        std::cout << "projicerar på y" << std::endl;
        //projection = YPROJECTION;
    }

    //float maxDifference std::max(std::max(fabs(difference[0]), fabs(difference[1])), fabs(difference[2]));
    /*   
    if((difference[0] < EPSILON && difference[0] > -EPSILON)) {

        i1 = 2;
        i2 = 1;
        std::cout << "projicerar på X" << std::endl;
        
    } else if((difference[1] < EPSILON && difference[1] > -EPSILON)) {
        
        i1 = 0;
        i2 = 2;
        std::cout << "projicerar på Y" << std::endl;
    } else {

        i1 = 0;
        i2 = 1;
        std::cout << "projicerar på Z" << std::endl;
    }*/

    Vector3<float> mittPunkt = intersectionEdge.first + (intersectionEdge.second - intersectionEdge.first) / 2.0f;  
    std::cout << "interSectionCenter: " << mittPunkt << std::endl;

    mDebugPoints.push_back(new Debugpoint(mittPunkt, Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f)));

   /* for(unsigned int i = 0; i < vertices.size(); i++) {
        
        Vector3<float> v = vertices[i] - centerPoint;

        float angle = atan2(v[s], v[t]);

        unsortedVertices.push_back(std::make_pair(angle, vertices[i]));
    }
*/  
    Vector3<float> vCenter = mVoronoiPoints.first + (mVoronoiPoints.second - mVoronoiPoints.first) / 2.0f;
    
    mDebugPoints.push_back(new Debugpoint(vCenter, Vector4<float>(0.0f, 0.0f, 1.0f, 1.0f)));
    Vector3<float> transVCenter = vCenter - mittPunkt;
    mDebugPoints.push_back(new Debugpoint(transVCenter, Vector4<float>(0.0f, 0.0f, 1.0f, 1.0f)));

    mDebugPoints.push_back(new Debugpoint(Vector3<float> (0.0,0.0,0.0), Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f)));
    //if(projection == YPROJECTION)
      //  vCenter[i2] = -vCenter[i2];

  
    Vector3<float> centerV1 = mVoronoiPoints.first - mittPunkt;
    Vector3<float> centerV2 = mVoronoiPoints.second - mittPunkt;
    float angleV1 = atan2(centerV1[i2], centerV1[i1]);
    float angleV2 = atan2(centerV2[i2], centerV2[i1]);
    float vCenterAngle = atan2(transVCenter[i2], transVCenter[i1]);
    

    std::cout << "----- ANGLE: \t vP: " << angleV1 << " vP2: " << angleV2 << std::endl;
    
   // std::cout << "Vcenter: " << vCenter << " vCenterAngle: " << vCenterAngle << std::endl;


    
    float angle = angleV1;

    if((angleV1 > 0.0f && angleV2 < 0.0f) || (angleV1 < 0.0f && angleV2 > 0.0f)) {

        std::cout << "--- GICK IN I IFSATS ----" << std::endl;

        Vector3<float> v1v2 = mVoronoiPoints.second - mVoronoiPoints.first;
        Vector3<float> i1i2 = intersectionEdge.second - intersectionEdge.first;

        std::cout << "v1: " << mVoronoiPoints.first << "   v2: " << mVoronoiPoints.second << std::endl;
        std::cout << "i1: " << intersectionEdge.first << "   i2: " << intersectionEdge.second << std::endl;
        std::cout << "v1v2: " << v1v2 << std::endl;
        std::cout << "i1i2: " << i1i2 << std::endl;

        if(v1v2[i2] > -EPSILON && i1i2[i1] < EPSILON) {
            i1i2 *= -1;
            std::cout << "v1v2[i2] > 0.0 & i1i2[i1] < 0.0f" << std::endl;
            std::cout << "efter flipp: " << i1i2 << std::endl;
        } else if(v1v2[i2] < EPSILON && i1i2[i1] > -EPSILON) {
            i1i2 *= -1;
            std::cout << "v1v2[i2] < 0.0 & i1i2[i1] > 0.0f" << std::endl;
            std::cout << "efter flipp: " << i1i2 << std::endl;
        }
        v1v2[i3] = 0.0f;
        i1i2[i3] = 0.0f;
        //std::cout << "v1v2 * i1i2: " << v1v2 * i1i2 << std::endl;
        v1v2 = v1v2.Normalize();
        i1i2 = i1i2.Normalize();

        float angleVoronoiIntersect = acos(v1v2 * i1i2);
        std::cout << "v1v2.N: " << v1v2 << ", i1i2.N: " << i1i2 << std::endl;

        std::cout << "angleVoronoiIntersect: " << angleVoronoiIntersect << std::endl;

        if( angleVoronoiIntersect > (M_PI/2.0f) + EPSILON ) {
            angle = angleV1;
            std::cout << "valde vinkel Voronoi1" << std::endl;
            std::cout << "angle: " << angle << std::endl;
        } else {
            angle = angleV2;
            std::cout << "valde vinkel Voronoi2" << std::endl;
            std::cout << "angle: " << angle << std::endl;
        }
    }
    
    for(unsigned int i = 0; i < mUniqueVerts.size(); i++) {
        Vector3<float> center_current_point = mUniqueVerts[i] - mittPunkt; 
        Vector3<float> closest;
        float distance1 = sqrt(pow(mUniqueVerts[i][i1] - intersectionEdge.first[i1],2) + pow(mUniqueVerts[i][i2] - intersectionEdge.first[i2],2));
        float distance2 = sqrt(pow(mUniqueVerts[i][i1] - intersectionEdge.second[i1],2) + pow(mUniqueVerts[i][i2] - intersectionEdge.second[i2],2));
        float uniqueToCenter = sqrt(pow(mUniqueVerts[i][i1] - vCenter[i1],2) + pow(mUniqueVerts[i][i2] - vCenter[i2],2));
        float angle_current_point = atan2(center_current_point[i2], center_current_point[i1]);

        std::cout << "Angle2: \t " << angle_current_point << std::endl;
        std::cout << "---- VERT: \t " << mUniqueVerts[i] << std::endl;
        
        if(angle < 0.0f) {
            if(angle_current_point < 0.0f) {
                std::cout << "la till för mindre: " << std::endl;
                
                if(distance1 < distance2){ 
                    closest = intersectionEdge.first;
                    std::cout << "--first--" << std::endl;
                } else{
                    closest = intersectionEdge.second;
                    std::cout << "--second--" << std::endl;
                }

                std::cout << "Closest: " << closest << ", mUniqueVerts[i]: " << mUniqueVerts[i] << std::endl;
                float closestToCenter = sqrt(pow(closest[i1] - vCenter[i1],2) + pow(closest[i2] - vCenter[i2],2));   
                if(uniqueToCenter < closestToCenter)
                    resolvedVertices.push_back(mUniqueVerts[i]);

            }
        } else {
            if(angle_current_point > 0.0f) {
                std::cout << "la till för större: " << std::endl;

                if(distance1 < distance2){ 
                    closest = intersectionEdge.first;
                    std::cout << "--first--" << std::endl;
                } else{
                    closest = intersectionEdge.second;
                    std::cout << "--second--" << std::endl;
                }    
                std::cout << "Closest: " << closest << ", mUniqueVerts[i]: " << mUniqueVerts[i] << std::endl;
                    
                float closestToCenter = sqrt(pow(closest[i1] - vCenter[i1],2) + pow(closest[i2] - vCenter[i2],2));   
                if(uniqueToCenter < closestToCenter){
                    std::cout << "uniqueToCenter: " << uniqueToCenter << ", closestToCenter: " << closestToCenter << std::endl;
                    resolvedVertices.push_back(mUniqueVerts[i]);
                }

            }
        }

    }

    mUniqueVerts = sortVertices(resolvedVertices);

    for(unsigned int i = 0; i < mUniqueVerts.size(); ++i){
        std::cout << "mUniqueVerts[" << i << "]: " << mUniqueVerts[i] << std::endl;
    }

    buildRenderData();
}


bool Splittingplane::calculateLineIntersectionPoint(std::pair<Vector3<float>, Vector3<float> > edge) {

    Vector3<float> voronoiPoint1 = mVoronoiPoints.first;
    Vector3<float> voronoiPoint2 = mVoronoiPoints.second;

    Vector3<float> difference = edge.second - edge.first;

    std::cout << "voronoi1: " << voronoiPoint1 << "\t voronoi2: " << voronoiPoint2 << std::endl;

    unsigned int i1 = 0;
    unsigned int i2 = 0;
       
    if((difference[0] < EPSILON && difference[0] > -EPSILON) && (edge.first[0] < mBoundingValues[XMIN][0] + EPSILON || edge.first[0] > mBoundingValues[XMAX][0] - EPSILON)) {

        voronoiPoint1[0] = edge.first[0];
        voronoiPoint2[0] = edge.first[0];

        i1 = 1;
        i2 = 2;
        std::cout << "projicerar på X" << std::endl;
        
    } else if((difference[1] < EPSILON && difference[1] > -EPSILON) && (edge.first[1] < mBoundingValues[YMIN][1] + EPSILON || edge.first[1] > mBoundingValues[YMAX][1] - EPSILON)) {
        
        voronoiPoint1[1] = edge.first[1];
        voronoiPoint2[1] = edge.first[1];

        i1 = 0;
        i2 = 2;
        std::cout << "projicerar på Y" << std::endl;

    } else {
        
        voronoiPoint1[2] = edge.first[2];
        voronoiPoint2[2] = edge.first[2];

        i1 = 0;
        i2 = 1;
        std::cout << "projicerar på Z" << std::endl;
    }

    std::cout << "-- projicerade punkter --" << std::endl;
    std::cout << "p1: (" << edge.first[i1] << ", " << edge.first[i2] << ")" << "p2: (" << edge.second[i1] << ", " << edge.second[i2] << ")"  << std::endl;
    std::cout << "p1: (" << voronoiPoint1[i1] << ", " << voronoiPoint1[i2] << ")" << "p2: (" << voronoiPoint2[i1] << ", " << voronoiPoint2[i2] << ")"  << std::endl;


    float x1 = edge.first[i1], x2 = edge.second[i1], x3 = voronoiPoint1[i1], x4 = voronoiPoint2[i1];
    float y1 = edge.first[i2], y2 = edge.second[i2], y3 = voronoiPoint1[i2], y4 = voronoiPoint2[i2];

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if(d < EPSILON && d > -EPSILON)
        return false;

    float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
    float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
    float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

    if((x < std::min(x1, x2) - EPSILON || x > std::max(x1, x2) + EPSILON) || (x < std::min(x3, x4) - EPSILON || x > std::max(x3, x4) + EPSILON))
        return false;
    if((y < std::min(y1, y2) - EPSILON || y > std::max(y1, y2) + EPSILON) || (y < std::min(y3, y4) - EPSILON || y > std::max(y3, y4) + EPSILON))
        return false;

    return true;
    
/*
    Vector3<float> dv1 = edge.second - edge.first;
    Vector3<float> dv2 = voronoiPoint2 - voronoiPoint1;
    Vector3<float> dv3 = voronoiPoint1  - edge.first;

    std::cout << "dv1: " << dv1 << "\tdv2: " << dv2 << "\tdv3: " << dv3 << std::endl;

    float angle = dv3 * Cross(dv1, dv2);

    std::cout << "angle: " << angle << std::endl;

    if(angle > EPSILON || angle < -EPSILON)
        return false;
 
    float s = (Cross(dv3, dv2) * Cross(dv1, dv2)) / (Cross(dv1, dv2).Length() * Cross(dv1, dv2).Length());

    std::cout << "täljare: " << (Cross(dv3, dv2) * Cross(dv1, dv2)) << "\tnämnare: " << (Cross(dv1, dv2).Length() * Cross(dv1, dv2).Length()) << std::endl;

    if (s >= -EPSILON && s <= 1.0f + EPSILON) {

        std::cout << "s = " << s << "\tDEN ÄR SANN!!!" << std::endl;
        return true;
    }

    return false;*/
}


bool Splittingplane::calculatePointLineIntersection(Vector3<float> intersectionPoint) {

    Vector3<float> difference = mVoronoiPoints.second - mVoronoiPoints.first;

    unsigned int i1 = 0;
    unsigned int i2 = 0;
       
    if(difference[0] < EPSILON && difference[0] > -EPSILON) {

        i1 = 1;
        i2 = 2;

    } else if(difference[1] < EPSILON && difference[1] > -EPSILON) {

        i1 = 0;
        i2 = 2;

    } else {

        i1 = 0;
        i2 = 1;
    }

    float k = (mVoronoiPoints.second[i2] - mVoronoiPoints.first[i2]) / (mVoronoiPoints.second[i1] - mVoronoiPoints.first[i1]);

    float m = k * mVoronoiPoints.first[i1] + mVoronoiPoints.first[i2];

    float line = k * intersectionPoint[i1] + m;

    if(intersectionPoint[i2] < line + EPSILON && intersectionPoint[i2] > line - EPSILON)
        return true;

    return false;
}


void Splittingplane::calculateClosestDistance(Vector3<float> intersectionPoint, std::vector<Vector3<float> > &resolvedVertices) {

    std::vector<float> distances;
    Vector3<float> midPoint = mVoronoiPoints.first + ((mVoronoiPoints.second - mVoronoiPoints.first) / 2.0f);
    float averageDistance = 0.0f;

    for(unsigned int i = 0; i < mUniqueVerts.size(); i++) {
        averageDistance += (mUniqueVerts[i] - midPoint).Length();
        distances.push_back((mUniqueVerts[i] - midPoint).Length());
    }

    averageDistance /= distances.size();

    for(unsigned int i = 0; i < distances.size(); i++) {
        if(distances[i] < averageDistance + EPSILON)
            resolvedVertices.push_back(mUniqueVerts[i]);
    }
}


std::vector<Vector3<float> > Splittingplane::sortVertices(std::vector<Vector3<float> > vertices) {

    Vector3<float> centerPoint = Vector3<float>(0.0f, 0.0f, 0.0f);

    for(unsigned int i = 0; i < vertices.size(); i++) {
        centerPoint += vertices[i];
    }

    centerPoint /= vertices.size();

    // Projected coordinates
    unsigned int s, t;

    // Which vertices do we want to project the polygon onto?
    if(mNormal * Vector3<float>(1.0f, 0.0f, 0.0f) > EPSILON || mNormal * Vector3<float>(1.0f, 0.0f, 0.0f) < -EPSILON ) {
        s = 1;
        t = 2;
    } else if (mNormal * Vector3<float>(0.0f, 1.0f, 0.0f) > EPSILON || mNormal * Vector3<float>(0.0f, 1.0f, 0.0f) < -EPSILON ) {
        s = 0;
        t = 2;
    } else {
        s = 0;
        t = 1;
    }

    std::vector<std::pair<float, Vector3<float> > > unsortedVertices;

    for(unsigned int i = 0; i < vertices.size(); i++) {
        
        Vector3<float> v = vertices[i] - centerPoint;

        float angle = atan2(v[s], v[t]);

        unsortedVertices.push_back(std::make_pair(angle, vertices[i]));
    }

    std::sort(
        unsortedVertices.begin(), 
        unsortedVertices.end(), 
        [](const std::pair<float, Vector3<float> > p1, const std::pair<float, Vector3<float> > p2) { 
            return p1.first < p2.first; 
        } );

    std::vector<Vector3<float>> sortedVertices;

    for(unsigned int i = 0; i < unsortedVertices.size(); i++) {
        sortedVertices.push_back(unsortedVertices[i].second);
    }

    return sortedVertices;
}


void Splittingplane::buildRenderData() {

    mVerts.clear();
    mVerts.shrink_to_fit();

    switch(mUniqueVerts.size()) {
        case 3:

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[1]);
            mVerts.push_back(mUniqueVerts[2]);

            break;
        case 4:
            
            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[1]);
            mVerts.push_back(mUniqueVerts[2]);

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[2]);
            mVerts.push_back(mUniqueVerts[3]);

            break;
        case 5:

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[1]);
            mVerts.push_back(mUniqueVerts[2]);

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[2]);
            mVerts.push_back(mUniqueVerts[3]);

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[3]);
            mVerts.push_back(mUniqueVerts[4]);

            break;
        case 6:
            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[1]);
            mVerts.push_back(mUniqueVerts[2]);

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[2]);
            mVerts.push_back(mUniqueVerts[3]);

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[3]);
            mVerts.push_back(mUniqueVerts[4]);

            mVerts.push_back(mUniqueVerts[0]);
            mVerts.push_back(mUniqueVerts[4]);
            mVerts.push_back(mUniqueVerts[5]);

            break;
        default:
            break;
    }
}

