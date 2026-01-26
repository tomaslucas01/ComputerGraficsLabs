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

	eye = Vector3(0, -1, 5);
	center = Vector3(0, 0, 0);
	up = Vector3(0, 1, 0);  

	this->camera.LookAt(eye, center, up);
	this->camera.SetAspectRatio(float(width) / float(height));
	
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

	this->camera.SetPerspective(60, this->camera.aspect, 0.1f, 10.f);

	Matrix44 anna_m = Matrix44();
	int scale = 6;
	anna_m.MakeScaleMatrix(scale, scale, scale);
	anna_m.M[3][1] = -3;

	Matrix44 cleo_m = anna_m;
	Matrix44 lee_m = anna_m;

	anna_m.M[3][0] = -2;
	cleo_m.M[3][2] = -10;
	lee_m.M[3][0] = 2;
	
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
	entities[0].Render(&framebuffer, &camera, Color(255, 0, 0));
	entities[1].Render(&framebuffer, &camera, Color(255, 255, 255));
	entities[2].Render(&framebuffer, &camera, Color(0, 0, 255));

	
	framebuffer.Fill(Color::BLACK);
	this->camera.LookAt(eye, center, up);

	entities[0].Render(&framebuffer, &camera, Color(255, 0, 0));
	entities[1].Render(&framebuffer, &camera, Color(255, 255, 255));
	entities[2].Render(&framebuffer, &camera, Color(0, 0, 255));
	

	framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{
	// this->camera.SetAspectRatio(float(window_width) / float(window_height));
	// this->camera.SetOrthographic(-orthographic_size * camera.aspect, orthographic_size * camera.aspect, orthographic_size, -orthographic_size, -10.f, 10.f); // Important to not stretch!

	for (Entity& e : entities) {
		e.Update(seconds_elapsed);
	}
	//Update the particles
}


//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break;
		case SDLK_PLUS: eye.z++; break;
		case SDLK_MINUS: eye.z--; break;
		case SDLK_1: mode = 1; framebuffer.Fill(Color::BLACK); break;
		case SDLK_2: mode = 2; framebuffer.Fill(Color::BLACK); break;
		case SDLK_n:
		case SDLK_f:
		case SDLK_v: break;
	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
	
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

	// ...
}

void Application::OnFileChanged(const char* filename)
{
	Shader::ReloadSingleShader(filename);
}