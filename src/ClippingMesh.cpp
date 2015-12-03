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


bool ClippingMesh::clipMesh() {

    unsigned int numSplittingPlanes = mHalfEdgeMesh->getCompound()->getNumberOfSplittingPlanes();

    SimpleMesh *sm = new SimpleMesh();

    // Loop over all faces of our mesh
    for(unsigned int i = 0; i < mHalfEdgeMesh->getNumFaces(); i++) {

        Polygon polygon;

        // Collect all vertics from the face that we want to clip
        polygon.v0 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).vert).pos;
        polygon.v1 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).next).vert).pos;
        polygon.v2 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).prev).vert).pos;

        // For now the polygon contains 3 vertices
        polygon.numVerts = 3;

        // Loop over all splitting planes for our mesh
        for(unsigned int j = 0; j < numSplittingPlanes; j++) {

            std::pair<Vector3<float>, Vector3<float> > voronoiPair = mHalfEdgeMesh->getCompound()->getSplittingPlane(j)->getVoronoiPoints();

            // Normal of the splitting plane, we can get it from SplittingPlane object,
            // but then it might point in the wrong direction so computing it should be faster
            Vector3<float> normal = (voronoiPair.first - voronoiPair.second).Normalize();

            // Arbitrary point on the splitting plane (any vertex), in this case the first vertex
            Vector3<float> P = mHalfEdgeMesh->getCompound()->getSplittingPlane(j)->getVertex(0);

            //std::cout << "numverts: " << polygon.numVerts << std::endl;

            polygon.numVerts = clipFace(polygon, normal, P);

            /*************************************************************************
    
            Finalize the SimpleMesh class and call the addFace function here for debugging
            of the clipped polygons. If the polygons have more than 3 verts, then triangulate
            the polygon. Also check the normals of each splitting plane, they need to point
            towards the voronoi point that we are currently evaluating. The voronoi points
            can be accessed via the halfedge pointer in this class.

            *************************************************************************/
        }

        if(polygon.numVerts == 4) {
            std::cout << "\nPolygon " << i << " :\n";
            std::cout << "\nv0: " << polygon.v0 << "\nv1: " << polygon.v1 << "\nv2: " << polygon.v2 << "\nv3: " << polygon.v3 << std::endl;
        } else if(polygon.numVerts == 3) {
            std::cout << "\nPolygon " << i << " :\n";
            std::cout << "\nv0: " << polygon.v0 << "\nv1: " << polygon.v1 << "\nv2: " << polygon.v2 << std::endl;
        }

        sortPolygonCounterClockWise(polygon);

        if(polygon.numVerts == 4) {
            Polygon P2;
            triangulate(polygon, P2);
        }


        //std::cout << "numverts: " << polygon.numVerts << std::endl;
    }
}


