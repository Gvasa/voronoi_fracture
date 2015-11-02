// Libs and headers
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

//Classes
#include "LoadObj.h"
#include "HalfEdgeMesh.h"

bool LoadObj::Load(HalfEdgeMesh *mesh, std::istream &is){
	// std::cerr << "Reading obj file.\nOutputting any skipped line(s) for reference.\n";
	bool success = ReadHeader(is);
	if(!success) {return false;}

	success = ReadData(is);
	if(!success) {return false;}

	// Build Mesh
	const unsigned int numTriangles = loadData.triangles.size();
	for(unsigned int t = 0; t < numTriangles; t++){
		Vector3<unsigned int> & triangle = loadData.triangles[t];
		std::vector<Vector3 <float> > verts;
		verts.push_back(loadData.verts[triangle[0]]);
		verts.push_back(loadData.verts[triangle[1]]);
		verts.push_back(loadData.verts[triangle[2]]);

		mesh->addFace(verts);
	}
	return true;
}

bool LoadObj::ReadHeader(std::istream &is){
	std::string buf;
	//read only to the first line starting with "v"
	while(!is.eof() && is.peek() != 'v'){
		getline(is, buf);
		//std::cerr << "\"" << buf << "\"\n";
	}
	if (is.good())
		return true;
	else
		return false;
}

bool LoadObj::ReadData(std::istream & is){
  std::string lineBuf;
  int c;
  int i=0;
  while(!is.eof()){
    c = is.peek();
    switch (c) {
    case 'V':
    case 'v':{
      std::string startBuf;
      is >> startBuf; // get the start of the line
      getline(is, lineBuf); // get the rest of the line
      if(startBuf == "v"){
        loadData.verts.push_back(Vector3<float>(lineBuf));
      }
    }
      break;
    case 'F':
    case 'f':
      {
        std::stringstream buf;
        is.get(*buf.rdbuf(), '\n'); // read a line into buf
        is.get(); // read the not extracted \n
        buf << "\n"; // and add it to the string stream

        std::string tmp;
        buf >> tmp; // get the first f or F (+ whitespace)

        // count the number of faces, delimited by whitespace
        int count = 0;
        while (buf >> tmp){
          count++;
        }
        // reset stream
        buf.clear();
        buf.seekg(0, std::ios::beg);

        // Determine wheter we have a triangle or a quad
        if (count == 3){
          loadData.triangles.push_back(ReadTri(buf));
        }
        else {
          std::cerr << "Encountered polygon with " << count << " faces. i'm giving up.\n";
          return false;
        }
      }
      break;
    default:
      // otherwise just skip the row
      getline(is, lineBuf);
      // output it so we see what we miss :)
      // std::cerr << "\"" << lineBuf << "\"\n";
      break;
    }
    i++;
  }
  return true;
}

