#include "ClippingMesh.h"


ClippingMesh::~ClippingMesh() {

    mVerts.clear();
    mVerts.shrink_to_fit();

    mEdges.clear();
    mEdges.shrink_to_fit();

    mFaces.clear();
    mFaces.shrink_to_fit();
}


void ClippingMesh::initialize() {

    std::cout << "\ninitializing ClippingMesh... ";

    std::cout << "Done!\n";
}


bool ClippingMesh::clipMesh(SimpleMesh *sm) {

    unsigned int numSplittingPlanes = mHalfEdgeMesh->getCompound()->getNumberOfSplittingPlanes();

    Vector3<float> refVoronoiPoint = mHalfEdgeMesh->getVoronoiPoint(0);

    std::vector<Vector3<float> > createdVertices;

    unsigned int e1 = mHalfEdgeMesh->getFace(10).edge;
    unsigned int e2 = mHalfEdgeMesh->getEdge(e1).next;
    unsigned int e3 = mHalfEdgeMesh->getEdge(e1).pair;

    std::cout << "\ne1.vert: " << mHalfEdgeMesh->getEdge(e1).vert << std::endl;
    std::cout << "e2.vert: " << mHalfEdgeMesh->getEdge(e2).vert << std::endl;
    std::cout << "e3.vert: " << mHalfEdgeMesh->getEdge(e3).vert << std::endl;

    // Loop over all faces of our mesh
    for(unsigned int i = 0; i < mHalfEdgeMesh->getNumFaces(); i++) {

        Polygon polygon;

        // Collect all vertices from the face that we want to clip
        polygon.v0 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).vert).pos;
        polygon.v1 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).next).vert).pos;
        polygon.v2 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).next).next).vert).pos;

        Vector3<float> correctNormal = mHalfEdgeMesh->getFace(i).normal; //Cross(polygon.v1 - polygon.v0, polygon.v2 - polygon.v0).Normalize();

        polygon.normal = correctNormal;

        //std::cout << "\nPolygon verts: " << polygon.v0 << ", " << polygon.v1 << ", " << polygon.v2 << std::endl;

        // For now the polygon contains 3 vertices
        polygon.numVerts = 3;

        // Loop over all splitting planes for our mesh
        for(unsigned int j = 0; j < 1; j++) {

            std::pair<Vector3<float>, Vector3<float> > voronoiPair = mHalfEdgeMesh->getCompound()->getSplittingPlane(j)->getVoronoiPoints();

            // Normal of the splitting plane, we can get it from SplittingPlane object,
            // but then it might point in the wrong direction so computing it should be faster
            Vector3<float> normal;
            if(voronoiPair.first == refVoronoiPoint)
                normal = (voronoiPair.second - voronoiPair.first).Normalize();
            else
                normal = (voronoiPair.first - voronoiPair.second).Normalize();

            // Arbitrary point on the splitting plane (any vertex), in this case the first vertex
            Vector3<float> P = mHalfEdgeMesh->getCompound()->getSplittingPlane(j)->getVertex(0);

            // Clip the polygon and set the number of vertices
            polygon.numVerts = clipFace(polygon, normal, P, createdVertices);

            /*************************************************************************
    
            Ok, for a convex polygon everything works. However, thats not enough.
            In order to handle concave polygons as well, we need to extend the
            HalfEdgeMesh class to handle non-manifold meshes. The implementation is
            described in the MoA lab1 description paper. We need to handle border cases
            and then traverse the mesh along the border in order to obtain the vertices
            along the border in a sorted manner. Then we can triangulate with the
            algorithm implemented bellow.

            *************************************************************************/
        }

        sortPolygonCounterClockWise(polygon);

        //std::cout << "\n----- NEW FACE -----\n" << std::endl;

        std::vector<Vector3<float> > face1;
        std::vector<Vector3<float> > face2;

        if(polygon.numVerts == 4) {
            
            Polygon P2;
            triangulateQuad(polygon, P2);
            P2.normal = Cross(polygon.v2 - polygon.v0, polygon.v1 - polygon.v0).Normalize();
            //if(!checkTriangleOrientation(P2, correctNormal)) {

            checkTriangleOrientation(P2, correctNormal);

            face1.push_back(P2.v0);
            face1.push_back(P2.v1);
            face1.push_back(P2.v2);
            //sm->addFace(face1);


            /*std::cout << "\nPolygon " << i << " :\n";
            std::cout << "\nv0: " << P2.v0 << "\nv1: " << P2.v1 << "\nv2: " << P2.v2 << std::endl;
            std::cout << "numverts: " << P2.numVerts << std::endl;*/
            //}

        }

        if(polygon.numVerts == 3) {
         
            //if(!checkTriangleOrientation(polygon, correctNormal)){
            
            checkTriangleOrientation(polygon, correctNormal);

            face2.push_back(polygon.v0);
            face2.push_back(polygon.v1);
            face2.push_back(polygon.v2);
            //sm->addFace(face2);

            /*std::cout << "\nPolygon " << i << " :\n";
            std::cout << "\nv0: " << polygon.v0 << "\nv1: " << polygon.v1 << "\nv2: " << polygon.v2 << std::endl;
            std::cout << "numverts: " << polygon.numVerts << std::endl;*/
            //}
        }

        face1.clear();
        face2.clear();
        face1.shrink_to_fit();
        face2.shrink_to_fit();
    }

    sortPolygonCounterClockWise(createdVertices);

    std::cout << "\n--- Created vertices ---\n";

    for(unsigned int i = 0; i < createdVertices.size(); i++) {
        std::cout << "\nv" << i << ": " << createdVertices[i] << std::endl;
    }

   /* std::vector<Vector3<float> > TMP;
    TMP.resize(9);
    TMP[0] = Vector3<float>(0.1f, 0.2f, 2.0f);
    TMP[1] = Vector3<float>(0.0f, 0.6f, 2.0f);
    TMP[2] = Vector3<float>(0.2f, 0.9f, 2.0f);
    TMP[3] = Vector3<float>(0.5f, 1.1f, 2.0f);
    TMP[4] = Vector3<float>(0.8f, 1.0f, 2.0f);
    TMP[5] = Vector3<float>(1.1f, 0.8f, 2.0f);
    TMP[6] = Vector3<float>(1.1f, 0.4f, 2.0f);
    TMP[7] = Vector3<float>(0.9f, 0.1f, 2.0f);
    TMP[8] = Vector3<float>(0.5f, 0.0f, 2.0f);

    sortPolygonCounterClockWise(TMP);

    std::vector<Vector3<float> > tmp;
    tmp.resize(5);

    for(unsigned int i = 0; i < tmp.size(); i++)
        tmp[i] = createdVertices[i];
*/
    triangulate(createdVertices, sm);

    createdVertices.clear();
    createdVertices.shrink_to_fit();
}


