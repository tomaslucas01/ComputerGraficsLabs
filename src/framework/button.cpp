#include "button.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"

Button::Button() {
	this->type = LOAD_IMG;
	this->image = Image();
	this->pos = Vector2(0, 0);
}

Button::Button(ButtonType type, Image img, Vector2 p) {
	this->type = type;
	this->image = img;
	this->pos = p;
}


bool Button::IsMouseInside(Vector2 mousePosition) {
	if (pos.x <= mousePosition.x && mousePosition.x <= pos.x + image.width
		&& pos.y <= mousePosition.y && mousePosition.y <= pos.y + image.height) {
		return true;
	}
	return false;
}