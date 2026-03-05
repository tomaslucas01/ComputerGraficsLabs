#include "material.h"

Material::Material() {
	this->shader = NULL;
	this->color = NULL;
	this->normal = NULL;

	this->ka = Vector3(1, 1, 1); 
	this->kd = Vector3(1, 1, 1);
	this->ks = Vector3(1, 1, 1);

	this->shininess = 20;
}

Material::Material(Shader* shader, const char* texture, const char* normal) {
	this->shader = shader;
	this->color = Texture::Get(texture);
	this->normal = Texture::Get(normal);

	this->ka = Vector3(0.8);
	this->kd = Vector3(0.7);
	this->ks = Vector3(1.0);

	this->shininess = 80;
}

void Material::Enable(const sUniformData& uniform_data) {
	// Pass all uniforms
	this->shader->Enable();
	this->shader->SetMatrix44("u_model", uniform_data.model_matrix);
	this->shader->SetMatrix44("u_viewprojection", uniform_data.viewprojection_matrix);
	this->shader->SetVector3("eye_pos", uniform_data.eye_pos);

	this->shader->SetTexture("u_texture", this->color);
	this->shader->SetTexture("u_normal", this->normal);

	this->shader->SetInt("shininess", this->shininess);
	this->shader->SetVector3("k_ambient", this->ka);
	this->shader->SetVector3("k_diffuse", this->kd);
	this->shader->SetVector3("k_specular", this->ks);

	this->shader->SetVector3("ambient_intensity", uniform_data.ambient_light);

	this->shader->SetVector3("light_intensity", uniform_data.scene_light.intensity);
	this->shader->SetVector3("light_pos", uniform_data.scene_light.pos);

	this->shader->SetInt("use_specular_text", uniform_data.use_specular_text ? 1 : 0);
}

void Material::Disable(const sUniformData& uniform_data) {
	this->shader->Disable();
}