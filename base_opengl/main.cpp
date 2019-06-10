//
//  main.m
//  base_opengl
//
//  Created by Guillaume Chabane on 01/06/2019.
//  Copyright © 2019 Guillaume Chabane. All rights reserved.
//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Inlude DEVIL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

// My include
#include "shader.hpp"
#include "texture.hpp"

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // disable double buffer
    //glfwWindowHint( GLFW_DOUBLEBUFFER, GL_FALSE );
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 640, 480, "Simple sprite", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Initialise all DevIL functionality
    ilutRenderer(ILUT_OPENGL);
    ilInit();
    iluInit();
    ilutInit();
    
    // Set the clear color
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Enbale Blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    // VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Shader program
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
    // MPV uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    // TextureSampler uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    // View setup
    vec3 position(0,0,0);
    glm::mat4 Projection = ortho(0.0f,640.0f,0.0f,480.0f,-10.0f,10.0f);
    glViewport(0, 0, 640, 480);
    vec3 direction(0,0,-1);
    glm::mat4 View = lookAt(position,position + direction,vec3(0,1,0));
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 MVP = Projection * View * Model;

    // Load a texture
    GLuint Texture = loadImage("spritesheet.png");
    

    // Set vertices
    static const GLint g_vertex_buffer_data[] = {
        0, 0,
        16, 0,
        16, 16,
        16, 16,
        0, 16,
        0, 0
        ,
        16, 0,
        32, 0,
        32, 16,
        32, 16,
        16, 16,
        16, 0
        
        
        
    };
    
    // Set UVs
    static const GLfloat g_uv_buffer_data[] = {
        0.0f,0.0f,
        0.5f,0.0f,
        0.5f,0.5f,
        0.5f,0.5f,
        0.0f,0.5f,
        0.0f,0.0f
        ,
        0.0f,0.0f,
        0.5f,0.0f,
        0.5f,0.5f,
        0.5f,0.5f,
        0.0f,0.5f,
        0.0f,0.0f
        
    };
    
    // Create and generate buffers
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data),  &g_uv_buffer_data[0], GL_STATIC_DRAW);
    
    
    
    // Update loop
    do{
        
        // Move the view
        if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
            position.x -= 1;
        }
        
        if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
            position.x += 1;
        }
        
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
            position.y -= 1;
        }
        
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
            position.y += 1;
        }
        
        View = lookAt(position,position + direction,vec3(0,1,0));
        //View = glm::scale(View, vec3(2,2,2));
        
        // Update MVP uniform
        MVP = Projection * View * Model;

        // Draw
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set shader program to use
        glUseProgram(programID);
        
        // Send updated MVP uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_INT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              2,                                // size : U+V => 2
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        
        glDrawArrays(GL_TRIANGLES, 0, 4*3);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        
        // need Swap buffers if double buffer
        glfwSwapBuffers(window);
        
        // need to glFlush() if single buffer
        //glFlush();
        
        glfwPollEvents();
        
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}


