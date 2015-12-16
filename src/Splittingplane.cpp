#include "Splittingplane.h"

Splittingplane::Splittingplane(Vector3<float> c, std::pair<Vector3<float>, Vector3<float> > vp)
    : mCenterPoint(c), mVoronoiPoints(vp) { }


Splittingplane::~Splittingplane() {

}


void Splittingplane::updateSplittingPlane(glm::mat4 M) {
    
    debug
    glm::vec4 tmpPos = M * glm::vec4(mCenterPoint[0], mCenterPoint[1], mCenterPoint[2], 1.0f);

    mCenterPoint = Vector3<float>(tmpPos.x, tmpPos.y, tmpPos.z) / 2.0f;
    
    std::cout << "updaterar splitting " << std::endl;
}
