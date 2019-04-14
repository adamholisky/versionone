#include "kernel.h"
#include "tamzen.h"

#define font_smoothing_active true

unsigned int _char_width;
unsigned int _char_height;
unsigned int _screen_border;
unsigned int _fg_color;
unsigned int _bg_color;
bool render_background;
bool _is_vga_active = false;

uint8_t * prev_mouse_data;
int * bg_mask;
int current_mouse_x;
int current_mouse_y;
unsigned int prev_mouse_x;
unsigned int prev_mouse_y;
bool is_first_draw;

vga_info _vga_info;

void initalize_vga( void ) {
	multiboot_info_t * multiboot_info = get_multiboot_header();

	_vga_info.pitch = (unsigned int) multiboot_info->framebuffer_pitch;
	_vga_info.width = (unsigned int) multiboot_info->framebuffer_width;
	_vga_info.height = (unsigned int) multiboot_info->framebuffer_height;
	_vga_info.bpp = (unsigned int) multiboot_info->framebuffer_bpp;
	_vga_info.pixel_width = _vga_info.pitch / _vga_info.width;
	_vga_info.fbuffer = (unsigned char *)vga_memory_base;

	_vga_info.char_height = 14;
	_vga_info.char_width = 7;
	_vga_info.fg_color = 0x000000;
	_vga_info.bg_color = 0xAAAAAA;

	_char_height = 14;
	_char_width = 7;
	_screen_border = 10;

	_bg_color = 0xAAAAAA;
	_fg_color = 0x000000;
	render_background = true;

	bg_mask = malloc( sizeof(int) * 14 * 7 );

	current_mouse_x = 1280/2;
	current_mouse_y = 720/2;

	prev_mouse_data = malloc( sizeof( uint8_t ) + ( 14 * 3 * 4 * 8 ) );
	is_first_draw = true;

	fillrect( _vga_info.fbuffer, _bg_color, 0, 0, 1280, 720 );

	_is_vga_active = true;
}

bool is_vga_active( void ) {
	return _is_vga_active;
}

vga_info * get_vga_info( void ) {
	return &_vga_info;
}

inline void move_rect( rect dest, rect src ) {
	unsigned int i = 0;
	unsigned char * mem_dest;
	unsigned char * mem_src;
	unsigned int mem_size;

	for( i = 0; i < src.h; i++ ) {
		mem_dest = _vga_info.fbuffer + (dest.x * _vga_info.pixel_width) + ((dest.y + i) * _vga_info.pitch );
		mem_src = _vga_info.fbuffer + (src.x * _vga_info.pixel_width) + ((src.y + i) * _vga_info.pitch );
		mem_size = 3 * (_vga_info.pixel_width * src.w);


		for(; mem_size != 0; mem_size--) *mem_dest++ = *mem_src++;
	}
}

void vga_move_line( unsigned int dest_y, unsigned int src_y ) {
	rect src;
	rect dest;

	src.x = 0;
	src.y = _screen_border + (src_y * _char_height);
	src.w = _vga_info.width;
	src.h = _char_height;

	dest.x = 0;
	dest.y = _screen_border + (dest_y * _char_height );
	dest.w = _vga_info.width;
	dest.h = _char_height;

	move_rect( dest, src );
}

void vga_put_char( unsigned char c, unsigned int x, unsigned int y ) {
	draw_char( _vga_info.fbuffer, _screen_border + (x * _char_width), _screen_border + (y * _char_height), _vga_info.fg_color, _vga_info.bg_color, c );
}

inline void putpixel( uint8_t * buffer, unsigned int x, unsigned int y, int color) {
	unsigned int where = (x * _vga_info.pixel_width) + (y * _vga_info.width * _vga_info.pixel_width);
	buffer[where] = color & 255;              // BLUE
	buffer[where + 1] = (color >> 8) & 255;   // GREEN
	buffer[where + 2] = (color >> 16) & 255;  // RED
}

void draw_rect( rect r, unsigned int color ) {
	fillrect( _vga_info.fbuffer, color, r.x, r.y, r.w, r.h );
}

inline void fillrect( uint8_t * buffer, uint32_t color, unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	uint8_t * where = (buffer + (x * _vga_info.pixel_width) + (y * _vga_info.width * _vga_info.pixel_width ));
	unsigned int i, j;
	
	//write_to_serial_port( ',' );

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			*(uint32_t *)(where + (j * _vga_info.pixel_width) ) = color;
			//debug_f( "%d ", j );
		}
		where += _vga_info.pitch;
		//debug_f( "%d\n", i );
	}
}

void draw_string( char * string, unsigned int x, unsigned int y, unsigned int fg_color, unsigned int bg_color ) {
	while( *string ) {
		draw_char( _vga_info.fbuffer, x , y, fg_color, bg_color, *string );

		x = x + 7;

		string++;
	}
}

