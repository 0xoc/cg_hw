#pragma once
#include <glm/vec3.hpp>

class Ray {
public:
	glm::vec3 o;
	glm::vec3 d;

	Ray();
	Ray(glm::vec3 origin, glm::vec3 direction);
	glm::vec3 p(float t);
};