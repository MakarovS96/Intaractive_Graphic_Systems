//
// Created by sennin on 25.08.17.
//

#include "shaderIzi.h"
#include <fstream>

GLuint loadShader(const char * shader_file_path, GLenum SHADER_TYPE) {
    GLuint shaderID = glCreateShader(SHADER_TYPE);
    std::string shaderCode;
    std::ifstream shaderFileStream(shader_file_path, std::ios::in);

    if (shaderFileStream.is_open()) {
        std::string Line = "";
        while (getline(shaderFileStream,Line)) {
            shaderCode += Line + "\n";
        }
        shaderFileStream.close();
    } else {
        fprintf(stdout, "We can't load shader file \"%s\".", shader_file_path);
        return GL_FALSE;
    }

    const char * shaderCodePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &shaderCodePointer, NULL);

    return shaderID;
}

GLuint compileShader(GLuint shaderID) {
    GLint success = GL_FALSE;
    int infoLogLength = 0;

    glCompileShader(shaderID);
    glGetShaderiv(shaderID,GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto* infoLog = new GLchar[infoLogLength];
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, infoLog);
        fprintf(stdout, "Compile shader error: \n%s\n", infoLog);

        delete[] infoLog;
        return GL_FALSE;
    }

    return shaderID;
}

GLuint linkProgram(GLuint programID) {
    GLint success = GL_FALSE;
    int infoLogLength = 0;

    glLinkProgram(programID);
    glGetProgramiv(programID,GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto* infoLog = new GLchar[infoLogLength];
        glGetProgramInfoLog(programID, infoLogLength, NULL, infoLog);
        fprintf(stdout, "Linking program error: \n%s\n", infoLog);

        delete[] infoLog;
        return GL_FALSE;
    }

    return programID;
}

GLuint createProgramWithShaders(const char *vertex_shader_file_path, const char *fragment_shader_file_path) {

    GLuint vertexShader = loadShader(vertex_shader_file_path, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragment_shader_file_path, GL_FRAGMENT_SHADER);

    if (vertexShader == GL_FALSE || fragmentShader == GL_FALSE) return GL_FALSE;

    if (compileShader(vertexShader) == GL_FALSE || compileShader(fragmentShader) == GL_FALSE) return GL_FALSE;

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);

    if (linkProgram(shaderProgram) == GL_FALSE) return GL_FALSE;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;

}

