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
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <unistd.h>
#include <map>


// Classes
#include "../math/Vector3.h"


class LoadObj{
public:
	LoadObj() {}

	bool loadObject(std::string fileName); //false return on error

    std::vector<std::vector<Vector3<float> > > getMeshVertexList(std::string objName);

protected:

    std::map<std::string, std::vector<std::vector<Vector3<float> > > > mObjects;  

	bool readHeader(std::istream &is);
	bool readData(std::istream &is);

	Vector3<unsigned int> readTri(std::istream &is);


	struct LoadData{
		std::vector<Vector3 <float> > verts;
		std::vector<Vector3 <unsigned int> > triangles;
	} loadData;

};

#endif