#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
	this->current_action = Button::PENCIL;
	this->current_color = Color::RED;

	Image i1;
	i1.LoadPNG("../res/images/clear.png");
	buttons.push_back(Button(Button::CLEAR_IMG, i1, Vector2(5, 10)));

	Image i2;
	i2.LoadPNG("../res/images/load.png");
	buttons.push_back(Button(Button::LOAD_IMG, i2, Vector2(10 + i1.width, 10)));

	Image i3;
	i3.LoadPNG("../res/images/save.png");
	buttons.push_back(Button(Button::SAVE_IMG, i3, Vector2(10 + i1.width * 2, 10)));


	Image i4;
	i4.LoadPNG("../res/images/eraser.png");
	buttons.push_back(Button(Button::ERASER, i4, Vector2(10 + i1.width * 3, 10)));


	Image i5;
	i5.LoadPNG("../res/images/line.png");
	buttons.push_back(Button(Button::LINE, i5, Vector2(10 + i1.width * 4, 10)));


	Image i6;
	i6.LoadPNG("../res/images/rectangle.png");
	buttons.push_back(Button(Button::RECTANGLE, i6, Vector2(10 + i1.width * 5, 10)));


	Image i7;
	i7.LoadPNG("../res/images/Circle.png");
	buttons.push_back(Button(Button::CIRCLE, i7, Vector2(10 + i1.width * 6, 10)));


	Image i8;
	i8.LoadPNG("../res/images/triangle.png");
	buttons.push_back(Button(Button::TRIANGLE, i8, Vector2(10 + i1.width * 7, 10)));



	Image i16;
	i16.LoadPNG("../res/images/pencil.png");
	buttons.push_back(Button(Button::PENCIL, i16, Vector2(10 + i1.width * 8, 10)));


	Image i9;
	i9.LoadPNG("../res/images/black.png");
	buttons.push_back(Button(Button::BLACK, i9, Vector2(10 + i1.width * 9, 10)));


	Image i10;
	i10.LoadPNG("../res/images/white.png");
	buttons.push_back(Button(Button::WHITE, i10, Vector2(10 + i1.width * 10, 10)));


	Image i11;
	i11.LoadPNG("../res/images/pink.png");
	buttons.push_back(Button(Button::MAGENTA, i11, Vector2(10 + i1.width * 11, 10)));

	Image i12;
	i12.LoadPNG("../res/images/cyan.png");
	buttons.push_back(Button(Button::CYAN, i12, Vector2(10 + i1.width * 12, 10)));

	Image i13;
	i13.LoadPNG("../res/images/yellow.png");
	buttons.push_back(Button(Button::YELLOW, i13, Vector2(10 + i1.width * 13, 10)));

	Image i14;
	i14.LoadPNG("../res/images/red.png");
	buttons.push_back(Button(Button::RED, i14, Vector2(10 + i1.width * 14, 10)));

	Image i15;
	i15.LoadPNG("../res/images/blue.png");
	buttons.push_back(Button(Button::BLUE, i15, Vector2(10 + i1.width * 15, 10)));
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	framebuffer.Fill(Color::BLACK);
	Vector2 p0(500, 400);
	Vector2 p1(800, 400);
	Vector2 p2(650, 650);
	framebuffer.DrawTriangle(p0, p1, p2, Color(200, 200, 200), true, Color(30, 100, 150));
}

// Render one frame
void Application::Render(void)
{
	// ...
	

	// Draw tool bar
	framebuffer.DrawRect(0, 0, 1500, 50, Color(130, 130, 130), 0, true, Color(130, 130, 130));

	// Draw buttons 
	for (Button& b : buttons) {
		framebuffer.DrawImage(b.image, b.pos.x, b.pos.y);
	}

	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{

	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
		for (Button& b : buttons) {
			if (b.IsMouseInside(mouse_position)) {
				switch (b.type) {
					case Button::CLEAR_IMG:
						framebuffer.Fill(Color::BLACK);
						break;
					case Button::LOAD_IMG:
						// Load?
						break;
					case Button::SAVE_IMG:
						// TODO
						// framebuffer.SaveTGA("../res/images/blabla.png");
						break;
					case Button::PENCIL:
					case Button::ERASER:
					case Button::LINE:
					case Button::RECTANGLE:
					case Button::CIRCLE:
					case Button::TRIANGLE:
						current_action = b.type;
						break;
					case Button::BLACK:
						current_color = Color::BLACK;
						break;
					case Button::WHITE:
						current_color = Color::WHITE;
						break;
					case Button::MAGENTA:
						current_color = Color(255, 255, 0);
						break;
					case Button::YELLOW:
						current_color = Color::YELLOW;
						break;
					case Button::RED:
						current_color = Color::RED;
						break;
					case Button::BLUE:
						current_color = Color::BLUE;
						break;
					case Button::CYAN:
						current_color = Color::CYAN;
						break;
				}

				std::cout << "(" << int(current_color.r) << ", " << int(current_color.g) << ", " << int(current_color.b) << ")";
			}
		}
	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
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