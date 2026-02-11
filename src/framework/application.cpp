#include "application.h"
#include "mesh.h"
#include "shader.h"
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


	/*this->camera.SetOrthographic(-orthographic_size * camera.aspect, orthographic_size * camera.aspect, orthographic_size, -orthographic_size, -10.f, 10.f); // Important to not stretch!

	Matrix44 anna_m = Matrix44();
	int scale = 20;
	anna_m.MakeScaleMatrix(scale, scale, scale);
	anna_m.M[3][1] = -3;

	Matrix44 cleo_m = anna_m;
	Matrix44 lee_m = anna_m;

	anna_m.M[3][0] = -6;
	lee_m.M[3][0] = 6;*/



	// Perspective
	curr_property = 1; //Initialize with camera near
	p_near = 2.0f;
	p_far = 10.0f;
	fov = 60.0f;
	this->camera.LookAt(eye, center, up);
	this->camera.SetPerspective(fov, this->camera.aspect, p_near, p_far);

	Matrix44 anna_m = Matrix44();
	int scale = 6;
	anna_m.MakeScaleMatrix(scale, scale, scale);
	anna_m.M[3][1] = -3;

	Matrix44 cleo_m = anna_m;
	Matrix44 lee_m = anna_m;

	anna_m.M[3][0] = -2;
	anna_m.M[3][1] = -1;

	cleo_m.M[3][1] = -1;
	cleo_m.M[3][2] = -2;

	lee_m.M[3][0] = 2;
	lee_m.M[3][1] = -1;
	lee_m.M[3][2] = -4;

	Entity anna = Entity("../res/meshes/anna.obj", anna_m, "../res/textures/anna_color_specular.tga", true);
	Entity cleo = Entity("../res/meshes/cleo.obj", cleo_m, "../res/textures/cleo_color_specular.tga", true);
	Entity lee = Entity("../res/meshes/lee.obj", lee_m, "../res/textures/lee_color_specular.tga", true);

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

	framebuffer.Fill(Color::WHITE);
}


// Render one frame
void Application::Render(void)
{
	framebuffer.Fill(Color::WHITE);
	zBuffer.Fill(1.0);

	if (mode == 1) {
		entities[0].Render(&framebuffer, &camera, &zBuffer, use_texture, use_occlusion, use_interpolation, use_wireframe);
	}

	else if (mode == 2) {
		entities[0].Render(&framebuffer, &camera, &zBuffer, use_texture, use_occlusion, use_interpolation, use_wireframe);
		entities[1].Render(&framebuffer, &camera, &zBuffer, use_texture, use_occlusion, use_interpolation, use_wireframe);
		entities[2].Render(&framebuffer, &camera, &zBuffer, use_texture, use_occlusion, use_interpolation, use_wireframe);
	}

	// Uncomment to see z-buffer
	// framebuffer.DrawImage(zBuffer, 0, 0);

	framebuffer.Render();
}


void Application::Update(float seconds_elapsed)
{
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

	Matrix44 m;
	m.MakeRotationMatrix(seconds_elapsed, Vector3(0, 1, 0));
	entities[0].Update(seconds_elapsed, m);


	if (mode == 2) {
		if (int(time) % 3 == 0 && int(time) != last_scale_trigger) { // Every three seconds (and also check if not same instant where int(time) % 3 == 0
			scale_growing = !scale_growing; // Change scale mode (grow or decreasse)
			last_scale_trigger = int(time); // Update last trigger
		}
		float growth_rate = 1.25f;
		float scale_factor;

		if (scale_growing) {
			scale_factor = pow(growth_rate, seconds_elapsed);
			
		}
		else {
			scale_factor = pow(1.0f / growth_rate, seconds_elapsed);
		}
		m.MakeScaleMatrix(scale_factor, scale_factor, scale_factor);
		entities[1].Update(seconds_elapsed, m);

		float amplitude = 0.25f;
		float frequency = 3.0f;

		float current = amplitude * sin(frequency * time);
		float previous = amplitude * sin(frequency * (time - seconds_elapsed));

		float delta = current - previous;

		m.MakeTranslationMatrix(0, delta, 0);
		entities[2].Update(seconds_elapsed, m);
	}
}


//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
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
	case SDLK_1: mode = 1; framebuffer.Fill(Color::BLACK); break;
	case SDLK_2: mode = 2; framebuffer.Fill(Color::BLACK); break;
	case SDLK_n: {
		std::cout << "Current Property = Camera Near\n";
		curr_property = 1;
		break;
	}
	case SDLK_f: {
		std::cout << "Current Property = Camera Far\n";
		curr_property = 2;
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
	case SDLK_z: {
		use_occlusion = !use_occlusion;
		break;
	}
	case SDLK_c: {
		use_interpolation = !use_interpolation;
		break;
	}
	case SDLK_w: {
		use_wireframe = !use_wireframe;
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