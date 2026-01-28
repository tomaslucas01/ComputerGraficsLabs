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
	//Vector3 dir = (eye - center).Normalize();
	//angle = atan2(dir.z, dir.x);
	angle = 90;

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
	anna_m.M[3][1] = -1;

	cleo_m.M[3][2] = 0;
	cleo_m.M[3][1] = -1;

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

	Matrix44 m;
	m.MakeRotationMatrix(seconds_elapsed, Vector3(0, 1, 0));
	entities[0].Update(seconds_elapsed, m);

	m.MakeScaleMatrix(1.001, 1.001, 1.001);
	entities[1].Update(seconds_elapsed, m);

	m.MakeTranslationMatrix(0, 0.005 * sin(3*time), 0);
	entities[2].Update(seconds_elapsed, m);

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
	if (event.button == SDL_BUTTON_LEFT) {

		angle += 0.15;
		eye.x = center.x + cos(angle) * radius;
		eye.z = center.z + sin(angle) * radius;

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