#include "entity.h"

Entity::Entity() {
	this->mesh = new Mesh();
	this->matrix = Matrix44();
	this->matrix.SetIdentity();
	this->material = Material();
}


Entity::Entity(const char* object, Matrix44 m, const char* texture, const char* normal, Shader* shader) {
	this->mesh = new Mesh();
	this->mesh->LoadOBJ(object);
	this->matrix = m;

	this->material.shader = shader;
	this->material.color = Texture::Get(texture);
	this->material.normal = Texture::Get(normal);
}


void Entity::Render(sUniformData& uniform_data) {
	uniform_data.model_matrix = &(this->matrix);

	this->material.Enable(uniform_data);

	this->mesh->Render();
}

void Entity::Update(float seconds_elapsed, Matrix44 transform_matrix) {
	this->matrix = this->matrix * transform_matrix;
}