int ClippingMesh::clipFace(Polygon &p, Vector3<float> n, Vector3<float> p0, std::vector<Vector3<float> > &createdVerts) {
    
    double A, B, C, D;
    double l;
    double side[3];
    Vector3<float> q;

    //Determine the equation of the plane as
    //Ax + By + Cz + D = 0
  
    l = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    A = (double)n[0] / l;
    B = (double)n[1] / l;
    C = (double)n[2] / l;
    D = -((double)n[0]*(double)p0[0] + (double)n[1]*(double)p0[1] + (double)n[2]*(double)p0[2]);

    // Evaluate the equation of the plane for each vertex
    // If side < 0 then it is on the side to be retained
    // else it is to be clippped

    side[0] = A*(double)p.v0[0] + B*(double)p.v0[1] + C*(double)p.v0[2] + D;
    side[1] = A*(double)p.v1[0] + B*(double)p.v1[1] + C*(double)p.v1[2] + D;
    side[2] = A*(double)p.v2[0] + B*(double)p.v2[1] + C*(double)p.v2[2] + D;

    // Are all the vertices are on the clipped side
    if (side[0] >= 0 && side[1] >= 0 && side[2] >= 0) {
        //std::cout << "--- Clipping 1 ---" << std::endl;
        return(0);
   }

    // Are all the vertices on the not-clipped side
    if (side[0] <= 0 && side[1] <= 0 && side[2] <= 0) {
        //std::cout << "--- Clipping 2 ---" << std::endl;
        return(3);
    }

    // Is p0 the only point on the clipped side
    if (side[0] > 0 && side[1] < 0 && side[2] < 0) {
        q[0] = p.v0[0] - side[0] * (p.v2[0] - p.v0[0]) / (side[2] - side[0]);
        q[1] = p.v0[1] - side[0] * (p.v2[1] - p.v0[1]) / (side[2] - side[0]);
        q[2] = p.v0[2] - side[0] * (p.v2[2] - p.v0[2]) / (side[2] - side[0]);
        p.v3 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);
        q[0] = p.v0[0] - side[0] * (p.v1[0] - p.v0[0]) / (side[1] - side[0]);
        q[1] = p.v0[1] - side[0] * (p.v1[1] - p.v0[1]) / (side[1] - side[0]);
        q[2] = p.v0[2] - side[0] * (p.v1[2] - p.v0[2]) / (side[1] - side[0]);
        p.v0 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);

        //std::cout << "--- Clipping 3 ---" << std::endl;

        return(4);
    }

    // Is p1 the only point on the clipped side 
    if (side[1] > 0 && side[0] < 0 && side[2] < 0) {
        p.v3 = p.v2;
        q[0] = p.v1[0] - side[1] * (p.v2[0] - p.v1[0]) / (side[2] - side[1]);
        q[1] = p.v1[1] - side[1] * (p.v2[1] - p.v1[1]) / (side[2] - side[1]);
        q[2] = p.v1[2] - side[1] * (p.v2[2] - p.v1[2]) / (side[2] - side[1]);
        p.v2 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);
        q[0] = p.v1[0] - side[1] * (p.v0[0] - p.v1[0]) / (side[0] - side[1]);
        q[1] = p.v1[1] - side[1] * (p.v0[1] - p.v1[1]) / (side[0] - side[1]);
        q[2] = p.v1[2] - side[1] * (p.v0[2] - p.v1[2]) / (side[0] - side[1]);
        p.v1 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);

        //std::cout << "--- Clipping 4 ---" << std::endl;

        return(4);
    }

    // Is p2 the only point on the clipped side
    if (side[2] > 0 && side[0] < 0 && side[1] < 0) {
        q[0] = p.v2[0] - side[2] * (p.v0[0] - p.v2[0]) / (side[0] - side[2]);
        q[1] = p.v2[1] - side[2] * (p.v0[1] - p.v2[1]) / (side[0] - side[2]);
        q[2] = p.v2[2] - side[2] * (p.v0[2] - p.v2[2]) / (side[0] - side[2]);
        p.v3 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);
        q[0] = p.v2[0] - side[2] * (p.v1[0] - p.v2[0]) / (side[1] - side[2]);
        q[1] = p.v2[1] - side[2] * (p.v1[1] - p.v2[1]) / (side[1] - side[2]);
        q[2] = p.v2[2] - side[2] * (p.v1[2] - p.v2[2]) / (side[1] - side[2]);
        p.v2 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);

        //std::cout << "--- Clipping 5 ---" << std::endl;

        return(4);
    }

    // Is p0 the only point on the not-clipped side
    if (side[0] < 0 && side[1] > 0 && side[2] > 0) {
        q[0] = p.v0[0] - side[0] * (p.v1[0] - p.v0[0]) / (side[1] - side[0]);
        q[1] = p.v0[1] - side[0] * (p.v1[1] - p.v0[1]) / (side[1] - side[0]);
        q[2] = p.v0[2] - side[0] * (p.v1[2] - p.v0[2]) / (side[1] - side[0]);
        p.v1 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);
        q[0] = p.v0[0] - side[0] * (p.v2[0] - p.v0[0]) / (side[2] - side[0]);
        q[1] = p.v0[1] - side[0] * (p.v2[1] - p.v0[1]) / (side[2] - side[0]);
        q[2] = p.v0[2] - side[0] * (p.v2[2] - p.v0[2]) / (side[2] - side[0]);
        p.v2 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);

        //std::cout << "--- Clipping 6 ---" << std::endl;

        return(3);
    }

    // Is p1 the only point on the not-clipped side
    if (side[1] < 0 && side[0] > 0 && side[2] > 0) {
        q[0] = p.v1[0] - side[1] * (p.v0[0] - p.v1[0]) / (side[0] - side[1]);
        q[1] = p.v1[1] - side[1] * (p.v0[1] - p.v1[1]) / (side[0] - side[1]);
        q[2] = p.v1[2] - side[1] * (p.v0[2] - p.v1[2]) / (side[0] - side[1]);
        p.v0 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);
        q[0] = p.v1[0] - side[1] * (p.v2[0] - p.v1[0]) / (side[2] - side[1]);
        q[1] = p.v1[1] - side[1] * (p.v2[1] - p.v1[1]) / (side[2] - side[1]);
        q[2] = p.v1[2] - side[1] * (p.v2[2] - p.v1[2]) / (side[2] - side[1]);
        p.v2 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);

        //std::cout << "--- Clipping 7 ---" << std::endl;

        return(3);
    }

    // Is p2 the only point on the not-clipped side
    if (side[2] < 0 && side[0] > 0 && side[1] > 0) {
        q[0] = p.v2[0] - side[2] * (p.v1[0] - p.v2[0]) / (side[1] - side[2]);
        q[1] = p.v2[1] - side[2] * (p.v1[1] - p.v2[1]) / (side[1] - side[2]);
        q[2] = p.v2[2] - side[2] * (p.v1[2] - p.v2[2]) / (side[1] - side[2]);
        p.v1 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);
        q[0] = p.v2[0] - side[2] * (p.v0[0] - p.v2[0]) / (side[0] - side[2]);
        q[1] = p.v2[1] - side[2] * (p.v0[1] - p.v2[1]) / (side[0] - side[2]);
        q[2] = p.v2[2] - side[2] * (p.v0[2] - p.v2[2]) / (side[0] - side[2]);
        p.v0 = q;
        if(!findVertex(createdVerts, q))
            createdVerts.push_back(q);

        //std::cout << "--- Clipping 8 ---" << std::endl;

        return(3);
    }

    // Shouldn't get here
    return(-1);
}


