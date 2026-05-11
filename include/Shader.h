#ifndef SHADER_H
#define SHADER_H
#endif

#include "../glad/glad.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
  public:
    GLuint programId;
    Shader(GLuint *vertex_s, GLuint *fragment_s,std::string vertex_file, std::string fragment_file);
    void useProgram();
  };



