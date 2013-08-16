//
//  ContourReader.h
//  GaussMesh
//

#ifndef GaussMesh_ContourReader_h
#define GaussMesh_ContourReader_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Mesh_structure.h"

#define SIZE 100

using namespace std;


class ContourReader
{
public:
    fstream  file;
    string filename;
    
    ContourReader(string filepath);
    ~ContourReader();
    vector<Point*> getContourPoints();
};



#endif
