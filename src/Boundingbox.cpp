#include "Boundingbox.h"

Boundingbox::Boundingbox(std::vector<Vector3<float> > uniqueVerts) {

    calculateBoundingbox(uniqueVerts);
}


Boundingbox::~Boundingbox() {
    mVerts.clear();
    //mVerts.shrink_to_fit();
}


void Boundingbox::initialize() {

    std::cout << "\nInitializing Boundingbox ...\n";

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders( "shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader" );

    // Set names for our uniforms, same as in shaders
    MVPLoc = glGetUniformLocation(shaderProgram, "MVP");

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

     std::cout << "\nBoundingbox initialized!\n" << std::endl;
}


void Boundingbox::render(Matrix4x4<float> MVP) {

    glDisable( GL_CULL_FACE );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

     // Use shader
    glUseProgram(shaderProgram);

    // Pass values of our matrices and materials to the GPU via uniforms
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVP(0, 0));

    // Rebind the buffer data, vertices are now updated
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vector3<float>), &mVerts[0], GL_STATIC_DRAW);

    // Draw geometry

    
    if(mWireframe)   //Draw Wireframe
        glDrawArrays(GL_LINES, 0, mVerts.size()); // 3 indices starting at 0 -> 1 triangle
    else                //draw triangles
        glDrawArrays(GL_TRIANGLES, 0, mVerts.size());

    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    glDisable( GL_BLEND );
    glEnable( GL_CULL_FACE );
}


