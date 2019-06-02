//
//  texture.cpp
//  mygame
//
//  Created by Chabane Guillaume on 01/03/2018.
//  Copyright © 2018 Chabane Guillaume. All rights reserved.
//
#define GL_GENERATE_MIPMAP_SGIS 0x8191
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

// Include OpenGL
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

// Include GLFW
//#include "GLFW/glfw3.h"

//#include <GL/glew.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){
    
    unsigned char header[124];
    
    FILE *fp;
    
    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
        return 0;
    }
    
    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }
    
    /* get the surface desc */
    fread(&header, 124, 1, fp);
    
    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width         = *(unsigned int*)&(header[12]);
    unsigned int linearSize     = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);
    
    
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);
    
    //unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);
            return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,2);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    
    
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

        offset += size;
        width  /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;

    }
    
    free(buffer);
    
    return textureID;
    
    
}


// Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
GLuint loadImage(const char * theFileName)
{
    ILuint imageID;                // Create an image ID as a ULuint
    
    GLuint textureID;            // Create a texture ID as a GLuint
    
    ILboolean success;            // Create a flag to keep track of success/failure
    
    //ILenum error;                // Create a flag to keep track of the IL error state
    
    ilGenImages(1, &imageID);         // Generate the image ID
    
    ilBindImage(imageID);             // Bind the image
    
    success = ilLoadImage(theFileName);     // Load the image file
    
    // If we managed to load the image, then we can start to do things with it...
    if (success)
    {
        
        // If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
        ILinfo ImageInfo;
        iluGetImageInfo(&ImageInfo);
        if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
        {
            iluFlipImage();
        }
        // Convert the image into a suitable format to work with
        // NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        
        // Quit out if we failed the conversion
        if (!success)
        {
            //error = ilGetError();
            // std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
            exit(-1);
        }
        
        // Generate a new texture
        glGenTextures(1, &textureID);
        
        glActiveTexture(GL_TEXTURE0);
        
        // Bind the texture to a name
        glBindTexture(GL_TEXTURE_2D, textureID);
        
//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // Set texture interpolation method to use linear interpolation (no MIPMAPS)
        //glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glGenerateMipmap(GL_TEXTURE_2D);
        
        // Specify the texture specification
        glTexImage2D(GL_TEXTURE_2D,                 // Type of texture
                     0,                // Pyramid level (for mip-mapping) - 0 is the top level
                     ilGetInteger(IL_IMAGE_FORMAT),    // Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
                     ilGetInteger(IL_IMAGE_WIDTH),    // Image width
                     ilGetInteger(IL_IMAGE_HEIGHT),    // Image height
                     0,                // Border width in pixels (can either be 1 or 0)
                     ilGetInteger(IL_IMAGE_FORMAT),    // Format of image pixel data
                     GL_UNSIGNED_BYTE,        // Image data type
                     ilGetData());            // The actual image data itself
//        std::cout<<ilGetInteger(IL_IMAGE_WIDTH)<<std::endl;
//        std::cout<<theFileName<<std::endl;
    }
    
    else // If we failed to open the image file in the first place...
    {
        //error = ilGetError();
        //std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
        exit(-1);
    }
    
    ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.
    
    //std::cout << "Texture creation successful." << std::endl;
    
    return textureID; // Return the GLuint to the texture so you can use it!
}
