#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	//Initialize particle system
	ps.Init(w, h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);
	this->fill_shapes = true;
	this->border_width = 1;
	paintheight = 10;
	paintwidth = 10;
	mode = 1;

	this->framebuffer.Resize(w, h);
	this->current_action = Button::PENCIL; //Starts with pencil
	this->current_color = Color::RED;

	// Load button images and create buttons
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

	/* optional
	Image i7;
	i7.LoadPNG("../res/images/Circle.png");
	buttons.push_back(Button(Button::CIRCLE, i7, Vector2(10 + i1.width * 6, 10)));
	*/

	Image i8;
	i8.LoadPNG("../res/images/triangle.png");
	buttons.push_back(Button(Button::TRIANGLE, i8, Vector2(10 + i1.width * 6, 10)));



	Image i16;
	i16.LoadPNG("../res/images/pencil.png");
	buttons.push_back(Button(Button::PENCIL, i16, Vector2(10 + i1.width * 7, 10)));


	Image i9;
	i9.LoadPNG("../res/images/black.png");
	buttons.push_back(Button(Button::BLACK, i9, Vector2(10 + i1.width * 8, 10)));


	Image i10;
	i10.LoadPNG("../res/images/white.png");
	buttons.push_back(Button(Button::WHITE, i10, Vector2(10 + i1.width * 9, 10)));


	Image i11;
	i11.LoadPNG("../res/images/pink.png");
	buttons.push_back(Button(Button::MAGENTA, i11, Vector2(10 + i1.width * 10, 10)));

	Image i12;
	i12.LoadPNG("../res/images/cyan.png");
	buttons.push_back(Button(Button::CYAN, i12, Vector2(10 + i1.width * 11, 10)));

	Image i13;
	i13.LoadPNG("../res/images/yellow.png");
	buttons.push_back(Button(Button::YELLOW, i13, Vector2(10 + i1.width * 12, 10)));

	Image i14;
	i14.LoadPNG("../res/images/red.png");
	buttons.push_back(Button(Button::RED, i14, Vector2(10 + i1.width * 13, 10)));

	Image i15;
	i15.LoadPNG("../res/images/blue.png");
	buttons.push_back(Button(Button::BLUE, i15, Vector2(10 + i1.width * 14, 10)));

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

}