bool ClippingMesh::sortPolygonCounterClockWise(Polygon &P) {

    // If the polygon has no vertices, then return it as sorted
    if(P.numVerts == 0)
        return true;

    Vector3<float> centerOfMass;

    if(P.numVerts == 3) {
        centerOfMass = P.v0 + P.v1 + P.v2;
    } else if(P.numVerts == 4) {
        centerOfMass = P.v0 + P.v1 + P.v2 + P.v3;
    }

    centerOfMass /= static_cast<float>(P.numVerts);

    unsigned int s, t;

    if(P.numVerts == 3 || P.numVerts == 4) {

        // Determine projection plane
        if((P.v0[0] > P.v1[0] - EPSILON_2 && P.v0[0] < P.v1[0] + EPSILON_2) && (P.v0[0] > P.v2[0] - EPSILON_2 && P.v0[0] < P.v2[0] + EPSILON_2)) {
            s = 1;
            t = 2;
        } else if((P.v0[1] > P.v1[1] - EPSILON_2 && P.v0[1] < P.v1[1] + EPSILON_2) && (P.v0[1] > P.v2[1] - EPSILON_2 && P.v0[1] < P.v2[1] + EPSILON_2)) {
            s = 0;
            t = 2;
        } else {
            s = 0;
            t = 1;
        }

        std::vector<std::pair<float, Vector3<float> > > verts;

        // Compute angles for each vertex, the static_cast call is just 
        // to ensure that we get a float back from the angle computation
        Vector3<float> v = P.v0 - centerOfMass;
        verts.push_back(std::make_pair(static_cast<float>(atan2(v[s], v[t])), P.v0));

        v = P.v1 - centerOfMass;
        verts.push_back(std::make_pair(static_cast<float>(atan2(v[s], v[t])), P.v1));

        v = P.v2 - centerOfMass;
        verts.push_back(std::make_pair(static_cast<float>(atan2(v[s], v[t])), P.v2));

        if(P.numVerts == 4) {
            v = P.v3 - centerOfMass;
            verts.push_back(std::make_pair(static_cast<float>(atan2(v[s], v[t])), P.v3));
        }

        // Sort verts by angle
        std::sort(
        verts.begin(), 
        verts.end(), 
        [](const std::pair<float, Vector3<float> > p1, const std::pair<float, Vector3<float> > p2) { 
            return p1.first < p2.first; 
        } );

        P.v0 = verts[0].second;
        P.v1 = verts[1].second;
        P.v2 = verts[2].second;

        if(P.numVerts == 4)
            P.v3 = verts[3].second;

        return true;
    }

    // It's impossible to get here
    return false;
}


