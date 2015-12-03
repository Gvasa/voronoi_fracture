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

    if(mColorScale.size() > 0) {
        mColorScale.clear();
        mColorScale.shrink_to_fit();
    }
}

void Compound::initialize() {

    std::cout << "\nInitializing Compound...\n" << std::endl;

    for(unsigned int i = 0; i < mDebugpoints.size(); i++)
        mDebugpoints[i]->initialize(Vector3<float>(0.0f, 0.0f, 0.0f));

    for(unsigned int i = 0; i < mSplittingPlanes.size(); i++)
        mSplittingPlanes[i]->initialize();


    std::cout << "\nCompound Initialized!\n" << std::endl;
}

void Compound::render(Matrix4x4<float> MVP) {

    std::vector<Matrix4x4<float> > tmp;
    tmp.push_back(MVP);

    for(unsigned int i = 0; i < mSplittingPlanes.size(); i++)
        mSplittingPlanes[i]->render(MVP);

    for(unsigned int i = 0; i < mDebugpoints.size(); i++)
        mDebugpoints[i]->render(tmp);
}


void Compound::calculateVoronoiPattern(Boundingbox* boundingBox, std::vector<Vector3<float> > voronoiPoints) {

    unsigned int planeCounter = 0;
    mVoronoiPoints = voronoiPoints;
    Vector3<float> voronoiMassCenter = Vector3<float>(0.0f, 0.0f, 0.0f);

    //from our voronoipoints create splitting planes and store them in mSplittingplanes
    for(unsigned int i = 0; i < voronoiPoints.size(); i++) {
        
        std::pair<Vector3<float>, Vector3<float> > voronoiPair;

        for (unsigned int j = i+1; j < voronoiPoints.size(); j++) {

            voronoiPair = std::make_pair(voronoiPoints[i], voronoiPoints[j]);
            std::cout << std::endl << voronoiPoints[i] << std::endl;
            std::cout << voronoiPoints[j] << std::endl;
            calculateSplittingPlane(boundingBox, voronoiPair, planeCounter);
            planeCounter++;
            std::cout << "\ncompute splitting plane!\n";
        }

        voronoiMassCenter += voronoiPoints[i];
    }

    voronoiMassCenter /= voronoiPoints.size();

    std::vector<std::pair<std::pair<unsigned int, unsigned int>, std::pair<Vector3<float>, Vector3<float> > > > mPlaneIntersections;
    std::vector<std::pair<unsigned int, unsigned int> > allPlaneIntersections;

    //calculate all the intersections between all the planes
    for(unsigned int i = 0; i < mSplittingPlanes.size(); i++) {
        for (unsigned int j = i + 1; j < mSplittingPlanes.size(); j++) {

            std::pair<Vector3<float>, Vector3<float> > intersectionPointsPair;
            if(calculatePlaneIntersection(mSplittingPlanes[i]->getVertexList(), mSplittingPlanes[j]->getVertexList(), intersectionPointsPair)) {

                mDebugpoints.push_back(new Debugpoint(intersectionPointsPair.first));
                mDebugpoints.push_back(new Debugpoint(intersectionPointsPair.second));

                mPlaneIntersections.push_back(std::make_pair(std::make_pair(i,j), intersectionPointsPair));
            }
        }
    }

    // Check if any splittingplanes have intersected, if not we want to remove
    // the one that is created between the points with longest distance
    if(mPlaneIntersections.size() == 0) {

        float dist = 0.0f;
        unsigned int index = 0;

        for(unsigned int i = 0; i < mSplittingPlanes.size(); i++) {

            std::pair<Vector3<float>, Vector3<float> > vPoints = mSplittingPlanes[i]->getVoronoiPoints();

            if((vPoints.first - vPoints.second).Length() > dist) {
                dist = (vPoints.first - vPoints.second).Length();
                index = i;
            }
        }    
        mSplittingPlanes.erase(mSplittingPlanes.begin()+index);
    }

    //resolve the intersections between the planes and return the new clipped planes.
    for(unsigned int i = 0; i < mPlaneIntersections.size(); i++) {
        
        unsigned int index1 = mPlaneIntersections[i].first.first;
        unsigned int index2 = mPlaneIntersections[i].first.second;
        
        
        mSplittingPlanes[index1]->resolveIntersection(mPlaneIntersections[i].second, voronoiMassCenter);

        mSplittingPlanes[index2]->resolveIntersection(mPlaneIntersections[i].second, voronoiMassCenter);

        std::cout << "TESTAR mSplittingplanes[" << index1 << "] & mSplittingplanes[" << index2 << "]   - convex: " << i <<  std::endl;
        calculateConvexShape(index1, index2, i);
    }
}

