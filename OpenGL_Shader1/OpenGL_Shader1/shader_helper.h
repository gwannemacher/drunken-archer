#pragma once
#include "stdafx.h"
#include <GL/glew.h>
#include <fstream>
#include <cstdio>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include "Vertex.h"

//typedef struct
//{
//  float location[4];
//  float color[4];
//  float normal[4];
//} Vertex;

GLuint SetupGLSL(char *fileName);