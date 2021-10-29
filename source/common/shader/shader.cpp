#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

void our::ShaderProgram::create() {
    //Create Shader Program
    program = glCreateProgram();
}

void our::ShaderProgram::destroy() {
    //Delete Shader Program
    if(program != 0) glDeleteProgram(program);
    program = 0;
}

//Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type) const {
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if(!file){
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCStr = sourceString.c_str();
    file.close();

    GLuint shaderID = glCreateShader(type);

    // send the source code to the shader
    // we pass count as 1 as we have only 1 string sent to compile,
    // we also send the address of the char * as it takes a **char as it can be an array to strings,
    // and we send the array of lengths to be nullptr as it can figure it out on its own
    glShaderSource(shaderID, 1, &sourceCStr, nullptr);
    glCompileShader(shaderID);

    // Here we check for compilation errors
    if(std::string error = checkForShaderCompilationErrors(shaderID); error.size() != 0) {
        std::cerr << "ERROR IN " << filename << std::endl;
        std::cerr << error << std::endl;
        glDeleteShader(shaderID);
        return false;
    }


    // attach the shader to the program then delete the shader
    // note that this won't delete it until it is no longer attached to any program (source: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml)
    glAttachShader(this->program, shaderID);
    glDeleteShader(shaderID);

    //We return true since the compilation succeeded
    return true;
}



bool our::ShaderProgram::link() const {
    // call opengl to link the program
    glLinkProgram(this->program);

    // Here we check for linking errors
    if(auto error = checkForLinkingErrors(program); error.size() != 0) {
        std::cerr << "LINKING ERROR" << std::endl;
        std::cerr << error << std::endl;
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader){
     //Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program){
     //Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}