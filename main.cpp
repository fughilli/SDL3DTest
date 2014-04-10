#include "main.h"

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

    GLuint shaderProgram = generate_shader_program("./shader.vert", "./shader.frag");

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

GLuint generate_shader_program(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    GLuint shaderProgram = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderSource = get_file_contents(vertexShaderPath);
    std::string fragmentShaderSource = get_file_contents(fragmentShaderPath);
    GLchar* glVertexShaderSource = new GLchar[vertexShaderSource.size()];
    GLchar* glFragmentShaderSource = new GLchar[fragmentShaderSource.size()];
    memcpy(glVertexShaderSource, vertexShaderSource.c_str(), vertexShaderSource.size()+1);
    memcpy(glFragmentShaderSource, fragmentShaderSource.c_str(), fragmentShaderSource.size()+1);

    glShaderSource(vertexShader, 1, (const GLchar**)(&glVertexShaderSource), NULL);
    glShaderSource(fragmentShader, 1, (const GLchar**)(&glFragmentShaderSource), NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glValidateProgram(shaderProgram);

    delete[] glVertexShaderSource;
    delete[] glFragmentShaderSource;

    return shaderProgram;
}
