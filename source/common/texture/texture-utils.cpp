#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

glm::ivec2 our::texture_utils::loadImage(Texture2D& texture, const char *filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* data = stbi_load(filename, &size.x, &size.y, &channels, 4);
    if(data == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return {0, 0};
    }
    //Bind the texture such that we upload the image data to its storage
    //TODO: Finish this function
    //HINT: The steps should be as follows: bind the texture, send the pixel data to the GPU, then generate the mipmap (if requested).

    // first bind the texture so that subsequent calls to glTexImage2D will upload the image data to the texture on the GPU
    texture.bind();

    // send the image data to the GPU, first parameter is the target(here it is 2D texture we already binded)
    // second parameter is the mipmap level, 0 is the base level (we don't want to set mipmap levels manually, we'll generate them later)
    // third parameter is the internal format of the texture (on GPU) (number of color components, 4 each of 8 bits)
    // fourth & fifth parameters are the size of the image (width, height)
    // sixth parameter is the border color (always equal to zero)
    // seventh parameter is the format of the pixel data we have (we read the image in RGBA format)
    // eighth parameter is the type of the pixel data we have (we read the image as unsigned char, so we use GL_UNSIGNED_BYTE)
    // ninth parameter is the data of the image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //Generate the mipmap if requested
    if(generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    // unbind the texture once we are done
    texture.unbind();

    stbi_image_free(data); //Free image data after uploading to GPU
    return size;
}