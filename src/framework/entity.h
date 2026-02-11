#pragma once

#include "mesh.h"
#include "image.h"

class Entity
{

public:
	Mesh* mesh;
	Matrix44 matrix;
	Image* texture;

	Entity::Entity();
	Entity::Entity(const char * object, Matrix44 m, const char * texture, bool flipY);
	void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer, bool use_texture, bool use_occlusion, bool use_interpolation);
	void Entity::Update(float seconds_elapsed, Matrix44 transform_matrix);
};