// Render one frame
void Application::Render(void)
{
	// ...

	//If we are in drawing mode
	if (mode == 1) {
		// Draw tool bar
		framebuffer.DrawRect(0, 0, window_width, 50, Color(130, 130, 130), 0, true, Color(130, 130, 130));

		// Draw buttons 
		for (Button& b : buttons) {
			framebuffer.DrawImage(b.image, b.pos.x, b.pos.y);
		}
	}

	//If we are in Particle mode
	if (mode == 2) {
		//Fill the screen with black to clear the drawing mode and initialize the particle system
		framebuffer.Fill(Color::BLACK);
		ps.Render(&framebuffer);


	}
	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{
	//Update the particles
	ps.Update(time, seconds_elapsed, window_width, window_height);
}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_1: { //If we press 1, we go to drawing mode (mode = 1). We fill the screen with black to clear previous mode
		mode = 1;
		framebuffer.Fill(Color::BLACK);
		break;
	}
	case SDLK_2: { //If we press 2, we go to particle mode (mode = 2). We fill the screen with black to clear previous mode
		mode = 2;
		framebuffer.Fill(Color::BLACK);
		break;
	}
	case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	case SDLK_f: fill_shapes = !fill_shapes; break;// Alternate between filled an not filled
	case SDLK_PLUS: //If we press +
		if (current_action == Button::PENCIL || current_action == Button::ERASER) { //If we are using pencil or eraser, increase paint size
			paintwidth++; paintheight++; break;
		}
		else { //Otherwise, increase border width (rectangle)
			border_width++;  break;
		}
	case SDLK_MINUS: //If we press -
		if (current_action == Button::PENCIL || current_action == Button::ERASER) { //If we are using pencil or eraser, decrease paint size
			paintwidth--; paintheight--; break;
		}
		else { //Otherwise, decrease border width (rectangle)
			border_width = (border_width == 1 ? 1 : border_width - 1); break;
		}
	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
	//Do this only in drawing mode
	if (mode == 1) {
		if (event.button == SDL_BUTTON_LEFT) {
			old_mouse_position = mouse_position;

			for (Button& b : buttons) {
				if (b.IsMouseInside(mouse_position)) {
					switch (b.type) {
					case Button::CLEAR_IMG: //To clear the image, fill it with black
						framebuffer.Fill(Color::BLACK);
						break;
					case Button::LOAD_IMG: { //We load the last saved image
						Image temp;
						temp.LoadTGA("../res/images/output.tga", true);
						framebuffer.DrawImage(temp, (window_width / 2) - temp.width / 2, (window_height / 2) - temp.height / 2);
						break;
					}
					case Button::SAVE_IMG: //We save the current image
						framebuffer.SaveTGA("../res/images/output.tga");
						break;
					case Button::PENCIL:
					case Button::ERASER:
					case Button::LINE:
					case Button::RECTANGLE:
						//case Button::CIRCLE: optional
					case Button::TRIANGLE:
						current_action = b.type; //Set the current action to the selected button
						break;
						//We change the current color according to the selected button
					case Button::BLACK:
						current_color = Color::BLACK;
						break;
					case Button::WHITE:
						current_color = Color::WHITE;
						break;
					case Button::MAGENTA:
						current_color = Color::PINK;
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
}
void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
	//Do this only in drawing mode
	if (mode == 1) {
		if (event.button == SDL_BUTTON_LEFT) {
			if (current_action == Button::LINE) { //For lines, we draw a line from the old mouse position to the current one
				framebuffer.DrawLineDDA(old_mouse_position.x, old_mouse_position.y, mouse_position.x, mouse_position.y, current_color);
			}
			else if (current_action == Button::RECTANGLE) { //Draw a rectangle
				std::cout << border_width;
				float start_x = std::min(old_mouse_position.x, mouse_position.x); // min x
				float start_y = std::min(old_mouse_position.y, mouse_position.y); // min y
				int width = std::abs(mouse_position.x - old_mouse_position.x); // width in absolute value (if not, the rectangle does not appear correctly)
				int height = std::abs(mouse_position.y - old_mouse_position.y); // height in absolute value (same as before)
				framebuffer.DrawRect(start_x, start_y, width, height, Color::WHITE, border_width, fill_shapes, current_color);
			}
			else if (current_action == Button::TRIANGLE) { //Draw a triangle
				float x_min = std::min(old_mouse_position.x, mouse_position.x); //Know which is x_min
				float x_max = std::max(old_mouse_position.x, mouse_position.x); //Know which is x_max
				float y_min = std::min(old_mouse_position.y, mouse_position.y); //Know which is y_min
				float y_max = std::max(old_mouse_position.y, mouse_position.y); //Know which is y_max

				Vector2 p0(x_min, y_min);                         //Left corner in (x_min, y_min)
				Vector2 p1(x_max, y_min);                         //Right corner in (x_max,y_min)
				Vector2 p2((x_min + x_max) / 2, y_max);        //Center corner above, in (averagex, y_max)

				framebuffer.DrawTriangle(p0, p1, p2, Color::WHITE, fill_shapes, current_color);
			}
		}
	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	//Do this only in drawing mode
	if (mode == 1) {
		if (mouse_state != 0) {
			if (current_action == Button::PENCIL) { //Draw constantly rectangles while moving the mouse, so it looks like a pencil
				framebuffer.DrawRect(mouse_position.x, mouse_position.y, paintwidth, paintheight, current_color, 0, true, current_color);
			}

			else if (current_action == Button::ERASER) { //Draw constantly black rectangles while moving the mouse, so it looks like an eraser
				framebuffer.DrawRect(mouse_position.x, mouse_position.y, paintwidth, paintheight, Color::BLACK, 0, true, Color::BLACK);
			}

		}
	}
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