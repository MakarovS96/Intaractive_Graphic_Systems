//
// Created by sennin on 9/5/17.
//

#ifndef HELLOTRIANGLE_SHADER_H
#define HELLOTRIANGLE_SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>

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
        // ------------------------------------------------------------------------
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec2(const std::string &name, float x, float y) const;
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w);
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const;

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
