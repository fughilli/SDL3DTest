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

const GLchar* VertexShaderSource =
{
    "void main() {\n"\
    "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"\
    "gl_Position = ftransform();\n"\
    "}\n"
};

const GLchar* FragmentShaderSource =
{
    "uniform sampler2D tex;\n"\
    "void main(){\n"\
    "vec4 color = texture2D(tex,gl_TexCoord[0].st);\n"\
    "gl_FragColor = color;\n"\
    "}\n"
};

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

int main( int argc, char* args[])
{
    bool quit = false;
    SDL_Event event;


    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("SDL 3D test",100,100,1280,800,SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    glewInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0f, 1280, 800, 0.0f, -100.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glViewport(0, 0, 1280, 800);

    // Load the texture
    SDL_Surface *triangleSurface;
    GLuint triangleTexture;
    GLenum triangleTextureFormat;
    GLint triangleNumColors;

    if(triangleSurface = IMG_Load("./Resources/tex.png"))
    {
        triangleNumColors = triangleSurface->format->BytesPerPixel;
        if(triangleNumColors == 4)
        {
            triangleTextureFormat = GL_RGBA;
        }
        else if(triangleNumColors == 3)
        {
            triangleTextureFormat = GL_RGB;
        }
        else
        {
            // Texture is not truecolor
        }

        glGenTextures(1, &triangleTexture);
        glBindTexture(GL_TEXTURE_2D, triangleTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D, 0, triangleNumColors,
            triangleSurface->w, triangleSurface->h,
            0, triangleTextureFormat, GL_UNSIGNED_BYTE,
            triangleSurface->pixels);
    }
    // Done loading the texture

    glBindTexture(GL_TEXTURE_2D, triangleTexture);

    GLuint shaderProgram = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
#ifdef DEBUG_LOADING_SHADERS
    std::string vertexShaderSource = get_file_contents("./shader.vert");
    std::string fragmentShaderSource = get_file_contents("./shader.frag");
    GLchar* glVertexShaderSource = new GLchar[vertexShaderSource.size()];
    GLchar* glFragmentShaderSource = new GLchar[fragmentShaderSource.size()];
    memcpy(glVertexShaderSource, vertexShaderSource.c_str(), vertexShaderSource.size()+1);
    memcpy(glFragmentShaderSource, fragmentShaderSource.c_str(), fragmentShaderSource.size()+1);

    glShaderSource(vertexShader, 1, (const GLchar**)(&glVertexShaderSource), NULL);
    glShaderSource(fragmentShader, 1, (const GLchar**)(&glFragmentShaderSource), NULL);

//    glShaderSource(vertexShader, 1, (const GLchar**)(&(GLchar*)(vertexShaderSource.c_str())), NULL);
//    glShaderSource(fragmentShader, 1, (const GLchar**)(&(GLchar*)(fragmentShaderSource.c_str())), NULL);
#else
    glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &FragmentShaderSource, NULL);
#endif

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glValidateProgram(shaderProgram);

#ifdef DEBUG_LOADING_SHADERS
    delete[] glVertexShaderSource;
    delete[] glFragmentShaderSource;
#endif

    float t = 0.0f;

    glVertexPointer(3, GL_FLOAT, 0, vertexlist);
    glEnableClientState(GL_VERTEX_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, texuvlist);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    while(quit != true)
    {
        glUseProgram(shaderProgram);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0,0,0,1);
        t+=0.05f;

        glPushMatrix();

        glTranslatef(1280.0f/2, 800.0f/2, 0.0f);
        glRotatef(t, 1.0f, -1.0f, 1.0f);

        glDrawElements(GL_TRIANGLES, numindices, GL_UNSIGNED_INT, vertexindices);

        glUseProgram(0);

        glPopMatrix();

        SDL_GL_SwapWindow(window);

        SDL_PollEvent(&event);

        if(event.type == SDL_QUIT)
        {
            quit = true;
        }
    }

    if(triangleSurface)
    {
        SDL_FreeSurface(triangleSurface);
    }
    SDL_GL_DeleteContext(glcontext);

    SDL_Quit();
    return 0;
}

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    return "";
}