void Boundingbox::calculateBoundingbox(std::vector<Vector3<float> > uniqueVerts) {

    std::map<std::string, unsigned int> boundingValues;

    for(unsigned int i = 0; i < uniqueVerts.size(); i++) {

        if(uniqueVerts.at(i)[0] < uniqueVerts.at(boundingValues["xMin"])[0]){
            boundingValues["xMin"] = i;
        }
        if(uniqueVerts.at(i)[0] > uniqueVerts.at(boundingValues["xMax"])[0]){
            boundingValues["xMax"] = i;
        }
        
        if(uniqueVerts.at(i)[1] < uniqueVerts.at(boundingValues["yMin"])[1]){
            boundingValues["yMin"] = i;
        }
        if(uniqueVerts.at(i)[1] > uniqueVerts.at(boundingValues["yMax"])[1]){
            boundingValues["yMax"] = i;
        }
        
        if(uniqueVerts.at(i)[2] < uniqueVerts.at(boundingValues["zMin"])[2]){
            boundingValues["zMin"] = i;
        }
        if(uniqueVerts.at(i)[2] > uniqueVerts.at(boundingValues["zMax"])[2]){
            boundingValues["zMax"] = i;
        }
    }  

    for(std::map<std::string, unsigned int>::iterator it = boundingValues.begin(); it != boundingValues.end(); ++it) {
        std::cout << (*it).first << "  " << uniqueVerts.at((*it).second) << std::endl;
    }

    // xMin Plane
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

    //xMax Plane
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

    //yMin Plane
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

     //yMax Plane
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

     //zMin Plane
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]));

     //zMax Plane
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));


     //Point 1
    std::vector<Vector3<float> > points;
    /*
    points.push_back(Vector3<float>(0.7, 0.3, -0.5));
    points.push_back(Vector3<float>(-0.3, -0.4, 0.6));
*/  


    Vector3<float> A(0.5f, 0.5f, 0.5f);
    Vector3<float> B(-0.25f, -0.25f, -0.25f);
    
    Vector3<float> mittPunkt = A + (B - A) / 2.0f;

    Vector3<float> normal = (B - A).Normalize();


    float x1 = (normal[0]*mittPunkt[1] - normal[1]*uniqueVerts.at(boundingValues["yMin"])[1] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMin"])[2] + normal[2]*mittPunkt[2]) / normal[0];
    float x2 = (normal[0]*mittPunkt[1] - normal[1]*uniqueVerts.at(boundingValues["yMin"])[1] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMax"])[2] + normal[2]*mittPunkt[2]) / normal[0];
    float x3 = (normal[0]*mittPunkt[1] - normal[1]*uniqueVerts.at(boundingValues["yMax"])[1] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMin"])[2] + normal[2]*mittPunkt[2]) / normal[0];
    float x4 = (normal[0]*mittPunkt[1] - normal[1]*uniqueVerts.at(boundingValues["yMax"])[1] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMax"])[2] + normal[2]*mittPunkt[2]) / normal[0];   

    std::cout << "X: " << x1 << " " << x2 << " " << x3 << " " << x4 << std::endl;

    Vector3<float> p1(x2, uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]);
    Vector3<float> p2(x2, uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMin"])[2]);

    float y1 = (-normal[0]*uniqueVerts.at(boundingValues["xMin"])[0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMin"])[2] + normal[2]*mittPunkt[2]) / normal[1];
    float y2 = (-normal[0]*uniqueVerts.at(boundingValues["xMin"])[0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMax"])[2] + normal[2]*mittPunkt[2]) / normal[1];
    float y3 = (-normal[0]*uniqueVerts.at(boundingValues["xMax"])[0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMin"])[2] + normal[2]*mittPunkt[2]) / normal[1];
    float y4 = (-normal[0]*uniqueVerts.at(boundingValues["xMax"])[0] + normal[0]*mittPunkt[0] + normal[1]*mittPunkt[1] - normal[2]*uniqueVerts.at(boundingValues["zMax"])[2] + normal[2]*mittPunkt[2]) / normal[1];  

    std::cout << "y: " << y1 << " " << y2 << " " << y3 << " " << y4 << std::endl;

    Vector3<float> p3(uniqueVerts.at(boundingValues["xMin"])[0], y2, uniqueVerts.at(boundingValues["zMax"])[2]);
    Vector3<float> p4(uniqueVerts.at(boundingValues["xMax"])[0], y3, uniqueVerts.at(boundingValues["zMin"])[2]);

    float z1 = (-normal[0]*uniqueVerts.at(boundingValues["xMin"])[0] + normal[0]*mittPunkt[0] - normal[1]*uniqueVerts.at(boundingValues["yMin"])[1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2];
    float z2 = (-normal[0]*uniqueVerts.at(boundingValues["xMin"])[0] + normal[0]*mittPunkt[0] - normal[1]*uniqueVerts.at(boundingValues["yMax"])[1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2];
    float z3 = (-normal[0]*uniqueVerts.at(boundingValues["xMax"])[0] + normal[0]*mittPunkt[0] - normal[1]*uniqueVerts.at(boundingValues["yMin"])[1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2];
    float z4 = (-normal[0]*uniqueVerts.at(boundingValues["xMax"])[0] + normal[0]*mittPunkt[0] - normal[1]*uniqueVerts.at(boundingValues["yMax"])[1] + normal[1]*mittPunkt[1] + normal[2]*mittPunkt[2]) / normal[2];

    std::cout << "z: " << z1 << " " << z2 << " " << z3 << " " << z4 << std::endl;

    Vector3<float> p5(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], z2);
    Vector3<float> p6(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], z3);
/*
    std::cout << xyminzmax << std::endl;
    std::cout << xymaxzmin << std::endl;

    std::cout << xminyzmax << std::endl;
    std::cout << xmaxyzmin << std::endl;
    
    std::cout << xminymaxz << std::endl;
    std::cout << xmaxyminz << std::endl;*/

    mVerts.push_back(p1);
    mVerts.push_back(p6);
    mVerts.push_back(p3);

    mVerts.push_back(p6);
    mVerts.push_back(p5);
    mVerts.push_back(p3);

    mVerts.push_back(p6);
    mVerts.push_back(p4);
    mVerts.push_back(p5);

    mVerts.push_back(p4);
    mVerts.push_back(p2);
    mVerts.push_back(p5);



    /*
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMin"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMax"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));
    mVerts.push_back(Vector3<float>(uniqueVerts.at(boundingValues["xMin"])[0], uniqueVerts.at(boundingValues["yMax"])[1], uniqueVerts.at(boundingValues["zMax"])[2]));

*/



    // ------------------------------------
    //  SE TILL ATT INGEN DEL AV NORMALEN ÄR 0 , IFSATS!!)
    //  
    // --------------------------------




   // calculateConvexHull(points);

}

void Boundingbox::calculateConvexHull(std::vector<Vector3<float> > points) {




}