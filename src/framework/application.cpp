#include "application.h"
#include "mesh.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);
	this->framebuffer.Resize(w, h);
	this->zBuffer.Resize(w, h);

	// Our inits
	mode = 1;
	mouse_left_pressed = false;
	mouse_right_pressed = false;

	this->camera = Camera();

	eye = Vector3(0, 0, 5);
	center = Vector3(0, 0, 0);
	up = Vector3(0, 1, 0);

	radius = (eye - center).Length();

	this->camera.SetAspectRatio(float(width) / float(height));

	//WE USE THIS LINE OF CODE TO KNOW WHERE IS THE CENTER AND OUR ANGLE WITH THE CENTRE
	Vector3 dir = (eye - center).Normalize();
	angle = atan2(dir.z, dir.x);
	pitch = asin(dir.y);


	// Perspective
	curr_property = 1; //Initialize with camera near
	p_near = 0.5f;
	p_far = 10.0f;
	fov = 60.0f;
	this->camera.LookAt(eye, center, up);
	this->camera.SetPerspective(fov, this->camera.aspect, p_near, p_far);

	
	// Quad render

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// Lights

	ambient_light = Vector3(0.1, 0.05, 0.02);

	sLight red_light;
	red_light.intensity = Vector3(6, 0, 0);
	red_light.pos = Vector3(2.5, 2, 2);

	sLight green_light;
	green_light.intensity = Vector3(0, 6, 0);
	green_light.pos = Vector3(-2.5, -0.5, 2);

	sLight blue_light;
	blue_light.intensity = Vector3(0, 0, 6);
	blue_light.pos = Vector3(0, 1, 2);

	scene_lights.push_back(red_light);
	scene_lights.push_back(green_light);
	scene_lights.push_back(blue_light);

	nLights = scene_lights.size();


	// Load shaders

	this->gouraud_shader = Shader::Get("../res/shaders/gouraud.vs", "../res/shaders/gouraud.fs");
	this->phong_shader = Shader::Get("../res/shaders/phong.vs", "../res/shaders/phong.fs");


	// Quad init

	Texture* text = Texture::Get("../res/images/fruits.png");
	this->quad_shader = Shader::Get("../res/shaders/quad.vs", "../res/shaders/quad.fs");
	this->quad_shader->SetTexture("u_texture", text);

	this->mesh = new Mesh();
	this->mesh->CreateQuad();

	// Entities init

	Matrix44 anna_m = Matrix44();
	int scale = 6;
	anna_m.MakeScaleMatrix(scale, scale, scale);
	
	Matrix44 cleo_m = anna_m;
	Matrix44 lee_m = anna_m;

	anna_m.M[3][1] = -1;

	cleo_m.M[3][0] = -3.5;
	cleo_m.M[3][1] = -1; 

	lee_m.M[3][0] = 3.5;
	lee_m.M[3][1] = -1;

	Entity anna = Entity("../res/meshes/anna.obj", anna_m, "../res/textures/anna_color_specular.tga", "../res/textures/anna_normal.tga", this->phong_shader);
	Entity cleo = Entity("../res/meshes/cleo.obj", cleo_m, "../res/textures/cleo_color_specular.tga", "../res/textures/cleo_normal.tga", this->phong_shader);
	Entity lee = Entity("../res/meshes/lee.obj", lee_m, "../res/textures/lee_color_specular.tga", "../res/textures/lee_normal.tga", this->phong_shader);

	entities.push_back(anna);
	entities.push_back(cleo);
	entities.push_back(lee);

}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	for (int i = 0; i < entities.size(); ++i) {
		entities[i].material.shader->SetInt("use_specular_text", (use_specular_text ? 1 : 0));
	}
}


// Render one frame
void Application::Render(void)
{
	// Quad render
	/*shader->Enable();
	shader->SetFloat("u_time", time);
	mesh->Render();
	shader->Disable();*/

	// Entity render
	// shader->Enable();

	// entities[0].Render(&camera);

	// shader->Disable();

	uniform_data.viewprojection_matrix = camera.viewprojection_matrix;
	uniform_data.eye_pos = eye;

	for (int i = 0; i < nLights; ++i) {
		if (i == 0) {
			glDisable(GL_BLEND);
		}
		else {
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		}
		for (int j = 0; j < entities.size(); ++j) {
			uniform_data.model_matrix = entities[j].matrix;
			uniform_data.ambient_light = (i == 0 ? ambient_light : Vector3(0)); // Just render ambient on first light pass
			uniform_data.scene_light = scene_lights[i];
			uniform_data.use_specular_text = use_specular_text;
			entities[j].Render(uniform_data);
		}
	}
	
	
}


void Application::Update(float seconds_elapsed)
{
	// Lab 3

	/*
	// movement
	float speed = 5 * seconds_elapsed;

	// Get camera-local axes in world space
	Vector3 right = camera.GetLocalVector(Vector3(1, 0, 0));    // right
	Vector3 upp = camera.GetLocalVector(Vector3(0, 1, 0));		// up
	Vector3 forward = camera.GetLocalVector(Vector3(0, 0, -1)); // forward


	//We put this in update so we don't have to spam click buttons
	if (keystate[SDL_SCANCODE_Q]) { eye = eye + upp * speed; center = center + upp * speed; } // up (camera moves down)
	if (keystate[SDL_SCANCODE_A]) { eye = eye - right * speed; center = center - right * speed; } // left (camera moves left)
	if (keystate[SDL_SCANCODE_E]) { eye = eye - upp * speed; center = center - upp * speed; } // down (camera moves up)
	if (keystate[SDL_SCANCODE_D]) { eye = eye + right * speed; center = center + right * speed; } // right (camera moves right)

	// if (keystate[SDL_SCANCODE_W]) { eye = eye + forward * speed; center = center + forward * speed; };
	// if (keystate[SDL_SCANCODE_S]) { eye = eye - forward * speed; center = center - forward * speed; };




	camera.LookAt(eye, center, up);

	// this->camera.SetAspectRatio(float(window_width) / float(window_height));
	// this->camera.SetOrthographic(-orthographic_size * camera.aspect, orthographic_size * camera.aspect, orthographic_size, -orthographic_size, -10.f, 10.f); // Important to not stretch!


	if (mode == 2) {
		
	}*/
}


