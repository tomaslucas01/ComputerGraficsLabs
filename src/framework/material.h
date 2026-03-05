#include "shader.h"
#include "texture.h"
#include "utils.h"

typedef struct sLight {
	Vector3 pos;
	Vector3 intensity;
} sLight;

typedef struct sUniformData {
	Matrix44 model_matrix;
	Matrix44 viewprojection_matrix;
	Vector3 eye_pos;
	Vector3 ambient_light;
	sLight scene_light;
	bool use_specular_text;
} sUniformData;

class Material {
public:
	Shader* shader;
	Texture* color;
	Texture* normal;

	Vector3 ka, kd, ks;

	int shininess;

	Material::Material();
	Material::Material(Shader* shader, const char* texture, const char* normal);

	void Material::Enable(const sUniformData& uniform_data);
	void Material::Disable(const sUniformData& uniform_data);
};