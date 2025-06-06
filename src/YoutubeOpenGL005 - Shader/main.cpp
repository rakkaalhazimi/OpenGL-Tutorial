#include<cmath>
#include<iostream>
#include<glad/glad.h>
#include<glfw3.h>

#include "EBO.h"
#include "VAO.h"
#include "VBO.h"
#include "shader.h"



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
  GLFWwindow *window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }
  
  // Introduce the windows into the current context
  glfwMakeContextCurrent(window);
  
  // Load GLAD so it configures OpenGL
  gladLoadGL();
  
  // Specify the viewport of OpenGL in the Window
  // In this case the viewport goes from x=0, y=0, to x=800, y=800
  glViewport(0, 0, 800, 800);
  
  // Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("shader/part5.vert", "shader/part5.frag");
  
  // Vertices coordinates
	GLfloat vertices[] =
	{
		//               COORDINATES                  /     COLORS           //
    -0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
    0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
    0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
    -0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
    0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
    0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
	};
  
  GLuint indices[] = 
  {
    0, 3, 5, // Lower left triangle
    3, 2, 4, // Lower right triangle
    5, 4, 1, // Upper triangle
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
  VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
  VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  VAO1.Unbind();
  
  // Unbind all to prevent accidentally modifying them
  VAO1.Unbind();
  VBO1.Unbind();
  EBO1.Unbind();
  
  // Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
  
  // Specify the color of the background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
  // Clean the back buffer and assign the new color to it
  glClear(GL_COLOR_BUFFER_BIT);
  // Swap the back buffer with the front buffer
  glfwSwapBuffers(window);
  
  
  // Main while loop
  while (!glfwWindowShouldClose(window))
  {
    
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram.Activate();
    // Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
    VAO1.Bind();
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    
    // Take care of all GLFW events
    glfwPollEvents();
  }
  
  VAO1.Delete();
  VBO1.Delete();
  EBO1.Delete();
  shaderProgram.Delete();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}