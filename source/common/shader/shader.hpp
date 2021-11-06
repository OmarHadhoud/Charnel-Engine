#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram{

    private:
        //Shader Program Handle
        GLuint program;

        // delete assignment operator and copy constructor
        // we need to delete them as the destructor of the shader program deletes the shader program in opengl, so we can't have
        // 2 shader programs having the same program in opengl, as then it will be deleted twice
        // so if we need two shaders that are the same, we have to create it twice, we can't use copy operator or assignment operator
        // as they will act as if they were pointers to one shader on the gpu
        // while they are actually one shader, so once one of them die, the other dies even if it still exists on cpu side
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;

    public:
        void create();
        void destroy();

        ShaderProgram(){ program = 0; }
        ~ShaderProgram(){ destroy(); }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use() {
            // use the shader program in opengl
            glUseProgram(this->program);
        }

        GLuint getUniformLocation(const std::string &name) {
            // get and return the location of the uniform in this program, the function takes GLChar which is char array, so call c_str()
            GLuint location = glGetUniformLocation(this->program, name.c_str());
            return location;
        }

        void set(const std::string &uniform, GLfloat value) {
            // get the location of the uniform, then set it in opengl
            GLuint location = this->getUniformLocation(uniform);
            glUniform1f(location, value);
        }

        void set(const std::string &uniform, glm::vec2 value) {
            // get the location of the uniform, then set it in opengl
            GLuint location = this->getUniformLocation(uniform);
            glUniform2f(location, value.x, value.y);
        }

        void set(const std::string &uniform, glm::vec3 value) {
            // get the location of the uniform, then set it in opengl
            GLuint location = this->getUniformLocation(uniform);
            glUniform3f(location, value.x, value.y, value.z);
        }

        void set(const std::string &uniform, glm::vec4 value) {
            // get the location of the uniform, then set it in opengl
            GLuint location = this->getUniformLocation(uniform);
            glUniform4f(location, value.x, value.y, value.z, value.w);
        }
    };

}

#endif