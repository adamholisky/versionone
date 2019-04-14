#include "kernel.h"

Button::Button( unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int bg_color, unsigned int fg_color ) {
	this->box.x = x;
	this->box.y = y;
	this->box.w = width;
	this->box.h = height;

	this->bg_color = bg_color;
	this->fg_color = fg_color;

	this->enabled = true;
}

void Button::draw( void ) {

}

