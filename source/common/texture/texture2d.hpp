#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: Complete this function
            // First generate the texture on the GPU
            glGenTextures(1, &name);
            // Bind the texture (make it the active one)
            bind();
            // Set the texture parameters for wrapping in the s and t direction to be repeat (texture will repeat itself)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // Set the texture parameters for maginfying/minifying in the s and t direction to be linear interpolation (for better realstic results)
            // we are using mipmaps so when miniifying we will lineary interpolate between the two closest mipmaps, and each mipmap value is also done by linear interpolation
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            // note: mimpamp have no meaning when magnifying, so we use only GL_LINEAR
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Unbind the texture once we are done
            unbind();
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: Complete this function
            // Delete the texture
            glDeleteTextures(1, &name);
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: Complete this function
            // Bind the texture
            glBindTexture(GL_TEXTURE_2D, name);
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: Complete this function
            // Unbind the texture by binding to zero
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}