#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

class Camera 
{
private:
	int& width;
	int& height;
	double& mouseX;
	double& mouseY;

	//check if wasd keys are pressed
	bool* keyPressed;

public:
	//projection and view matrices
	glm::mat4 view;
	glm::mat4 projection;

	//camera's location, direction and up vector
	glm::vec3 location;
	glm::vec3 direction;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	Camera(int& winWidth, int& winHeight, double& x_pos, double& y_pos, bool* key):width(winWidth), height(winHeight), mouseX(x_pos), mouseY(y_pos), keyPressed(key)
	{
		location = glm::vec3(0.0f, 0.0f, 5.0f);
		direction = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	void update(const double& delta) 
	{
		float delta_f = (float)delta;

		if (mouseY > 89) 
		{
			mouseY = 89;
		}else if (mouseY < -89) 
		{
			mouseY = -89;
		}

		//set direction to cursor direction
		direction.x = cosf(glm::radians(mouseX)) * cosf(glm::radians(mouseY)) * delta_f;
		direction.y = sinf(glm::radians(mouseY)) * delta_f;
		direction.z = sinf(glm::radians(mouseX)) * cosf(glm::radians(mouseY)) * delta_f;
		direction = glm::normalize(direction);

		float speed = delta_f * 10.0f;

		//change camera's position if W, A, S, D keys are pressed
		if (keyPressed[0])
		{
			//W key 
			location += direction * speed;
		}
		if (keyPressed[1])
		{
			//A key
			location -= glm::normalize(glm::cross(direction, up)) * speed;
			
		}
		if (keyPressed[2])
		{
			//S key
			location -= direction * speed;
		}
		if (keyPressed[3])
		{
			//D key
			location += glm::normalize(glm::cross(direction, up)) * speed;
		}

		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		view = glm::lookAt(location, location + direction, up);

	}
};