bool ClippingMesh::sortPolygonCounterClockWise(std::vector<Vector3<float> > &V) {

    // If the polygon has no vertices, then return it as sorted
    if(V.size() == 0)
        return true;

    Vector3<float> centerOfMass;

    for(unsigned int i = 0; i < V.size(); i++) {
        centerOfMass += V[i];
    }

    centerOfMass /= static_cast<float>(V.size());

    if(V.size() >= 3) {

        unsigned int s, t;

        unsigned int i1, i2, i3;
        i1 = 0;
        i2 = V.size() / 2;
        i3 = V.size() - 1;


        float xmax = V[0][0], ymax = V[0][1], zmax = V[0][2], xmin = V[0][0], ymin = V[0][1], zmin = V[0][2];
        
        for(unsigned int i = 0; i < V.size(); i++) {
            if(V[i][0] > xmax)
                xmax = V[i][0];
            if(V[i][1] > ymax)
                ymax = V[i][1];
            if(V[i][2] > zmax)
                zmax = V[i][2];
            if(V[i][0] < xmin)
                xmin = V[i][0];
            if(V[i][1] < ymin)
                ymin = V[i][1];
            if(V[i][2] < zmin)
                zmin = V[i][2];
        }

        float xdiff = fabs(xmax - xmin);
        float ydiff = fabs(ymax - ymin);
        float zdiff = fabs(zmax - zmin);

        // Determine projection plane
        if(xdiff < ydiff && xdiff < zdiff) {
            s = 1;
            t = 2;
            std::cout << "\nX-projection\n";
        } else if(ydiff < xdiff && ydiff < zdiff) {
            s = 0;
            t = 2;
            std::cout << "\nY-projection\n";
        } else {
            s = 0;
            t = 1;
            std::cout << "\nZ-projection\n";
        }

        std::cout << "\nx-max: " << xmax << "\ty-max: " << ymax << "\tz-max: " << zmax << std::endl;
        std::cout << "x-min: " << xmin << "\ty-min: " << ymin << "\tz-min: " << zmin << std::endl;
        std::cout << "x-diff: " << xdiff << "\ty-diff: " << ydiff << "\tz-diff: " << zdiff << std::endl;

        /*
        if((V[i1][0] > V[i2][0] - 0.5f && V[i1][0] < V[i2][0] + 0.5f) && (V[i1][0] > V[i3][0] - 0.5f && V[i1][0] < V[i3][0] + 0.5f)) {
            s = 1;
            t = 2;
            //std::cout << "\nX-projection\n";
        } else if((V[i1][1] > V[i2][1] - 0.5f && V[i1][1] < V[i2][1] + 0.5f) && (V[i1][1] > V[i3][1] - 0.5f && V[i1][1] < V[i3][1] + 0.5f)) {
            s = 0;
            t = 2;
            //std::cout << "\nY-projection\n";
        } else {
            s = 0;
            t = 1;
            //std::cout << "\nZ-projection\n";
        }*/

        std::vector<std::pair<float, Vector3<float> > > verts;

        Vector3<float> v;
        for(unsigned int i = 0; i < V.size(); i++) {
            // Compute angles for each vertex, the static_cast call is just 
            // to ensure that we get a float back from the angle computation
            v = V[i] - centerOfMass;
            verts.push_back(std::make_pair(static_cast<float>(atan2(v[s], v[t])), V[i]));
        }

        // Sort verts by angle
        std::sort(
        verts.begin(), 
        verts.end(), 
        [](const std::pair<float, Vector3<float> > p1, const std::pair<float, Vector3<float> > p2) { 
            return p1.first < p2.first; 
        } );

        for(unsigned int i = 0; i < V.size(); i++)
            V[i] = verts[i].second;

        std::cout << "\nSorting done!" << std::endl;
        return true;
    }

    // It's impossible to get here
    return false;
}


