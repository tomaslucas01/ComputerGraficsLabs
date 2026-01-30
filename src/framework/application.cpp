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

	// Our inits

	this->camera = Camera();

	eye = Vector3(0, 0, 5);
	center = Vector3(0, 0, 0);
	up = Vector3(0, 1, 0);
	radius = (eye - center).Length();

	this->camera.LookAt(eye, center, up);
	this->camera.SetAspectRatio(float(width) / float(height));
	
	//CASE WE MOVE THE CAMERA, WE USE THIS LINE OF CODE TO KNOW WHERE IS THE CENTER AND OUR ANGLE WITH THE CENTRE
	Vector3 dir = (eye - center).Normalize();
	angle = atan2(dir.z, dir.x);


	// Orthographic view

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

	px = 0.1f;
	py = 4.0f;
	fov = 60;

	this->camera.SetPerspective(60, this->camera.aspect, px, py);

	Matrix44 anna_m = Matrix44();
	int scale = 6;
	anna_m.MakeScaleMatrix(scale, scale, scale);
	anna_m.M[3][1] = -3;

	Matrix44 cleo_m = anna_m;
	Matrix44 lee_m = anna_m;

	anna_m.M[3][0] = -2;
	anna_m.M[3][1] = -1;

	cleo_m.M[3][2] = 0;
	cleo_m.M[3][1] = -2.5;

	lee_m.M[3][0] = 2;
	lee_m.M[3][1] = -1.5;
	
	Entity anna = Entity("../res/meshes/anna.obj", anna_m);
	Entity cleo = Entity("../res/meshes/cleo.obj", cleo_m);
	Entity lee = Entity("../res/meshes/lee.obj", lee_m);

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

	framebuffer.Fill(Color::BLACK);
}


// Render one frame
void Application::Render(void)
{
	framebuffer.Fill(Color::BLACK);

	if (mode == 1) {
		entities[0].Render(&framebuffer, &camera, Color(255, 0, 0));
	}
	else {
		entities[0].Render(&framebuffer, &camera, Color(255, 0, 0));
		entities[1].Render(&framebuffer, &camera, Color(255, 255, 255));
		entities[2].Render(&framebuffer, &camera, Color(0, 0, 255));
	}

	framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{
	// this->camera.SetAspectRatio(float(window_width) / float(window_height));
	// this->camera.SetOrthographic(-orthographic_size * camera.aspect, orthographic_size * camera.aspect, orthographic_size, -orthographic_size, -10.f, 10.f); // Important to not stretch!

	/*Matrix44 m;
	m.MakeRotationMatrix(seconds_elapsed, Vector3(0, 1, 0));
	entities[0].Update(seconds_elapsed, m);

	if (int(time) % 3 == 0 && int(time) != last_scale_trigger) { // Every three seconds (and also check if not same instant where int(time) % 3 == 0
		scale_growing = !scale_growing; // Change scale mode (grow or decreasse)
		last_scale_trigger = int(time); // Update last trigger
	}

	if (scale_growing) {
		m.MakeScaleMatrix(1.001f, 1.001f, 1.001f);
	}
	else {
		m.MakeScaleMatrix(0.999f, 0.999f, 0.999f);
	}
	entities[1].Update(seconds_elapsed, m);


	m.MakeTranslationMatrix(0, 0.005 * sin(3 * time), 0);
	entities[2].Update(seconds_elapsed, m);*/
}


//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break;
	case SDLK_PLUS: {
		if (curr_property == 1) {
			px += 0.1;
			std::cout << "Near field: " << px << "\n";
		}
		else if (curr_property == 2) {
			py += 0.1;
			std::cout << "Far field: " << py << "\n";
		}
		else { 
			if (fov < 170) fov += 5; //Avoid faces turning upside down
		} 

		// if (px >= py) px = py - 0.1; //near can't be greater than far

		this->camera.SetPerspective(fov, this->camera.aspect, px, py);
		break;
	}
	case SDLK_MINUS: {
		if (curr_property == 1) {
			px -= 0.1;
			// if (px < 0.01) px = 0.01;
			std::cout << "Near field: " << px << "\n";
		}
		else if (curr_property == 2) {
			py -= 0.1;
			// if (py <= px) py = px + 1;
			std::cout << "Far field: "<< py <<"\n";	

		}
		else {
			if (fov > 5) fov -= 5;	//Avoid faces turning upside down
		}

		this->camera.SetPerspective(fov, this->camera.aspect, px, py);
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

	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) {

		angle += 0.15;
		eye.x = center.x + cos(angle) * radius;
		eye.z = center.z + sin(angle) * radius;

		camera.LookAt(eye, center, up);
	}
	if (event.button == SDL_BUTTON_RIGHT) {
		center.x++;
		center.y++;
		center.z++;
		camera.LookAt(eye, center, up);
	}

}
void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{

}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
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