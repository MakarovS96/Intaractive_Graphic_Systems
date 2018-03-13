//
// Created by sennin on 9/5/17.
//

#ifndef HELLOTRIANGLE_SHADER_H
#define HELLOTRIANGLE_SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>


class Shader {
    public:
        // constructor reads and builds the shader
        Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
        Shader(GLuint programID);
        // use/activate the shader
        void use();
        // utility uniform functions
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        //shader ID getter
        GLuint getID();
        //shader functions
        GLuint loadShader(const char * shader_file_path, GLenum SHADER_TYPE);
        GLuint compileShader(GLuint shaderID);
        GLuint linkProgram(GLuint programID);
        GLuint createProgramWithShaders(const char * vertex_shader_file_path, const char * fragment_shader_file_path);

    private:
        // the program ID
        GLuint ID;
};


#endif //HELLOTRIANGLE_SHADER_H
