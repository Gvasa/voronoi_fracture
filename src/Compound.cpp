#include "Compound.h"

Compound::Compound(Boundingbox* boundingBox, std::vector<Vector3 <float> > voronoiPoints) {

    calculateVoronoiPattern(boundingBox, voronoiPoints);
}


Compound::~Compound() {

}


void Compound::calculateVoronoiPattern(Boundingbox* boundingBox, std::vector<Vector3<float> > voronoiPoints) {

    mVoronoiPoints = voronoiPoints;
    Vector3<float> voronoiMassCenter = Vector3<float>(0.0f, 0.0f, 0.0f);

    //from our voronoipoints create splitting planes and store them in mSplittingplanes
    for(unsigned int i = 0; i < voronoiPoints.size(); i++) {
        
        std::pair<Vector3<float>, Vector3<float> > voronoiPair;

        for (unsigned int j = i+1; j < voronoiPoints.size(); j++) {

            voronoiPair = std::make_pair(voronoiPoints[i], voronoiPoints[j]);
            std::cout << std::endl << voronoiPoints[i] << std::endl;
            std::cout << voronoiPoints[j] << std::endl;
            calculateSplittingPlane(boundingBox, voronoiPair);
            std::cout << "\ncompute splitting plane!\n";
        }

        voronoiMassCenter += voronoiPoints[i];
    }
}


void Compound::calculateSplittingPlane(Boundingbox* boundingBox, std::pair<Vector3<float>, Vector3<float> > voronoiPoints) {
    
    Vector3<float> centerPoint = voronoiPoints.first + (voronoiPoints.second - voronoiPoints.first) / 2.0f;  
    Vector3<float> normal = (voronoiPoints.second - voronoiPoints.first).Normalize();

    mSplittingPlanes.push_back(new Splittingplane(voronoiPoints, normal, centerPoint));
}
