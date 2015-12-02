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

        // Loop over all splitting planes for our mesh
        for(unsigned int j = 0; j < numSplittingPlanes; j++) {

            //std::cout << "clip face: " << i << ", with plane: " << j << std::endl;

            Polygon polygon;

            // Collect all vertics from the face that we want to clip
            polygon.v0 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).vert).pos;
            polygon.v1 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).next).vert).pos;
            polygon.v2 = mHalfEdgeMesh->getVert(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getEdge(mHalfEdgeMesh->getFace(i).edge).prev).vert).pos;

            // For now the polygon contains 3 vertices
            polygon.numVerts = 3;

            // Normal of the splitting plane
            Vector3<float> normal = mHalfEdgeMesh->getCompound()->getSplittingPlane(j)->getNormal();

            // Arbitrary point on the splitting plane (any vertex), in this case the first vertex
            Vector3<float> P = mHalfEdgeMesh->getCompound()->getSplittingPlane(j)->getVertex(0);

            //std::cout << "numverts: " << polygon.numVerts << std::endl;

            polygon.numVerts = clipFace(polygon, normal, P);

            std::cout << "numverts: " << polygon.numVerts << std::endl;

            /*************************************************************************
    
            Finalize the SimpleMesh class and call the addFace function here for debugging
            of the clipped polygons. If the polygons have more than 3 verts, then triangulate
            the polygon. Also check the normals of each splitting plane, they need to point
            towards the voronoi point that we are currently evaluating. The voronoi points
            can be accessed via the halfedge pointer in this class.

            *************************************************************************/
        }
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

    //std::cout << "\nmesh is clipped" << std::endl;
    // Shouldn't get here
    return(-1);
}