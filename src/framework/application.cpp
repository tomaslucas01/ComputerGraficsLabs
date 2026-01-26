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

	Vector3 eye = Vector3(0, 0, 1);
	Vector3 center = Vector3(0, 0, 0);
	Vector3 up = Vector3(0, 1, 0);  

	this->camera.LookAt(eye, center, up);

	this->camera.SetOrthographic(-10, 10, 10, -10, 0.1f, 10.f);


	Matrix44 model_matrix = Matrix44();
	model_matrix.SetIdentity();
	model_matrix.MakeScaleMatrix(10, 10, 10);

	e = Entity("../res/meshes/cleo.obj", model_matrix);
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
	e.Render(&framebuffer, &camera, Color(255, 0, 0));
	framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{
	// camera.UpdateViewMatrix();
	//Update the particles
}


//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
		
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