#include "material.h"

Material::Material() {
	this->shader = NULL;
	this->color = NULL;
	this->normal = NULL;

	this->ka, this->kd, this->ks = Vector3(1, 1, 1);

	this->shininess = 20;
}

Material::Material(Shader* shader, const char* texture, const char* normal) {
	this->shader = shader;
	this->color = Texture::Get(texture);
	this->normal = Texture::Get(normal);

	this->ka, this->kd, this->ks = Vector3(1, 1, 1);

	this->shininess = 20;
}

void Material::Enable(const sUniformData& uniform_data) {
	// Pass all uniforms
	this->shader->Enable();
	this->shader->SetMatrix44("u_model", *(uniform_data.model_matrix));
	this->shader->SetMatrix44("u_viewprojection", *(uniform_data.viewprojection_matrix));
	this->shader->SetTexture("u_texture", this->color);
	this->shader->SetTexture("u_normal", this->normal);
}

void Material::Disable(const sUniformData& uniform_data) {
	this->shader->Disable();
}