#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <map>

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"

class Boundingbox {

public:

    Boundingbox(std::vector<Vector3<float> >);
    ~Boundingbox();

    void initialize(Vector3<float>);
    void render(std::vector<Matrix4x4<float> >);

private:

    void calculateBoundingbox(std::vector<Vector3<float> >);

    std::vector<Vector3<float> > mVerts;
};

#endif // BOUNDINGBOX_H