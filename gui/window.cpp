#include "kernel.h"
#include <string.h>

/**
 * @brief      Creates a new window to draw on the screen.
 *
 * @param      title   The title of the window
 * @param[in]  x       Upper left x coordinates
 * @param[in]  y       Upper left y coordinates
 * @param[in]  width   The width of the window in pixels
 * @param[in]  height  The height of the window in pixels
 */
Window::Window( char name[255], char * title, unsigned int x, unsigned int y, unsigned int width, unsigned int height ) 
: Container( name ) {
	this->box.x = x;
	this->box.y = y;
	this->box.w = width;
	this->box.h = height;

	this->bg_color = color_window;
	this->fg_color = color_window_box;

	strcpy( this->title, title );

	add_to_top_level_controls( this );
}

/**
 * @brief      Redraws the window on the screen.
 */
void Window::draw( void ) {
	rect title_bar;
	rect interior;

	interior.x = this->box.x + 1;
	interior.y = this->box.y + 1;
	interior.w = this->box.w - 2;
	interior.h = this->box.h - 2;

	title_bar.x = this->box.x;
	title_bar.y = this->box.y;
	title_bar.w = this->box.w;
	title_bar.h = 25;

	draw_rect( this->box, this->fg_color );
	draw_rect( interior, this->bg_color );
	draw_rect( title_bar, color_titlebar );
	draw_string( this->title, title_bar.x + 5, title_bar.y + 5, color_titlebar_text, color_titlebar );

	this->draw_controls();
}
