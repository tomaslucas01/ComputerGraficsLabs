#pragma once

#include "mesh.h"
#include "image.h"
#include "material.h"

class Entity
{

public:
	Mesh* mesh;
	Matrix44 matrix;
	Material material;

	Entity::Entity();
	// Entity::Entity(const char * object, Matrix44 m, const char * texture, bool flipY);
	Entity::Entity(const char* object, Matrix44 m, const char* texture, const char* normal, Shader* shader);

	/*void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer,
		bool use_texture, bool use_occlusion, bool use_interpolation, bool use_wireframe
	);
	void Entity::Render(Camera* camera);*/

	void Entity::Render(sUniformData& uniform_data);
	void Entity::Update(float seconds_elapsed, Matrix44 transform_matrix);
};