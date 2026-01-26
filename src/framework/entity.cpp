#include "entity.h"

Entity::Entity() {
	this->mesh = new Mesh();
	this->matrix = Matrix44();
	this->matrix.SetIdentity();
}

Entity::Entity(const char * object, Matrix44 m) {
	this->mesh = new Mesh();
	this->mesh->LoadOBJ(object);
	this->matrix = m;
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
	std::vector<Vector3> v = mesh->GetVertices();
	if (v.empty()) return; //extreme case: no vertices
	int width = framebuffer->width; //framebuffer width
	int height = framebuffer->height; //framebuffer height


	for (int i = 0; i + 2 < v.size(); i = i + 3) { //we go by 3 because we render triangles
		// a) Transform the vertices from local to world space using the Entity’s model matrix
		Vector3 Pos[3];
		for (int k = 0; k < 3; k++) {
			Vector3 v3 = v[i + k];

			//Transform from local space to world space
			Vector4 P(v3.x, v3.y, v3.z, 1);
			Vector4 world = matrix * P; // M * P

			//projects a vector using the camera’s viewprojection matrix.
			Pos[k] = camera->ProjectVector(world.GetVector3());
		}

		// Make sure to render only the projected triangles that lay inside the cube [-1,1]^3.
		if (Pos[0].x < -1 || Pos[1].x < -1 || Pos[2].x < -1 || Pos[0].x > 1 || Pos[1].x > 1 || Pos[2].x > 1) continue;
		if (Pos[0].y < -1 || Pos[1].y < -1 || Pos[2].y < -1 || Pos[0].y > 1 || Pos[1].y > 1 || Pos[2].y > 1) continue;
		if (Pos[0].z < -1 || Pos[1].z < -1 || Pos[2].z < -1 || Pos[0].z > 1 || Pos[1].z > 1 || Pos[2].z > 1) continue;

		//Before drawing each of the triangle lines, convert the clip space positions to screen space using the framebuffer width and height.
		int space_x[3];
		int space_y[3];
		for (int k = 0; k < 3; k++) {
			//Use your framebuffer (screen) width and height to convert the clip-space [-1, 1]
			float a = (Pos[k].x + 1) / 2;
			space_x[k] = (int)(a * (float)width);
			float b = (Pos[k].y + 1) / 2;
			space_y[k] = (int)(b * (float)height);
		}
		framebuffer->DrawLineDDA(space_x[0], space_y[0], space_x[1], space_y[1], c);
		framebuffer->DrawLineDDA(space_x[1], space_y[1], space_x[2], space_y[2], c);
		framebuffer->DrawLineDDA(space_x[2], space_y[2], space_x[0], space_y[0], c);

	}

}

/*void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
	std::vector<Vector3> v = mesh->GetVertices();

	for (Vector3 c : mesh->GetVertices()) {
		Vector4 p = Vector4(c.x, c.y, c.z, 1);

		p = this->matrix * p; // Project to world space

		camera->ProjectVector(p.GetVector3());

		if (p.x > 1 || p.x < -1 ||
			p.y > 1 || p.y < -1 ||
			p.z > 1 || p.z < -1) return;

		p.x = (p.x + 1.0) / 2.0;
		p.y = (p.y + 1.0) / 2.0;
		p.z = (p.z + 1.0) / 2.0;
	}
}*/