int ClippingMesh::clipFace(Polygon &p, Vector3<float> n, Vector3<float> p0) {
    
    double A, B, C, D;
    double l;
    double side[3];
    Vector3<float> q;

   
    //Determine the equation of the plane as
    //Ax + By + Cz + D = 0
  
    l = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    A = n[0] / l;
    B = n[1] / l;
    C = n[2] / l;
    D = -(n[0]*p0[0] + n[1]*p0[1] + n[2]*p0[2]);

    // Evaluate the equation of the plane for each vertex
    // If side < 0 then it is on the side to be retained
    // else it is to be clippped

    side[0] = A*p.v0[0] + B*p.v0[1] + C*p.v0[2] + D;
    side[1] = A*p.v1[0] + B*p.v1[1] + C*p.v1[2] + D;
    side[2] = A*p.v2[0] + B*p.v2[1] + C*p.v2[2] + D;

    // Are all the vertices are on the clipped side
    if (side[0] >= 0 && side[1] >= 0 && side[2] >= 0)
       return(0);

    // Are all the vertices on the not-clipped side
    if (side[0] <= 0 && side[1] <= 0 && side[2] <= 0)
       return(3);

    // Is p0 the only point on the clipped side
    if (side[0] > 0 && side[1] < 0 && side[2] < 0) {
       q[0] = p.v0[0] - side[0] * (p.v2[0] - p.v0[0]) / (side[2] - side[0]);
       q[1] = p.v0[1] - side[0] * (p.v2[1] - p.v0[1]) / (side[2] - side[0]);
       q[2] = p.v0[2] - side[0] * (p.v2[2] - p.v0[2]) / (side[2] - side[0]);
       p.v3 = q;
       q[0] = p.v0[0] - side[0] * (p.v1[0] - p.v0[0]) / (side[1] - side[0]);
       q[1] = p.v0[1] - side[0] * (p.v1[1] - p.v0[1]) / (side[1] - side[0]);
       q[2] = p.v0[2] - side[0] * (p.v1[2] - p.v0[2]) / (side[1] - side[0]);
       p.v0 = q;
       return(4);
    }

    // Is p1 the only point on the clipped side 
    if (side[1] > 0 && side[0] < 0 && side[2] < 0) {
       p.v3 = p.v2;
       q[0] = p.v1[0] - side[1] * (p.v2[0] - p.v1[0]) / (side[2] - side[1]);
       q[1] = p.v1[1] - side[1] * (p.v2[1] - p.v1[1]) / (side[2] - side[1]);
       q[2] = p.v1[2] - side[1] * (p.v2[2] - p.v1[2]) / (side[2] - side[1]);
       p.v2 = q;
       q[0] = p.v1[0] - side[1] * (p.v0[0] - p.v1[0]) / (side[0] - side[1]);
       q[1] = p.v1[1] - side[1] * (p.v0[1] - p.v1[1]) / (side[0] - side[1]);
       q[2] = p.v1[2] - side[1] * (p.v0[2] - p.v1[2]) / (side[0] - side[1]);
       p.v1 = q;
       return(4);
    }

    // Is p2 the only point on the clipped side
    if (side[2] > 0 && side[0] < 0 && side[1] < 0) {
       q[0] = p.v2[0] - side[2] * (p.v0[0] - p.v2[0]) / (side[0] - side[2]);
       q[1] = p.v2[1] - side[2] * (p.v0[1] - p.v2[1]) / (side[0] - side[2]);
       q[2] = p.v2[2] - side[2] * (p.v0[2] - p.v2[2]) / (side[0] - side[2]);
       p.v3 = q;
       q[0] = p.v2[0] - side[2] * (p.v1[0] - p.v2[0]) / (side[1] - side[2]);
       q[1] = p.v2[1] - side[2] * (p.v1[1] - p.v2[1]) / (side[1] - side[2]);
       q[2] = p.v2[2] - side[2] * (p.v1[2] - p.v2[2]) / (side[1] - side[2]);
       p.v2 = q;
       return(4);
    }

    // Is p0 the only point on the not-clipped side
    if (side[0] < 0 && side[1] > 0 && side[2] > 0) {
       q[0] = p.v0[0] - side[0] * (p.v1[0] - p.v0[0]) / (side[1] - side[0]);
       q[1] = p.v0[1] - side[0] * (p.v1[1] - p.v0[1]) / (side[1] - side[0]);
       q[2] = p.v0[2] - side[0] * (p.v1[2] - p.v0[2]) / (side[1] - side[0]);
       p.v1 = q;
       q[0] = p.v0[0] - side[0] * (p.v2[0] - p.v0[0]) / (side[2] - side[0]);
       q[1] = p.v0[1] - side[0] * (p.v2[1] - p.v0[1]) / (side[2] - side[0]);
       q[2] = p.v0[2] - side[0] * (p.v2[2] - p.v0[2]) / (side[2] - side[0]);
       p.v2 = q;
       return(3);
    }

    // Is p1 the only point on the not-clipped side
    if (side[1] < 0 && side[0] > 0 && side[2] > 0) {
       q[0] = p.v1[0] - side[1] * (p.v0[0] - p.v1[0]) / (side[0] - side[1]);
       q[1] = p.v1[1] - side[1] * (p.v0[1] - p.v1[1]) / (side[0] - side[1]);
       q[2] = p.v1[2] - side[1] * (p.v0[2] - p.v1[2]) / (side[0] - side[1]);
       p.v0 = q;
       q[0] = p.v1[0] - side[1] * (p.v2[0] - p.v1[0]) / (side[2] - side[1]);
       q[1] = p.v1[1] - side[1] * (p.v2[1] - p.v1[1]) / (side[2] - side[1]);
       q[2] = p.v1[2] - side[1] * (p.v2[2] - p.v1[2]) / (side[2] - side[1]);
       p.v2 = q;
       return(3);
    }

    // Is p2 the only point on the not-clipped side
    if (side[2] < 0 && side[0] > 0 && side[1] > 0) {
       q[0] = p.v2[0] - side[2] * (p.v1[0] - p.v2[0]) / (side[1] - side[2]);
       q[1] = p.v2[1] - side[2] * (p.v1[1] - p.v2[1]) / (side[1] - side[2]);
       q[2] = p.v2[2] - side[2] * (p.v1[2] - p.v2[2]) / (side[1] - side[2]);
       p.v1 = q;
       q[0] = p.v2[0] - side[2] * (p.v0[0] - p.v2[0]) / (side[0] - side[2]);
       q[1] = p.v2[1] - side[2] * (p.v0[1] - p.v2[1]) / (side[0] - side[2]);
       q[2] = p.v2[2] - side[2] * (p.v0[2] - p.v2[2]) / (side[0] - side[2]);
       p.v0 = q;
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

        /*for(unsigned int i = 0; i < verts.size(); i++) {
            std::cout << "\nangle: " << verts[i].first << std::endl;
            std::cout << "v0: " << verts[i].second << std::endl;
        }*/

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


bool ClippingMesh::triangulate(Polygon &P1, Polygon &P2) {

    P2.v0 = P1.v0;
    P2.v1 = P1.v2;
    P2.v2 = P1.v3;

    P1.numVerts = 3;
    P2.numVerts = 3;
}

