#include "Mesh.h"
#include "Model.h"



const unsigned int width = 800;
const unsigned int height = 800;

// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};


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
	Shader shaderProgram("shader/part13.vert", "shader/part13.frag");
	
	// Texture data
	Texture textures[]
	{
		// Texture("textures/planks.png", "diffuse", 0),
		// Texture("textures/planksSpec.png", "specular", 1)
	};
	
	std::vector<Vertex> vertVec(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> indVec(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector<Texture> texVec(textures, textures + sizeof(textures) / sizeof(Texture));
	
	Mesh myMesh(vertVec, indVec, texVec);
  
  // Assign the color, position and model of light  
  glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
  
  // Assign variables to object shader
  shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
  
  
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
  Camera camera(width, height, glm::vec3(0.0f, 0.5f, 2.0f));
	
	Model model(("models/sword/scene.gltf"));
  
  // Main while loop
  while (!glfwWindowShouldClose(window))
  {
    
    // Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		
		// Draw models
		model.Draw(shaderProgram, camera);
		myMesh.Draw(shaderProgram, camera);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
  }
  
  // Delete all the objects we've created
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
  
	return 0;
}