//
// Created by sennin on 9/5/17.
//

#include "Shader.h"

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
    this->ID = createProgramWithShaders(vertexPath, fragmentPath);
}

Shader::Shader(GLuint programID) {
    this->ID = programID;
}

GLuint Shader::loadShader(const char * shader_file_path, GLenum SHADER_TYPE) {
    GLuint shaderID = glCreateShader(SHADER_TYPE);
    std::string shaderCode;
    std::ifstream shaderFileStream;
    shaderFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        shaderFileStream.open(shader_file_path);
        std::stringstream shaderSStream;

        shaderSStream << shaderFileStream.rdbuf();

        shaderCode = shaderSStream.str();

        shaderFileStream.close();
    } catch(std::ifstream::failure e) {
        fprintf(stdout, "We can't load shader file \"%s\".\n", shader_file_path);
        return GL_FALSE;
    }

    const char * shaderCodePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &shaderCodePointer, NULL);

    return shaderID;
}

GLuint Shader::compileShader(GLuint shaderID) {
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

GLuint Shader::linkProgram(GLuint programID) {
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

GLuint Shader::createProgramWithShaders(const char * vertex_shader_file_path, const char * fragment_shader_file_path) {

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

GLuint Shader::getID() {
    return this->ID;
}

void Shader::use() {
    glUseProgram(this->ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}