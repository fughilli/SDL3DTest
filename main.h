#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gl/glew.h>
#include <gl/wglew.h>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#define DEBUG_LOADING_SHADERS

using namespace std;

const int NUM_QUADS_IN_RING = 10;
const float RING_RADIUS = 100.0f;
const float RING_THICKNESS = 40.0f;
const float PI = 3.1415926535897932384626433832795f;
const float TWO_PI = PI*2;

const int numvertices = 5;
const int numindices = 9;

GLfloat vertexlist[numvertices*3] =
{
    0, 0, 0,
    -50, -50, 0,
    50, -50, 0,
    50, 50, 0,
    -50, 50, 0
};

GLuint vertexindices[numindices] =
{
    0,1,2,0,3,4,0,2,3
};

GLfloat texuvlist[numvertices*2] =
{
    0.5f,0.5f,
    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f
};

std::string get_file_contents(const char *filename);
GLuint generate_shader_program(const char *vertexShaderPath, const char *fragmentShaderPath);