bool ClippingMesh::triangulateQuad(Polygon &P1, Polygon &P2) {

    P2.v0 = P1.v0;
    P2.v1 = P1.v2;
    P2.v2 = P1.v3;

    P1.numVerts = 3;
    P2.numVerts = 3;
}


bool ClippingMesh::triangulate(std::vector<Vector3<float> > &P, SimpleMesh *sm) {

    std::cout << "\ntriangulating..." << std::endl;

    // Can't make any triangles
    if(P.size() < 3) {
        //std::cout << "less than 3" << std::endl;
        return false;
    }
    else if(P.size() == 3) {
        // Add the polygon as it is, might not be needed.
        //std::cout << "exactly 3" << std::endl;
    }

    unsigned int vectorSize = P.size();
    unsigned int currentVectorSize = P.size();

    std::vector<Vector3<float> > face;
    face.resize(3);

    for(unsigned int i = 0; i < currentVectorSize;) {
        for(unsigned int j = 0; j < currentVectorSize; j++) {

            if(P.size() == 3) {

                face.resize(3);

                face[0] = P[0];
                face[1] = P[2];
                face[2] = P[1];

                std::cout << "\nVert 0: " << face[0] << std::endl;
                std::cout << "Vert 1: " << face[1] << std::endl;
                std::cout << "Vert 2: " << face[2] << std::endl << std::endl;

                sm->addFace(face);

                face.clear();
                //face.shrink_to_fit();

                return true;
            }

            // Dont go anything if are on the triangle we are evaluating
            if(i == 0 && (j == currentVectorSize-1 || j == 1))
                continue;
            else if((j == i || j == i - 1) || j == i + 1)
                continue;

            unsigned int i1, i2, i3;

            if(i == 0) {
                i1 = currentVectorSize - 1;
                i2 = i;
                i3 = i+1;
            }
            else if(i == currentVectorSize - 1) {
                i1 = i-1;
                i2 = i;
                i3 = 0;
            }
            else {
                i1 = i-1;
                i2 = i;
                i3 = i + 1;
            }

            std::vector<Vector3<float> > V;
            V.resize(3);
            V[0] = P[i1];
            V[1] = P[i2];
            V[2] = P[i3];

            if(isInsidePolygon(V, P[j])) {
                // We cant make a triangle yet, move on.
                //std::cout << "\n\nDEN LIGGER INNANFÖR!!\n\n";
                i++;
                continue;
            }
            else {

                if( fabs(acos((P[i1] - P[i2]) * (P[i3] - P[i2]))) < M_PI - EPSILON ) {
                    // We can add a triangle, nice.
                    std::cout << "\n\nDEN LIGGER INTE INNANFÖR!!" << " i:" << i << "\n";

                    face.resize(3);

                    face[0] = P[i1];
                    face[1] = P[i3];
                    face[2] = P[i2];

                    std::cout << "Vert 0: " << face[0] << std::endl;
                    std::cout << "Vert 1: " << face[1] << std::endl;
                    std::cout << "Vert 2: " << face[2] << std::endl << std::endl;

                    sm->addFace(face);
                    face.clear();

                    P.erase(P.begin() + i);
                    currentVectorSize--;
                    i = 0;
                    break;

                }
                //std::vector<Vector3<float> > 
                i++;
                //sm->addFace(face2);
                
                
            }
        }
    }

    std::cout << "\ntriangulation done!" << std::endl;

    return true;
}


