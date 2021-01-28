#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.hpp"
#include "ShaderProgram.hpp"
#include "MoneyLoc.hpp"
#include "Camera.hpp"
#include <queue>
#include <stdlib.h>
#include <time.h> 

#include <thread>

#define TOTAL_PARTICLES 100000
#define SPREAD_MIN_ANGLE 70.0
#define SPREAD_MAX_ANGLE 90.0
#define MIN_ACCELERATION 10.0
#define MAX_ACCELERATION 15.0
#define GRAVITY -9.8
#define PI 3.1415926
#define MIN_ROTATE_DELTA 0.1
#define MAX_ROTATE_DELTA 3
//in second
#define PARTICLE_SPAWN_INTERVAL 0.00001
#define GROUND -100.0

int windowWidth = 1024;
int windowHeight = 768;
double mousex = -90;
double mousey = 0;
bool keyPressed[4] = {false};

//glfw window
GLFWwindow* window = nullptr;

namespace vertex
{
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	float vertices[] =
	{
		//face 1
		-0.5f, 0.25f, 0.0001f, 0.0f, 1.0f,
		0.5f, 0.25f, 0.0001f, 1.0f, 1.0f,
		-0.5f, -0.25f, 0.0001f, 0.0f, 0.5f,
		0.5f, -0.25f, 0.0001f, 1.0f, 0.5f,

		//face 2
		-0.5f, 0.25f, -0.0001f, 0.0f, 0.5f,
		0.5f, 0.25f, -0.0001f, 1.0f, 0.5f,
		-0.5f, -0.25f, -0.0001f, 0.0f, 0.0f,
		0.5f, -0.25f, -0.0001f, 1.0f, 0.0f,
	};

	unsigned int elements[] = 
	{
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
	};

	//bind EBO VBO and VAO 
	void loadData()
	{
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		//bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//set vertex attributes for vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //vertex location
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));// texture coordinate
		//enable attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		//bind EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	}

}

void windowSizeChangedCallback(GLFWwindow* window, int width, int height) 
{
	//change render area
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

void mousePostitionChangedCallback(GLFWwindow* window, double xpos, double ypos) 
{
	static double last_xpos = xpos;
	static double last_ypos = ypos;

	mousex += (xpos - last_xpos) * 0.1;
	mousey -= (ypos - last_ypos) * 0.1;

	last_xpos = xpos;
	last_ypos = ypos;
}

inline float randomFloat(float minRange, float maxRange) 
{
	return minRange + (((float)rand() / RAND_MAX) * (maxRange - minRange));
}

//display current FPS every second on the window title
inline void showFPS(const double& delta) 
{
	static double oneSecond = 0;
	static int frameCounter = 0;

	oneSecond += delta;
	frameCounter++;
	
	if (oneSecond > 1) 
	{
		glfwSetWindowTitle(window, ("Money Fountain FPS:" + std::to_string(frameCounter)).c_str());
		frameCounter = 0;
		oneSecond = 0;
	}

}

//process keyboard events
inline void keyboardEventProcess(const double& delta) 
{
	//if ESC is pressed, then exit program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, true);
	}
	else 
	{
		//update keyPressed array
		keyPressed[0] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		keyPressed[1] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		keyPressed[2] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		keyPressed[3] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

	}

}

