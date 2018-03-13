//
// Created by sennin on 25.08.17.
//

#ifndef HELLOTRIANGLE_SHADER_LODER_H
#define HELLOTRIANGLE_SHADER_LODER_H

#include <glad/glad.h>


GLuint loadShader(const char * shader_file_path, GLuint SHADER_TYPE);
GLuint compileShader(GLuint shaderID);
GLuint linkProgram(GLuint programID);
GLuint createProgramWithShaders(const char * vertex_shader_file_path, const char * fragment_shader_file_path);

#endif //HELLOTRIANGLE_SHADER_LODER_H
