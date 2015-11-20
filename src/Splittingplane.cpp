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

    std::cout << "\ndelete splitting plane!" << std::endl;

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderProgram);

    if(mVerts.size() > 0) {
        mVerts.clear();
        mVerts.shrink_to_fit();
    }

    if(mUniqueVerts.size() > 0) {
        mUniqueVerts.clear();
        mUniqueVerts.shrink_to_fit();
    }

    for(unsigned int i = 0; i < mDebugPoints.size(); ++i)
        delete mDebugPoints[i];

    if(mDebugPoints.size() > 0) {
        mDebugPoints.clear();
        mDebugPoints.shrink_to_fit();
    }
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


void Splittingplane::resolveIntersection(std::pair<Vector3<float>, Vector3<float> > intersectionEdge, Vector3<float> voronoiMassCenter) {

    // Check if the intersectionedge already has been tested with the plane
    //std::cout << "---- testar plan ---- " 
    for(auto it = mIntersectedPoints.begin(); it != mIntersectedPoints.end(); ++it) {

        if( ( (intersectionEdge.first - (*it).first).Length() < EPSILON ) &&  ( (intersectionEdge.first - (*it).first).Length() > -EPSILON) ){
            if( ( (intersectionEdge.second - (*it).second).Length() < EPSILON ) &&  ( (intersectionEdge.second - (*it).second).Length() > -EPSILON) )     
                return;
            else
                continue;

        } else if( ( (intersectionEdge.first - (*it).second).Length() < EPSILON ) &&  ( (intersectionEdge.first - (*it).second).Length() > -EPSILON) ) {
            if( ( (intersectionEdge.second - (*it).first).Length() < EPSILON ) &&  ( (intersectionEdge.second - (*it).first).Length() > -EPSILON) )
                return;
            else
                continue;
        }
    }

    //if it hasn't been tested add it to the tested edges.
    mIntersectedPoints.push_back(intersectionEdge);

    std::vector<Vector3<float> > resolvedVertices;

    //push back the intersection points, will always be used in the new plane
    resolvedVertices.push_back(intersectionEdge.first);
    resolvedVertices.push_back(intersectionEdge.second);
    
    Vector3<float> difference = mVoronoiPoints.first - mVoronoiPoints.second;

    unsigned int i1 = 0;
    unsigned int i2 = 0;
    unsigned int i3 = 0;

    unsigned int projection; 

    //check the difference to know which way to project the plane for further calculations
    if(fabs(difference[0]) > fabs(difference[1] + EPSILON)) {
        if(fabs(difference[0]) > fabs(difference[2] + EPSILON)) {
            i1 = 2;
            i2 = 1;
            i3 = 0;
            projection = XPROJECTION;
        } else {
            i1 = 0;
            i2 = 1;
            i3 = 2;
            projection = ZPROJECTION;
        }
    } else {
        i1 = 2;
        i2 = 0;
        i3 = 1;
        projection = YPROJECTION;
    }

    Vector3<float> intersectionCenter = intersectionEdge.first + (intersectionEdge.second - intersectionEdge.first) / 2.0f;  
   
    Vector3<float> centerV1 = mVoronoiPoints.first - intersectionCenter;
    Vector3<float> centerV2 = mVoronoiPoints.second - intersectionCenter;
    float angleV1 = atan2(centerV1[i2], centerV1[i1]);
    float angleV2 = atan2(centerV2[i2], centerV2[i1]);
    float angleVoronoiIntersect;
    float angle = angleV1;


    //this is a special case when the two voronoi points has one positive and one negative value
    //it will calculate which voronoi points to use when deciding if plane points should be kept or not
    if((angleV1 > 0.0f && angleV2 < 0.0f) || (angleV1 < 0.0f && angleV2 > 0.0f)) {
        std::cout << "--- ifsats -- " << std::endl;


        Vector3<float> v1v2 = mVoronoiPoints.second - mVoronoiPoints.first;
        Vector3<float> i1i2 = intersectionEdge.second - intersectionEdge.first;

        //make sure that the two vectors being tested is pointing correctly (both positive)
       /*
        if(v1v2[i2] > intersectionCenter - EPSILON && i1i2[i1] < intersectionCenter + EPSILON) 
            i1i2 *= -1;
        else if(v1v2[i2] < intersectionCenter + EPSILON && i1i2[i1] > intersectionCenter - EPSILON) 
            i1i2 *= -1;
        */
        std::cout << "IPC: " << intersectionCenter << std::endl;
        std::cout << "v1v2: " << v1v2 << std::endl;
        std::cout << "i1i2: " << i1i2 << std::endl;

        if(v1v2[i2] < intersectionCenter[i2] - EPSILON)
            v1v2 *= -1;
        if(i1i2[i2] < intersectionCenter[i2] - EPSILON)
            i1i2 *= -1;
         

        std::cout << "---- flip ----" << std::endl;
        std::cout << "v1v2: " << v1v2 << std::endl;
        std::cout << "i1i2: " << i1i2 << std::endl;

        v1v2[i3] = 0.0f;
        i1i2[i3] = 0.0f;
        
        v1v2 = v1v2.Normalize();
        i1i2 = i1i2.Normalize();

        angleVoronoiIntersect = acos(v1v2 * i1i2);
        
        Vector3<float> vm;
        Vector3<float> im;
        float dot;
        float det;

        std::cout << "angleVoronoiIntersect: " << angleVoronoiIntersect << std::endl;


        Vector3<float> v1vc = mVoronoiPoints.first - voronoiMassCenter;
        Vector3<float> v2vc = mVoronoiPoints.second - voronoiMassCenter;

        v1vc[i3] = 0.0f;
        v2vc[i3] = 0.0f;

        //depending on the angle between the two vectors we choose which voronoi point to use
        //if( angleVoronoiIntersect < (M_PI/2.0f) + EPSILON ) {
        if( v1vc.Length() > v2vc.Length() ) {
            std::cout << "valde mVoronoiPoints.first: " << mVoronoiPoints.first << std::endl; 

            vm = (mVoronoiPoints.first - intersectionCenter).Normalize();
            vm[i3] = 0.0f;

            im = (intersectionEdge.second - intersectionCenter).Normalize();
            im[i3] = 0.0f;

            dot = vm * im;
            det = vm[i1]*im[i2] - vm[i2]*im[i1];

            angle = atan2(det, dot);
        } else {
                
            std::cout << "valde mVoronoiPoints.second: " << mVoronoiPoints.second << std::endl;

            vm = (mVoronoiPoints.second - intersectionCenter).Normalize();
            vm[i3] = 0.0f;

            im = (intersectionEdge.second - intersectionCenter).Normalize();
            im[i3] = 0.0f;

            dot = vm * im;
            det = vm[i1]*im[i2] - vm[i2]*im[i1];

            angle = atan2(det, dot);
        }
    }

    // loop through the planes points and see if it should be kept or not
    for(unsigned int i = 0; i < mUniqueVerts.size(); i++) {
    
        Vector3<float> uVmP = (mUniqueVerts[i] - intersectionCenter).Normalize();
        Vector3<float> iPmP = (intersectionEdge.second - intersectionCenter).Normalize();

        uVmP[i3] = 0.0f;
        iPmP[i3] = 0.0f;

        float dot2 = uVmP * iPmP;
        float det2 = uVmP[i1]*iPmP[i2] - uVmP[i2]*iPmP[i1];

        float angle_current_point = atan2(det2, dot2);

        if(angle < 0.0f) {
            
            if(angle_current_point < 0.0f) 
                resolvedVertices.push_back(mUniqueVerts[i]);
        } else {    

            if(angle_current_point > 0.0f)
                resolvedVertices.push_back(mUniqueVerts[i]);
        }
    }

    mUniqueVerts = sortVertices(resolvedVertices);

    buildRenderData();
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


void Splittingplane::resetSplittingPlane() {

    if(mIntersectedPoints.size() > 0) {
        mIntersectedPoints.clear();
        mIntersectedPoints.shrink_to_fit();
    }
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