int main(void) 
{
	
	//init glfw
	glfwInit();

	//set opengl verison 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//setup glfw window
	window = glfwCreateWindow(windowWidth, windowHeight, "Money Fountain", NULL, NULL);
	if (window == nullptr) 
	{
		//failed to create window
		std::cout << "Unable to create the Window" << std::endl;
		return -1;
	}

	//setup opengl context
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, windowWidth, windowHeight);
	
	//callbacks
	glfwSetFramebufferSizeCallback(window, windowSizeChangedCallback);
	glfwSetCursorPosCallback(window, mousePostitionChangedCallback);

	//time interval between two frames
	double delta_t = 0;
	double time_now = glfwGetTime();
	double time_last = time_now;

	//enable depth buffer, set background color and set input mouse
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//init shader program and load Texture to vram
	ShaderProgram shader;
	Texture texture;
	try
	{
		shader.Init("data\\shaders\\vertex.vert", "data\\shaders\\fragment.vert");
		texture.Init("data\\100dollar.jpg");
	}
	catch (std::runtime_error e)
	{
		std::cout << "Error message: " << e.what() << std::endl;
		return -1;
	}

	shader.use();
	//bind texture 
	glUniform1i(shader.getUniformLocation("tex0"), 0);

	//load vertex data to vram
	vertex::loadData();

	//camera and object location matrices
	MoneyLoc test;
	test.direction = glm::vec3(0.5f, 0.0f, 0.0f);
	test.angle = 40;
	test.position = glm::vec3(0.0f, 0.0f, -5.0f);
	test.set();

	//particles
	MoneyLoc *particles = new MoneyLoc[TOTAL_PARTICLES];
	std::queue<unsigned int> recycleList;
	double particleSpawnCounter = 0;

	//put all available particles in recycle queue
	for (int i = 0; i < TOTAL_PARTICLES; i++) 
	{
		recycleList.push(i);
	}

	//camera
	Camera camera(windowWidth, windowHeight, mousex, mousey, keyPressed);

	//set random seed
	srand(time(NULL));

	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
		//calculate delta t
		time_now = glfwGetTime();
		delta_t = time_now - time_last;
		time_last = time_now;

		//input event process
		keyboardEventProcess(delta_t);

		//render body
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.getID());
		glBindVertexArray(vertex::VAO);

		//update camera matrices
		camera.update(delta_t);
		glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(camera.view));
		glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));

		//spawn new particles
		particleSpawnCounter += delta_t;
		if ((particleSpawnCounter > PARTICLE_SPAWN_INTERVAL)) 
		{
			while ((particleSpawnCounter > 0) && !recycleList.empty())
			{
				unsigned int i = recycleList.front();
				recycleList.pop();

				particles[i].visible = true;
				particles[i].position = glm::vec3(0.0f, 0.0f, 0.0f);
				particles[i].velocity = glm::vec3(0.0f, 0.0f, 0.0f);

				//set initial acceleration
				float x_rad = randomFloat(0, 2 * PI);
				float y_rad = randomFloat(glm::radians(SPREAD_MIN_ANGLE), glm::radians(SPREAD_MAX_ANGLE));

				particles[i].acceleration.x = cosf(x_rad) * cosf(y_rad);
				particles[i].acceleration.y = sinf(y_rad);
				particles[i].acceleration.z = sinf(x_rad) * cosf(y_rad);
				particles[i].acceleration *= randomFloat(MIN_ACCELERATION, MAX_ACCELERATION);

				//random rotate
				particles[i].direction = glm::vec3(randomFloat(-1, 1), randomFloat(-1, 1), randomFloat(-1, 1));
				particles[i].angle = randomFloat(0, 2 * PI);
				particles[i].angle_delta = randomFloat(MIN_ROTATE_DELTA, MAX_ROTATE_DELTA);
				particleSpawnCounter -= PARTICLE_SPAWN_INTERVAL;
			}
		}

		//update particle position
		for (unsigned int i = 0; i < TOTAL_PARTICLES; i++) 
		{
			if (particles[i].visible) 
			{
				//if the particle hit the ground then put it in the recycle queue
				if (particles[i].position.y < GROUND) 
				{
					particles[i].visible = false;
					recycleList.push(i);
				}
				else 
				{
					//update particle position
					particles[i].acceleration.y += GRAVITY * (float)delta_t;
					particles[i].velocity = particles[i].acceleration * (float)delta_t;
					particles[i].angle += particles[i].angle_delta * (float)delta_t;
					particles[i].position += particles[i].velocity;
					particles[i].set();
					glUniformMatrix4fv(shader.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(particles[i].matrix));
					glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
				}
			
			}
		
		}


		//show buffer
		glfwSwapBuffers(window);
		glfwPollEvents();

		//show FPS
		showFPS(delta_t);
	}
	glfwTerminate();
	return 0;

}