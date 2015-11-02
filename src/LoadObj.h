/******************************************************************************************
 *
 * A utility for loading object files (.obj)
 *
 * Based primarily on code from LIU course TNM079 lab files written by:
 * Gunnar Johansson, Ken Museth, Michael Bang Nielsen, Ola Nilsson and Andreas Söderström
 *****************************************************************************************/

#ifndef LOADOBJ_H
#define LOADOBJ_H

// Libs and headers
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

// Classes
#include "HalfEdgeMesh.h"
#include "math/Vector3.h"

class LoadObj{
public: 
	LoadObj() {}

	bool Load(Geometry *, std::istream & is); //false return on error

protected:
	bool ReadHeader(std::istream &is);
	bool ReadData(std::istream &is);

	Vector3<unsigned int> ReadTri(std::istream &is);

	struct LoadData{
		std::vector<Vector3 <float> > verts;
		std::vector<Vector3 <unsigned int> > triangles;
	} loadData;

};

#endif