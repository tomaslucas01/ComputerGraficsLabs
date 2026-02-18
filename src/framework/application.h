/*
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
#include "particleSystem.h"
#include "entity.h"
#include "shader.h"

class Application
{
public:

	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	// Defined by us ----
	
	Camera camera;
	std::vector<Entity> entities;
	int orthographic_size = 10;
	int mode = 1;
	Vector3 eye;
	Vector3 center;
	Vector3 up;

	float angle;
	float pitch;

	float radius;
	int scale_growing = 1;
	int last_scale_trigger = 0;
	int curr_property = 1;
	float p_near, p_far = 0;
	int fov = 0;
	bool mouse_left_pressed;
	bool mouse_right_pressed;

	bool use_occlusion = true;
	bool use_texture = true;
	bool use_interpolation = true;
	bool use_wireframe = false;

	// Lab 4

	Shader* shader;
	Mesh* mesh;


	// ------------------

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;
	FloatImage zBuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init(void);
	void Render(void);
	void Update(float dt);

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport(0, 0, width, height);
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
		this->zBuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return Vector2(float(w), float(h));
	}
};