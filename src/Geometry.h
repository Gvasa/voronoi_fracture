/******************************************************************************
 *
 * An abstraction class for handling of different types of geometries
 * 
 *****************************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

class Geometry {

public: 

    virtual ~Geometry() {};

    // A Geometry must have the following:
    virtual void init() = 0;
    virtual void draw() = 0;
    virtual bool addFace() = 0;

    // A Geometry might have the following:
    // Add functions
};

#endif // GEOMETRY_H