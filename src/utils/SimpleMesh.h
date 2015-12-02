#ifndef SIMPLEMESH_H
#define SIMPLEMESH_H

// Libs and headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Classes
#include "../Geometry.h"
#include "../tools/shader.hpp"
#include "Utils.h"

class SimpleMesh : public Geometry {

public:

    SimpleMesh();

    ~SimpleMesh();

    void initialize(Vector3<float>);
    
    void render(std::vector<Matrix4x4<float> >);

    unsigned int getType() { return HALFEDGEMESH; }

private:

    struct Material : public Geometry::Material {   
    } mMaterial;

};

#endif // SIMPLEMESH_H