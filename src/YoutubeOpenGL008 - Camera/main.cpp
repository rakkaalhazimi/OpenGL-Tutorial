#include<cmath>
#include<iostream>
#include<glad/glad.h>
#include<glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "EBO.h"
#include "VAO.h"
#include "VBO.h"
#include "shader.h"
#include "Texture.h"
#include "Camera.h"



const unsigned int width = 800;
const unsigned int height = 800;


int main() 
{
  // Initialize GLFW
  glfwInit();
  
  // Tell GLFW what version of OpenGL we are using
  // In this case, we are using OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Tell GLFW we are using the CORE profile
  // So that means we only have the modern functions
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // Create a GLFWwindow object of 800 by 800 pixels, naming it YoutubeOpenGL
  GLFWwindow *window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }
  
  // Introduce the windows into the current context
  glfwMakeContextCurrent(window);
  
  glfwSwapInterval(1); // Enable vsync to cap at screen refresh rate (usually 60Hz)
  
  // Load GLAD so it configures OpenGL
  gladLoadGL();
  
  // Specify the viewport of OpenGL in the Window
  // In this case the viewport goes from x=0, y=0, to x=800, y=800
  glViewport(0, 0, width, height);
  
  // Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("shader/part8.vert", "shader/part7.frag");
  
  // Vertices coordinates
  GLfloat vertices[] =
  { 
    //     COORDINATES     /        COLORS      /   TexCoord  //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	    0.0f, 0.0f, // left back
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	    5.0f, 0.0f, // left front
    0.5f, 0.0f, -0.5f,      0.83f, 0.70f, 0.44f,	    0.0f, 0.0f, // right front
    0.5f, 0.0f,  0.5f,      0.83f, 0.70f, 0.44f,	    5.0f, 0.0f, // right back
    0.0f, 0.8f,  0.0f,      0.92f, 0.86f, 0.76f,	    2.5f, 5.0f, // top
    // Bottom
    -0.5f, -0.0001f,  0.5f,     0.83f, 0.70f, 0.44f,	    0.0f, 0.0f, // left back
    -0.5f, -0.0001f, -0.5f,     0.83f, 0.70f, 0.44f,	    0.0f, 5.0f, // left front
    0.5f, -0.0001f, -0.5f,      0.83f, 0.70f, 0.44f,	    5.0f, 5.0f, // right front
    0.5f, -0.0001f,  0.5f,      0.83f, 0.70f, 0.44f,	    5.0f, 0.0f, // right back
  };

  // Indices for vertices order
  GLuint indices[] =
  {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4,
    // Bottom face
    5, 6, 8,
    7, 6, 8
  };
  
  
  // Generates Vertex Array Object and binds it
  VAO VAO1;
  VAO1.Bind();
  
  // Generates Vertex Buffer Object and links it to vertices
  VBO VBO1(vertices, sizeof(vertices));
  // Generates Element Buffer Object and links it to indices
  EBO EBO1(indices, sizeof(indices));
  
  // Links VBO attributes such as coordinates and colors to VAO
  // vertex 1       vertex 2      vertex 3
  // 0 0 0 0 0 0    0 0 0 0 0 0   0 0 0 0 0 0
  // |coor|color|   |coor|color|  |coor|color|
  // |lay0|lay1|    |lay0|lay1|   |lay0|lay1|
  // 
  // the stride is 6,
  // for layout 0, the offset is 0
  // for layout 1, the offset is 3
  VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
  VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  VAO1.LinkAttrib(VBO1, 2, 3, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  VAO1.Unbind();
  
  // Unbind all to prevent accidentally modifying them
  VAO1.Unbind();
  VBO1.Unbind();
  EBO1.Unbind();
  
  // Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
  
  // Textures
  Texture brsTexture = Texture("textures/brs.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  brsTexture.texUnit(shaderProgram, "tex0", 0);
  
  // Specify the color of the background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
  // Clean the back buffer and assign the new color to it
  glClear(GL_COLOR_BUFFER_BIT);
  // Swap the back buffer with the front buffer
  glfwSwapBuffers(window);
  
  // Variables that help the rotation of the pyramid
	float rotation = 0.0f;
	double prevTime = glfwGetTime();
  
  // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
  
  // 
  Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
  
  // Main while loop
  while (!glfwWindowShouldClose(window))
  {
    
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Clear back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram.Activate();
    
    // Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
    camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
    
    // Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
    brsTexture.Bind();
    VAO1.Bind();
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    
    // Take care of all GLFW events
    glfwPollEvents();
  }
  
  VAO1.Delete();
  VBO1.Delete();
  EBO1.Delete();
  brsTexture.Delete();
  shaderProgram.Delete();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}