//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// Lab 3


	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break;
	case SDLK_PLUS: {
		switch (curr_property) {
		case 1: {
			if (p_near < (p_far - 0.5f)) {
				p_near += 0.5f;
			}
			break;
		}
		case 2: {
			p_far += 0.5;
			break;
		}
		case 3: {
			if (fov < 170) {
				fov += 5;
			}
			break;
		}
		}
		this->camera.SetAspectRatio(float(window_width) / float(window_height));
		this->camera.SetPerspective(fov, this->camera.aspect, p_near, p_far);
		this->camera.LookAt(eye, center, up);

		break;
	}
	case SDLK_MINUS: {
		if (curr_property == 1) {
			p_near -= 0.5f;
			if (p_near < 0.05f) p_near = 0.05f;
			if (p_near >= p_far - 0.05f) p_near = p_far - 0.05f;
		}
		else if (curr_property == 2) {
			p_far -= 0.5f;
			if (p_far <= p_near) p_far = p_near + 1;
		}
		else {
			if (fov > 0) fov -= 5;	//Avoid faces turning upside down
		}
		this->camera.SetAspectRatio(float(window_width) / float(window_height));
		this->camera.SetPerspective(fov, this->camera.aspect, p_near, p_far);
		this->camera.LookAt(eye, center, up);
		break;
	}
	case SDLK_1: {
		
		if (shader_type == QUAD) {
			// A
		}
		else {
			if (shader_type == PHONG) {
				nLights = 1;
			}
		}
		break;
	}
	case SDLK_2: {
		if (shader_type == QUAD) {
			// A
		}
		else {
			if (shader_type == PHONG) {
				nLights = 2;
			}
		}
		break;
	}
	case SDLK_3: {
		if (shader_type == QUAD) {
			// A
		}
		else {
			if (shader_type == PHONG) {
				nLights = 3;
			}
		}
		break;
	}
	case SDLK_n: {
		std::cout << "Current Property = Camera Near\n";
		curr_property = 1;
		break;
	}
	case SDLK_g: {
		for (int i = 0; i < entities.size(); ++i) {
			entities[i].material.shader = gouraud_shader;
		}
		break;
	}
	case SDLK_p: {
		for (int i = 0; i < entities.size(); ++i) {
			entities[i].material.shader = phong_shader;
		}
		break;
	}
	case SDLK_f: {
		this->quad_shader->SetFloat("mode", 1.0);
		std::cout << "Change!";
		break;
	}
	case SDLK_s: {
		use_specular_text = !use_specular_text;
		break;
	}
	case SDLK_v: {
		std::cout << "Current Property = Camera FOV\n";
		curr_property = 3;
		break;
	}
	case SDLK_t: {
		use_texture = !use_texture;
		break;
	}
	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) mouse_left_pressed = true;

	if (event.button == SDL_BUTTON_RIGHT) mouse_right_pressed = true;
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) mouse_left_pressed = false;

	if (event.button == SDL_BUTTON_RIGHT) mouse_right_pressed = false;
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	if (mouse_state != 0) {

		angle -= mouse_delta.x / (window_width * 0.1f);
		pitch -= mouse_delta.y / (window_height * 0.1f);

		if (angle > PI) angle -= 2 * PI;
		else if (angle < -PI) angle += 2 * PI;

		if (pitch > PI) pitch -= 2 * PI;
		else if (pitch < -PI) pitch += 2 * PI;


		if (mouse_left_pressed) {

			eye.x = center.x + cos(angle) * radius * cos(pitch);
			eye.y = center.y + radius * sin(pitch);
			eye.z = center.z + sin(angle) * radius * cos(pitch);

			// Up vector is tangent to orbit "sphere", so it is the derivative of pitch
			up = Vector3(
				-sin(pitch) * cos(angle),
				cos(pitch),
				-sin(pitch) * sin(angle)
			);
		}
		else if (mouse_right_pressed) {

			float invAngle = angle + PI;
			float invPitch = -pitch;

			center.x = eye.x + cos(invAngle) * radius * cos(invPitch);
			center.y = eye.y + radius * sin(invPitch);
			center.z = eye.z + sin(invAngle) * radius * cos(invPitch);

			up = Vector3(
				-sin(invPitch) * cos(invAngle),
				cos(invPitch),
				-sin(invPitch) * sin(invAngle)
			);
		}
		camera.LookAt(eye, center, up);
	}
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	if (dy > 0)
		radius -= 0.5f;   // zoom in
	else if (dy < 0)
		radius += 0.5f;   // zoom out

	radius = std::max(radius, 0.5f); //we don't want to be inside the faces

	eye.x = center.x + cos(angle) * radius;
	eye.z = center.z + sin(angle) * radius;

	camera.LookAt(eye, center, up);
}


void Application::OnFileChanged(const char* filename)
{
	Shader::ReloadSingleShader(filename);
}