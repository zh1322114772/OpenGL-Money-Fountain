#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//location and direction for every money particle
class MoneyLoc 
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::mat4 matrix;
	float angle;
	float angle_delta;

	bool visible = false;
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	void set() 
	{
		matrix = glm::translate(glm::mat4(1.0), position);
		matrix = glm::rotate(matrix, angle, direction);
	}

};
