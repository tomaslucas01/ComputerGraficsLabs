#pragma once

#include "mesh.h"
#include "image.h"

class Entity
{

public:
	Mesh* mesh;
	Matrix44 matrix;

	Entity::Entity();
	Entity::Entity(const char * object, Matrix44 m);
	void Entity::Render(Image* framebuffer, Camera* camera, const Color& c);
};