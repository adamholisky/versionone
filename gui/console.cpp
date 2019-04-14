#include "kernel.h"

#define tab_size 8

/**
 * @brief      Createa a new Console object
 *
 * @param[in]  x         Top left x coordinates
 * @param[in]  y         Top left y coordinates
 * @param[in]  width     The width of the console in pixels
 * @param[in]  height    The height of the console in pixels
 * @param[in]  bg_color  The background color
 * @param[in]  fg_color  The foreground color
 */
Console::Console( char * name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int bg_color, unsigned int fg_color ) 
: Control( name ) {
	unsigned int _x, _y;

	this->box.x = x;
	this->box.y = y;
	this->box.w = width;
	this->box.h = height;

	this->width = this->box.w / 7;
	this->height = this->box.h / 14;

	this->bg_color = bg_color;
	this->fg_color = fg_color;

	this->current_x = 0;
	this->current_y = 0;
	this->prev_x = 0;
	this->prev_y = 0;

	this->waiting_on_input = false;

	//printf( "w h: %d %d\n", con->width, con->height );

	this->buffer = (char *)malloc( sizeof(char) * this->width * this->height );

	for( _y = 0; _y < this->height; _y++ ) {
		for( _x = 0; _x < this->width; _x++ ) {
			this->buffer[ (_y * this->width) + _x ] = ' ';
		}
	}

	add_console( this );
}

/**
 * @brief      Draws the console.
 */
void Console::draw( void ) {
	unsigned int x;
	unsigned int y;
	vga_info * vga = get_vga_info();

	draw_rect( this->box, this->bg_color );

	for( y = 0; y < this->height; y++ ) {
		for( x = 0; x < this->width; x++ ) {
			draw_char( vga->fbuffer, this->box.x + (x * vga->char_width), this->box.y + (y * vga->char_height), this->fg_color, this->bg_color, this->buffer[ (y * this->width) + x ] );
		}
	}
}

/**
 * @brief      Puts a character at a specific location
 *
 * @param[in]  c     Character to place
 * @param[in]  x     Column to place character at
 * @param[in]  y     Row to place character at
 */
void Console::put_char_at( char c, unsigned int x, unsigned int y ) {
	vga_info * vga = get_vga_info();

	this->buffer[ x + (this->width * y) ] = c;
	draw_char( vga->fbuffer, this->box.x + (x * vga->char_width), this->box.y + (y * vga->char_height), this->fg_color, this->bg_color, c );
}

/**
 * @brief      Puts a character at the current row and column and updates the carrot.
 *
 * @param[in]  c     The character to place
 */
void Console::put_char( char c ) {
	unsigned int x;
	unsigned int y;
	bool continue_tabbing = true;

	if( c == '\n' ) {
		this->put_char_at( ' ', this->current_x, this->current_y );
		this->current_x = this->width - 1;
	} else if( c == '\t' ) {
		this->can_update_cursor = false;

		while( this->current_x % tab_size ) {
			if( continue_tabbing ) {
				if( this->current_x != this->width ) {
					this->put_char( ' ' );
				} else {
					continue_tabbing = false;
				}
			}
		}
		
		this->can_update_cursor = true;
	} else {
		this->put_char_at( c, this->current_x, this->current_y );
	}

	this->current_x++;

	if( this->current_x == this->width ) {
		this->current_x = 0;

		this->current_y++;

		if( this->current_y == this->height ) {
			this->current_y--;

			for( y = 0; y < this->height - 1; y++ ) {
				for( x = 0; x < this->width; x++ ) {
					this->buffer[ (y * this->width) + x ] = this->buffer[ ((y + 1) * this->width) + x ];
				}
			}

			for( x = 0; x < this->width; x++ ) {
				this->buffer[ ((this->height - 1) * this->width) + x ] = ' ';
			}

			this->draw();
		}
	}

	if( this->can_update_cursor ) {
		gui_update_carrot();
	}
}

/**
 * @brief      Puts a blank character at the last character placed and updates the carrot.
 */
void Console::clear_last_char( ) {
	this->put_char_at( ' ', this->current_x, this->current_y );
	this->current_x--;

	gui_update_carrot();
}

