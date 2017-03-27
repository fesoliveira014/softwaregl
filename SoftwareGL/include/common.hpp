#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <sstream>
//#include <istream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

typedef unsigned char byte;
#ifdef _WIN32
typedef unsigned long uint;
#endif

struct bbox2d
{
	glm::vec2 min, max;

	bbox2d() : min(glm::vec2(0)), max(glm::vec2(0)) {}
	bbox2d(glm::vec2 a, glm::vec2 b) : min(a), max(b) {}
	bbox2d(float a, float b) : min(glm::vec2(a)), max(glm::vec2(b)) {}
};

struct glvertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;

	glvertex() : position(glm::vec3(0)), uv(glm::vec2(0)), normal(glm::vec3(0)) {}
	glvertex(const glm::vec3 &p, const glm::vec2 &t, const glm::vec3 &n) : position(p), uv(t), normal(n) {}
	glvertex(const glvertex & v) : position(v.position), uv(v.uv), normal(v.normal) {}
};

#endif // COMMON_H