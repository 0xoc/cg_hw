#include "Ray.h"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
Ray::Ray()
{
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction)
{
	o = origin;
	d = glm::normalize(direction);
}

glm::vec3 Ray::p(float t)
{
	return (o + t * d);
}
