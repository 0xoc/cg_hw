#include "Ray.h"

Ray::Ray()
{
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction)
{
	o = origin;
	d = direction;
}

glm::vec3 Ray::p(float t)
{
	return (o + t * d);
}
