#pragma once

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "image.h"

class Button
{

public:

	enum ButtonType {
		CLEAR_IMG,
		LOAD_IMG,
		SAVE_IMG,
		PENCIL,
		ERASER,
		LINE,
		RECTANGLE,
		CIRCLE,
		TRIANGLE,
		BLACK,
		WHITE,
		PINK,
		YELLOW,
		RED,
		BLUE,
		CYAN
	};


	ButtonType type;
	Image image;
	Vector2 pos;

	Button();
	Button(ButtonType type, Image img, Vector2 p);

	bool IsMouseInside(Vector2 mousePosition);
};