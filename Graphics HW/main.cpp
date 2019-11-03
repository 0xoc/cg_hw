#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "Ray.h"
#include <glm/gtx/vector_angle.hpp>
#define Z_INF 1000

#define MOD 1	// function mode 0

inline float max(float a, float b) {
	if (a > b)
		return a;
	return b;
}

inline glm::vec3 front(glm::vec3 a, glm::vec3 b) {
	if (a.z > b.z)
		return a;

	return b;
}

bool writeToFile(std::string content, std::string path);
bool writeToPPM(std::vector<std::vector<glm::vec3>> &image, std::string path);

class Shape{
public:
	virtual bool contact(Ray &ray) = 0;
	virtual glm::vec3 contactPoint(Ray &ray) = 0;
	virtual glm::vec3 getColor(Ray &ray) = 0;
};

class Sphere : public Shape {
public:
	glm::vec3 center;
	glm::vec3 color;
	float r;
	
	Sphere(glm::vec3 icenter, float ir, glm::vec3 icolor) { center = icenter; r = ir; color = icolor; }
	
	bool contact(Ray& ray) {
		glm::vec3 oc = ray.o - center;
		float a = glm::dot(ray.d, ray.d);
		float b = 2.0 * glm::dot(oc, ray.d);
		float c = glm::dot(oc, oc) - r * r;
		float delta = b * b - 4 * a*c;
		return delta > 0;
	}

	glm::vec3 contactPoint(Ray& ray) {
		glm::vec3 oc = ray.o - center;
		float a = glm::dot(ray.d, ray.d);
		float b = 2.0 * glm::dot(oc, ray.d);
		float c = glm::dot(oc, oc) - r * r;
		float delta = b * b - 4 * a*c;
		float answer = (-b - sqrt(delta)) / 2 * a;
		return ray.p(answer);
	}

	glm::vec3 getColor(Ray& ray) {
		glm::vec3 normal = glm::normalize(contactPoint(ray) - center);
		float factor = cos(glm::angle(normal, glm::normalize(ray.d)));
		if (factor > 1)
			std::cout << factor << std::endl;
		return abs(factor) * color;
	}


};


class Triangle {
public:
	glm::vec3 a, b, c;

	Triangle(glm::vec3 aa, glm::vec3 bb, glm::vec3 cc) {
		a = aa;
		b = bb;
		c = cc;
	}

	float area() {
		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		return 0.5f * glm::length(ab) * glm::length(ac) * sin(glm::angle(ab, ac));
	}

	
};

Sphere red_s(glm::vec3(0.0, 0.0, -3.0), 1.5, glm::vec3(80, 70, 88)); // black
Sphere blue_s(glm::vec3(0.0, 0.5, -1.0), 0.5, glm::vec3(240, 222, 203)); // light
Sphere green_s(glm::vec3(0.8, -0.0, -1.5), 0.5, glm::vec3(206, 46, 108)); // red

std::vector<Shape*> shapes;


bool isOnfunction(float x, float y) {
	 return abs(y - pow(x,5)) < 0.01;
}


glm::vec3 getColor(glm::vec3 &point, glm::vec3 &ll, glm::vec3 &h, glm::vec3 &v, Ray& ray) {
	
	if (MOD == 0) {
		if (isOnfunction(point.x, point.y)) {
			return glm::vec3(255, 255, 255);
		} 
		return glm::vec3(0, 0, 0);
	}

	bool contact = false;
	float front_z = -Z_INF;
	Shape* front_shape = 0;
	for (auto shape : shapes) {
		if (shape->contact(ray)) {
			contact = true;
			glm::vec3 contactPoint = shape->contactPoint(ray);
			if (contactPoint.z > front_z) {
				front_z = contactPoint.z;
				front_shape = shape;
			}
		}
	}

	if (contact) {
		return ((Sphere*)front_shape)->getColor(ray);
	}

	float max_d = v.y - ll.y;
	float dist = v.y - point.y;
	
	glm::vec3 blue(87, 169, 154);
	glm::vec3 white(236, 244, 243);
	
	float f = 1.0f - (dist / max_d);

	glm::vec3 color =  f * blue + (1-f) * white;
	return color;

}

int main(void) {

	int w = 600;
	int y = 300;
	shapes.push_back(&red_s);
	shapes.push_back(&blue_s);
	shapes.push_back(&green_s);

	glm::vec3 ll(-2, -1, -1);
	glm::vec3 width(4, 0, 0);
	glm::vec3 height(0, 2, 0);
	glm::vec3 origin(0.0f, 0.0f, 0.0f);

	std::vector<std::vector<glm::vec3>> image(w);
	float r, g, b, u, v;

	for (int j = 0; j < w; j++) {
		for (int i = 0; i < y; i++) {
			u = (float)j / (float)w;
			v = (float)(y - i) / (float)y;
			glm::vec3 point = ll + u * width + v * height;
			Ray ray(origin, ll + u * width + v * height);

			image[j].push_back(getColor(point,ll,width, height, ray));

		}
	}

	writeToPPM(image, "image.ppm");

	system("pause");
	return 0;
}


bool writeToPPM(std::vector<std::vector<glm::vec3>> &image, std::string path) {
	std::stringstream img_string;
	img_string << "P3" << std::endl << image.size() << " " << image[0].size() << std::endl << 255 << std::endl;

	for (int i = 0; i < image[0].size(); i++) {
		for (int j = 0; j < image.size(); j++) {
			if (j != 0)
				img_string << " ";
			img_string << int(image[j][i].r) << " " << int(image[j][i].g) << " " << int(image[j][i].b);
		}
		if (i != image[0].size() - 1)
			img_string << std::endl;
	}

	return writeToFile(img_string.str(), path);

}

bool writeToFile(std::string content, std::string path) {
	std::ofstream fileHandle;
	fileHandle.open(path.c_str());
	if (!fileHandle.is_open()) {
		std::cout << "Could not open file \"" << path << "\"" << std::endl;
		return false;
	}
	fileHandle << content;
	fileHandle.close();
	std::cout << "file written" << std::endl;
	return true;
}
