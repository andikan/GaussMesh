//
//  drawFunction.h
//  GaussMesh
//
//  Created by  Andikan on 13/8/4.
//  Copyright (c) 2013年 Andikan. All rights reserved.
//

#ifndef GaussMesh_drawfunction_h
#define GaussMesh_drawfunction_h

#include "IncludeLib.h"

static GLfloat g_axes_vertex_buffer_data[] = {
    0.0f, 0.0f, 0.0f,
    100.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 100.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 100.0f
};

// One color for each vertex. They were generated randomly.
static GLfloat g_axes_color_buffer_data[2*3*3] = {
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};


#endif
