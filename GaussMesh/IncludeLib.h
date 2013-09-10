//
//  IncludeLib.h
//  GaussMesh
//
//  Created by  Andikan on 13/8/2.
//  Copyright (c) 2013 Andikan. All rights reserved.
//

#ifndef GaussMesh_IncludeLib_h
#define GaussMesh_IncludeLib_h

// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>       /* sqrt */
using namespace std;


// Include GLEW
#ifndef GLEW_DEFINED
#define GLEW_DEFINED
#include <GL/glew.h>
#endif

// Include GLFW
#ifndef GLFW_DEFINED
#define GLFW_DEFINED
#include <GL/glfw.h>
#endif

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
// #include <AntTweakBar.h>

// Include poly2tri
#include "poly2tri/poly2tri.h"

// Include header
#include "drawFunction.h"
#include "Mesh_structure.h"
#include "ContourReader.h"
#include "Triangulation.h"





#endif