bool ClippingMesh::isInsidePolygon(const std::vector<Vector3<float> > &F, const Vector3<float> &v) {

    if( (sameSide(v, F[0], F[1], F[2]) && sameSide(v, F[1], F[0], F[2])) && sameSide(v, F[2], F[0], F[1]) )
        return true;
    else
        return false;
}


bool ClippingMesh::sameSide(Vector3<float> p1, Vector3<float> p2, Vector3<float> a, Vector3<float> b) {

    Vector3<float> cp1 = Cross(b - a, p1 - a);
    Vector3<float> cp2 = Cross(b - a, p2 - a);

    if(cp1 * cp2 >= 0.0f)
        return true;
    else
        return false;
}


bool ClippingMesh::checkTriangleOrientation(Polygon &P, Vector3<float> normal) {

    Vector3<float> edge1 = P.v1 - P.v0;
    Vector3<float> edge2 = P.v2 - P.v0;

    Vector3<float> polygonNormal = Cross(edge1, edge2).Normalize();

    if((polygonNormal[0] > normal[0] - EPSILON && polygonNormal[0] < normal[0] + EPSILON) &&
       (polygonNormal[1] > normal[1] - EPSILON && polygonNormal[1] < normal[1] + EPSILON) &&
       ((polygonNormal[2] > normal[2] - EPSILON && polygonNormal[2] < normal[2] + EPSILON))) {
        /*std::cout << "\nSAMMA NORMAL" << std::endl;
        std::cout << "correct normal: " << P.normal << std::endl;
        std::cout << "this normal: " << polygonNormal << std::endl;*/
        return true;
    } else {
        /*std::cout << "\nOLIKA NORMAL" << std::endl;
        std::cout << "correct normal: " << P.normal << std::endl;
        std::cout << "this normal: " << polygonNormal << std::endl;*/

        Vector3<float> tmp = P.v1;
        P.v1 = P.v2;
        P.v2 = tmp;
        P.normal = Cross(P.v1 - P.v0, P.v2 - P.v0).Normalize();
        //std::cout << "fixed normal: " << P.normal << std::endl;
        return false;
    }
}


bool ClippingMesh::findVertex(std::vector<Vector3<float> > &V, Vector3<float> v) {

    std::vector<Vector3<float> >::iterator it;

    it = find(V.begin(), V.end(), v);

    if(it != V.end())
        return true;
    else
        return false;
}