void Compound::calculateConvexShape(unsigned int index1, unsigned int index2, unsigned int currentConvex) {

    std::vector<Vector3<float> > v1 = mSplittingPlanes[index1]->getVertexList();
    std::vector<Vector3<float> > v2 = mSplittingPlanes[index2]->getVertexList();
    
    std::vector<Vector3<float> > uniqueVerticesToAdd; //All the uniqueVertices in a cell
    std::vector<unsigned int> verticesIndex;          //The index list to the cell of all vertices, probably used for drawing

    //Bygg upp uniqueVerticesToAdd and verticesIndex
    for(unsigned int i = 0; i < v1.size() ; i++){
        auto it = std::find(uniqueVerticesToAdd.begin(), uniqueVerticesToAdd.end(), v1[i]);
        
        //if the current vertices does not already exists in the unique list add it else just add a index to the indexlist.
        if(it == uniqueVerticesToAdd.end()) {
            uniqueVerticesToAdd.push_back(v1[i]);
            verticesIndex.push_back(uniqueVerticesToAdd.size() - 1);
        } else {
            verticesIndex.push_back(it - uniqueVerticesToAdd.begin());
        }
    }
    
    for(unsigned int i = 0; i < v2.size() ; i++){

        auto it2 = std::find(uniqueVerticesToAdd.begin(), uniqueVerticesToAdd.end(), v2[i]);
        //if the current vertices does not already exists in the unique list add it else just add a index to the indexlist.
        if(it2 == uniqueVerticesToAdd.end()) {
            uniqueVerticesToAdd.push_back(v2[i]);
            verticesIndex.push_back(uniqueVerticesToAdd.size() - 1);
        } else {
            verticesIndex.push_back(it2 - uniqueVerticesToAdd.begin());
        }
    }
    
    //compute the center of our cell
    Vector3<float> cellCenter;
    for(unsigned int i = 0; i < uniqueVerticesToAdd.size(); i++) 
        cellCenter += uniqueVerticesToAdd[i];

    cellCenter /= uniqueVerticesToAdd.size();

    // calculate all distances from the current cellCenter to all the voronoi points
    std::vector<std::pair<float, Vector3<float> > >voronoiDistances;
    for(unsigned int i = 0; i < mVoronoiPoints.size(); i++)
        voronoiDistances.push_back(std::make_pair((cellCenter - mVoronoiPoints[i]).Length(), mVoronoiPoints[i]));

    //sort so we always have the shortes distance first in our vector, this is our cells voronoiPoint
    std::sort(voronoiDistances.begin(), voronoiDistances.end(), [](const std::pair<float, Vector3<float> > p1,
                                                                    const std::pair<float, Vector3<float> > p2)
                                                                    {
                                                                        return p1.first < p2.first;
                                                                    });


    //OTROLIGT HÅRDKODAD SKA FAN INTE VA SÅHÄR!
    float distance1, distance2, distance3;
    
    //calculate if a corner point should be added or not.
    for(unsigned int i = 0; i < mBoundingPoints.size(); i++) {
        
        distance1 = (mBoundingPoints[i] - voronoiDistances[0].second).Length();
        distance2 = (mBoundingPoints[i] - voronoiDistances[1].second).Length();
        distance3 = (mBoundingPoints[i] - voronoiDistances[2].second).Length();

        if(distance1 < distance2 && distance1 < distance3) {
            uniqueVerticesToAdd.push_back(mBoundingPoints[i]);
            verticesIndex.push_back(verticesIndex.back() + 1);
        }
    }

    //add our newly created uniquelist and indexlist to our private vectors
    mUniqueConvexList.push_back(uniqueVerticesToAdd);
    mConvexList.push_back(verticesIndex);
}

void Compound::calculateSplittingPlane(Boundingbox* boundingBox, std::pair<Vector3<float>, Vector3<float> > voronoiPoints, unsigned int planeIndex) {
    
    mBoundingValues.clear();
    mBoundingValues.shrink_to_fit();

    mBoundingValues = boundingBox->getBoundingValues();
    

    Vector3<float> mittPunkt = voronoiPoints.first + (voronoiPoints.second - voronoiPoints.first) / 2.0f;  
    Vector3<float> normal = (voronoiPoints.second - voronoiPoints.first).Normalize();
    
    std::vector<Vector3<float> > xPoints;
    std::vector<Vector3<float> > yPoints;
    std::vector<Vector3<float> > zPoints;

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

    mSplittingPlanes.push_back(new Splittingplane(okPoints, mBoundingValues, voronoiPoints, normal, getColor(mSplittingPlanes.size())));
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
                    std::cout << "tempPoint" <<tempPoint << std::endl;
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

    if(angle > EPSILON_2 || angle < -EPSILON_2)
        return false;
 
    float s = (Cross(dv3, dv2) * Cross(dv1, dv2)) / (Cross(dv1, dv2).Length() * Cross(dv1, dv2).Length());

    if (s >= -EPSILON_2 && s <= 1.0f + EPSILON_2) {

        intersectionPoint = edge1.first + dv1.EntryMult(Vector3<float>(s,s,s));

        return true;
    }

    return false;
}

void Compound::calculateBoundingBoxPoints() {

    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], mBoundingValues[YMIN][1], mBoundingValues[ZMIN][2]) );
    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], mBoundingValues[YMIN][1], mBoundingValues[ZMIN][2]) );
    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], mBoundingValues[YMIN][1], mBoundingValues[ZMAX][2]) );
    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], mBoundingValues[YMIN][1], mBoundingValues[ZMAX][2]) );

    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], mBoundingValues[YMAX][1], mBoundingValues[ZMIN][2]) );
    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], mBoundingValues[YMAX][1], mBoundingValues[ZMIN][2]) );
    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMAX][0], mBoundingValues[YMAX][1], mBoundingValues[ZMAX][2]) );
    mBoundingPoints.push_back(Vector3<float>(mBoundingValues[XMIN][0], mBoundingValues[YMAX][1], mBoundingValues[ZMAX][2]) );

    std::cout << "---- boundingpoints ---" << std::endl;
    for(unsigned int i = 0; i < mBoundingPoints.size(); i++) {
        std::cout << mBoundingPoints[i] << std::endl;
        //  mDebugpoints.push_back(new Debugpoint(mBoundingPoints[i]));
    }
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