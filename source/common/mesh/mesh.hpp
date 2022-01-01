#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc

            // Generate vertex array object and bind it
            // We use vertex array object to define how the data is interpreted (attributes)
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // Generate vertex buffer, bind it and copy the vertex data to the VRAM
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // Use static draw as we won't change the data of the buffer
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            // Generate element buffer, bind it and copy the element data to the VRAM
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // Use static draw as we won't change the data of the buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);

            // Enable the vertex attributes array, so that they can be accessed during rendering
            // when we call vertex array commands (DrawArrays, DrawElements, etc)
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);

            // Specify the layout of the vertex data
            // params: attribute index, number of components, data type, normalized, stride, offset
            // note: for color, we use unsigned byte as we store the color as one packed value consisting of 8 bits(1 byte) for every channel
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            // Unbind the vertex array object (just to make sure the code following this func doesn't overwrite the vertex array object)
            glBindVertexArray(0);
            // Unbind the buffers (just to make sure we don't accidentally overwrite the buffers in code following this function)
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // Update the element count to be used when drawing
            elementCount = GLsizei(elements.size());
        }

        // this function should render the mesh
        void draw()
        {
            //TODO: Write this function

            // Bind the vertex array object that will be used for drawing the mesh
            glBindVertexArray(VAO);
            // Draw the mesh used indexed drawing
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
            // Unbind the vertex array object
            glBindVertexArray(0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh()
        {
            //TODO: Write this function
            // Delete the vertex buffer as we're done with the mesh and no longer need the data
            glDeleteBuffers(1, &VBO);
            // Delete the element buffer as we're done with the mesh and no longer need the data
            glDeleteBuffers(1, &EBO);
            // Delete the vertex array object as we're done with the mesh and no longer need it
            glDeleteVertexArrays(1, &VAO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}