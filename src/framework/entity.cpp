#include "entity.h"

Entity::Entity() {
	this->mesh = new Mesh();
	this->matrix = Matrix44();
	this->matrix.SetIdentity();
	this->texture = new Image();
}

Entity::Entity(const char * object, Matrix44 m, const char* texture, bool flipY) {
	this->mesh = new Mesh();
	this->mesh->LoadOBJ(object);
	this->matrix = m;
	this->texture = new Image();
	this->texture->LoadTGA(texture, flipY);
}

void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer,
	bool use_texture, bool use_occlusion, bool use_interpolation, bool use_wireframe
) {
	std::vector<Vector3> v = mesh->GetVertices();
	std::vector<Vector2> mesh_uvs = mesh->GetUVs();

	if (v.empty()) return; //extreme case: no vertices


	Image::sTriangleInfo triangle;

	for (int i = 0; i + 2 < v.size(); i = i + 3) { //we go by 3 because we render triangles
		// a) Transform the vertices from local to world space using the Entity’s model matrix
		Vector3 Pos[3];
		Vector2 UV[3];
		for (int k = 0; k < 3; k++) {
			Vector3 v3 = v[i + k];

			//Transform from local space to world space
			Vector4 P(v3.x, v3.y, v3.z, 1);
			Vector4 world = matrix * P; // M * P

			//projects a vector using the camera’s viewprojection matrix.
			Pos[k] = camera->ProjectVector(world.GetVector3());
			UV[k] = mesh_uvs[i + k];
		}

		// Make sure to render only the projected triangles that lay inside the cube [-1,1]^3.

		// X planes
		if (Pos[0].x < -1 && Pos[1].x < -1 && Pos[2].x < -1) continue;
		if (Pos[0].x > 1 && Pos[1].x > 1 && Pos[2].x > 1) continue;

		// Y planes
		if (Pos[0].y < -1 && Pos[1].y < -1 && Pos[2].y < -1) continue;
		if (Pos[0].y > 1 && Pos[1].y > 1 && Pos[2].y > 1) continue;

		// Z planes (THIS is your near/far plane)
		if (Pos[0].z < -1 || Pos[1].z < -1 || Pos[2].z < -1) continue; // Near
		if (Pos[0].z > 1 || Pos[1].z > 1 || Pos[2].z > 1) continue; // Far

		//Before drawing each of the triangle lines, convert the clip space positions to screen space using the framebuffer width and height.
		int space_x[3];
		int space_y[3];
		float space_z[3];
		for (int k = 0; k < 3; k++) {
			//Use your framebuffer (screen) width and height to convert the clip-space [-1, 1]
			float a = (Pos[k].x + 1) / 2;
			space_x[k] = (int)(a * (float)framebuffer->width);

			float b = (Pos[k].y + 1) / 2;
			space_y[k] = (int)(b * (float)framebuffer->height);

			space_z[k] = (Pos[k].z + 1.0f) *0.5f; // Convert z to [0, 1] range
		}

		if (use_wireframe) {
			framebuffer->DrawLineDDA(space_x[0], space_y[0], space_x[1], space_y[1], Color::GRAY);
			framebuffer->DrawLineDDA(space_x[1], space_y[1], space_x[2], space_y[2], Color::GRAY);
			framebuffer->DrawLineDDA(space_x[0], space_y[0], space_x[2], space_y[2], Color::GRAY);
		}
		else {
			triangle.p0 = Vector3(space_x[0], space_y[0], space_z[0]);
			triangle.p1 = Vector3(space_x[1], space_y[1], space_z[1]);
			triangle.p2 = Vector3(space_x[2], space_y[2], space_z[2]);

			triangle.uv0 = UV[0];
			triangle.uv1 = UV[1];
			triangle.uv2 = UV[2];

			triangle.c0 = Color::RED;
			triangle.c1 = Color::GREEN;
			triangle.c2 = Color::BLUE;

			triangle.texture = texture;

			framebuffer->DrawTriangleInterpolated(
				triangle,
				zBuffer,
				use_texture, use_occlusion, use_interpolation
			);
		}

		// framebuffer->DrawTriangle(Vector2(space_x[0], space_y[0]), Vector2(space_x[1], space_y[1]), Vector2(space_x[2], space_y[2]), c, true, c);

		
	}
}

void Entity::Update(float seconds_elapsed, Matrix44 transform_matrix) {
	this->matrix = this->matrix * transform_matrix;
}