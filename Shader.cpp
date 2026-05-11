#include "include/Shader.h"
Shader::Shader(GLuint *vertex_s, GLuint *fragment_s,std::string vertex_file, std::string fragment_file)
{
  std::string tmp[2];
  std::ifstream file1, file2;
  std::stringstream buffer;

    file1.open(vertex_file);
    file2.open(fragment_file);
    if(!file1.is_open() || !file2.is_open()){
        std::cerr<<"Gabim ne hapjen e file";
        exit(-1);
    }
    buffer<<file1.rdbuf();
    tmp[0] = buffer.str();
    const char* v_shader = tmp[0].c_str();

    *vertex_s = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(*vertex_s, 1, &v_shader, NULL);
    glCompileShader(*vertex_s);

    GLint success;
    char infoLog[512];


    glGetShaderiv(*vertex_s, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(*vertex_s, 512, NULL, infoLog);
        std::cout << "GABIM: Kompilimi i Vertex Shader dështoi!\n" << infoLog << std::endl;
        exit(-1);
    }

    buffer.str("");
    buffer.clear();
    buffer<<file2.rdbuf();
    tmp[1] = buffer.str();
    const char* f_shader = tmp[1].c_str();

    *fragment_s = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*fragment_s, 1, &f_shader, NULL);
    glCompileShader(*fragment_s);

    glGetShaderiv(*fragment_s, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(*fragment_s, 512, NULL, infoLog);
        std::cout << "GABIM: Kompilimi i Fragment Shader dështoi!\n" << infoLog << std::endl;
        exit(-1);
    }

    file1.close();
    file2.close();

    programId = glCreateProgram();
    glAttachShader(programId, *vertex_s);
    glAttachShader(programId, *fragment_s);
    glLinkProgram(programId);
}
void Shader::useProgram()
{
    glUseProgram(programId);
}