inline void draw_char( uint8_t * buffer, unsigned int x, unsigned int y, uint32_t fg, uint32_t bg, unsigned int font_char ) {
	int row, col;
	uint8_t * where = (buffer + (x * _vga_info.pixel_width) + (y * _vga_info.width * _vga_info.pixel_width));
	uint32_t new_r, new_g, new_b, new_fg;
	double alpha = 0.3;

	new_r = (((fg & 0xFF0000) >> 16) * alpha) + ((1 - alpha) * ((bg & 0xFF0000) >> 16));
	new_g = (((fg & 0x00FF00) >> 8) * alpha) + ((1 - alpha) * ((bg & 0x00FF00) >> 8));
	new_b = (((fg & 0x0000FF)) * alpha) + ((1 - alpha) * ((bg & 0x0000FF)));

	new_fg = ((new_r << 16) & 0xFF0000) | ((new_g << 8) & 0x00FF00) | (new_b & 0x0000FF);

	for (row = 0; row < 14; row++) {
		for (col = 0; col < 7; col++) {
			*(bg_mask + ((row) * 7) + (col) ) = 0;
		}
	}

	for (row = 0; row < 14; row++) {
		for (col = 0; col < 7; col++) {
			if( (_tamzen_bits[ _tamzen_offset[ font_char ] + row ] >> 8 & (1 << 7 - col)) ) {
				*(uint32_t *)(where + (col) * _vga_info.pixel_width) = fg;

				if( font_smoothing_active ) {
					//  X.
					//  .E
					//
					//IF pixel right 1 and down 1 exists, fill 1 right with alpha, and fill 1 down with alpha
					if( (col + 1 <= 7) && (row + 1 <= 14) ) { // if right 1 can happen and 1 down can happen
						if( (_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1) ] >> 8 & (1 << (7 - col - 1)) ) ) { // does right 1 and down 1 exist?
							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row) ] >> 8 & (1 << (7 - col - 1)) ) ) {	// does right 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col + 1) * _vga_info.pixel_width)) = new_fg;
								*(bg_mask + ((row) * 7) + (col + 1) ) = 1;
							}

							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1 ) ] >> 8 & (1 << (7 - col)) ) ) {// does down 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col) * _vga_info.pixel_width ) + _vga_info.pitch ) = new_fg;
								*(bg_mask + ((row + 1) * 7 ) + (col) ) = 1;
							}
						}
					} 	   

					//  .X
					//  E.
					//IF pixel left 1 and down 1 exists, fill 1 left with alpha, and fill 1 down with alpga
					if( (col - 1 >= 0) && (row + 1 <= 14) ) { // if right 1 can happen
						if( (_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1) ] >> 8 & (1 << (7 - col + 1)) ) ) { // does right 1 and down 1 exist?
							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row) ] >> 8 & (1 << (7 - col + 1)) ) ) {	// does right 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col - 1) * _vga_info.pixel_width)) = new_fg;
								*(bg_mask + ((row) * 7) + (col - 1) ) = 1;
							}

							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1 ) ] >> 8 & (1 << (7 - col)) ) ) {// does down 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col) * _vga_info.pixel_width ) + _vga_info.pitch ) = new_fg;
								*(bg_mask + ((row + 1) * 7 ) + (col) ) = 1;
							}
						}
					} 
				}
			} else {
				if( render_background ) {
					if( *(bg_mask + (row * 7) + (col)) == 0 ) { // Only render the bg if there isn't a filled pixel there
						*(uint32_t *)(where + (col) * _vga_info.pixel_width) = bg;
					}
				}
			}
		}
	   
		where += _vga_info.pitch;
	}
}

void draw_mouse( unsigned int x, unsigned int y ) {
	unsigned int i = 0;
	int row, col;
	uint8_t * mem_dest;
	uint8_t * mem_src;
	unsigned int mem_size;
	uint8_t * where = (_vga_info.fbuffer + (x * _vga_info.pixel_width) + (y * _vga_info.width * _vga_info.pixel_width));

	// 1. Reset the buffer under the mouse to what it was prior to placement
	if( is_first_draw == false ) {
		for( i = 0; i < 14; i++ ) {
			mem_dest = _vga_info.fbuffer + (prev_mouse_x * _vga_info.pixel_width) + ((prev_mouse_y + i) * _vga_info.pitch );
			mem_src = prev_mouse_data + ( i * ( 8 * _vga_info.pixel_width ) );
			mem_size = (_vga_info.pixel_width * 8);

			for(; mem_size != 0; mem_size--) *mem_dest++ = *mem_src++;	
		}
	}

	// 2. Take what's in the buffer at the new place and save it
	for( i = 0; i < 14; i++ ) {
		mem_src = _vga_info.fbuffer + (x * _vga_info.pixel_width) + ((y + i) * _vga_info.pitch );
		mem_dest = prev_mouse_data + ( i * ( 8 * _vga_info.pixel_width ) );
		mem_size = (_vga_info.pixel_width * 8);

		for(; mem_size != 0; mem_size--) *mem_dest++ = *mem_src++;
	}

	// 3. Render mouse over new location
	for (row = 0; row < 14; row++) {
		for (col = 7; col > -1; col--) {
			if( (cursor_bitmap[ row ] >> 8 & (1 << col)) ) {
				*(uint32_t *)(where + (7 - col) * _vga_info.pixel_width) = color_black;
			}
		}
	   
		where += _vga_info.pitch;
	}

	// 4. Save data for future movements
	prev_mouse_y = y;
	prev_mouse_x = x;
	is_first_draw = false;
}

void move_mouse( unsigned int direction, unsigned int amount ) {
	switch( direction ) {
		case cursor_up:
			current_mouse_y = current_mouse_y - amount;
			break;
		case cursor_down:
			current_mouse_y = current_mouse_y + amount;
			break;
		case cursor_right:
			current_mouse_x = current_mouse_x + amount;
			break;
		case cursor_left:
			current_mouse_x = current_mouse_x - amount;
			break;
	}

	if( current_mouse_y > 720 - 14) {
		current_mouse_y = 720 - 14;
	}

	if( current_mouse_y < 0 ) {
		current_mouse_y = 0;
	}

	if( current_mouse_x > 1280 - 7) {
		current_mouse_x = 1280 - 7;
	}

	if( current_mouse_x < 0 ) {
		current_mouse_x = 0;
	}

	draw_mouse( current_mouse_x, current_mouse_y );
}