#include "kernel.h"
#include <string.h>

class Term {
	public:
		uint32_t	current_row;
		uint32_t	current_column;
		uint32_t	foreground_color;
		uint32_t	background_color;

		void set_color( uint32_t foreground, uint32_t background );
		void print_color( void );
};

void Term::set_color( uint32_t foreground, uint32_t background ) {
	this->foreground_color = foreground;
	this->background_color = background;
}

void Term::print_color( void ) {
	printf( "0x%02X -- 0x%02X", this->foreground_color, this->background_color );
}

unsigned int 	term_current_row;
unsigned int 	term_current_column;
unsigned char	term_current_color;
unsigned short * term_buffer;

unsigned int VGA_WIDTH = 80;
unsigned int VGA_HEIGHT = 25;
unsigned int y_start;

bool 			is_debug_output = false;
Serial_Port 	*debug_port;

/* Setup the terminal
*   1. Define the default colors
*   2. Blank the screen
*/
void term_setup( void ) {
	unsigned int x = 0;
	unsigned int y = 0;

	term_current_row = 0;
	term_current_column = 0;
	y_start = 0;
	term_current_color = vga_entry_color( VGA_COLOR_WHITE, VGA_COLOR_BLUE );
	term_buffer = (unsigned short *) (0xC00B8000);

	for( y = 0; y < VGA_HEIGHT; y++ ) {
		for( x = 0; x < VGA_WIDTH; x++ ) {
			term_buffer[ (y * VGA_WIDTH) + x ] = vga_entry( ' ', term_current_color );
		}
	}

	
	
	init_printf( NULL, putc );
}

void initalize_serial_port( void ) {
	debug_port = new Serial_Port( Serial_Port::COM1 );
}

void write_to_serial_port( char c ) {
	debug_port->write( c );
}

void term_setup_for_gui( void ) {
	y_start = 0;
	VGA_HEIGHT = 25;
	term_current_row = 0;
}

/* Set the termina's color
*/
void term_set_color( unsigned char color ) {
	term_current_color = color;
}

/* Put character c at position x, y with color
* 
* Does not advance buffer.
*/
void term_put_char_at( char c, unsigned char color, unsigned int x, unsigned int y ) {
	color = 0;

	if( c != '\n' ) {
		if( is_gui_active() ) {
			gui_put_char_at( c, x, y );
		} else {
			term_buffer[ (y * VGA_WIDTH) + x ] = vga_entry( c, term_current_color );
		}
	}
}

/* Put character c at the current positions and advance the current position
*/
void term_put_char( char c ) {
	unsigned int x;
	unsigned int y;
	unsigned int max_row;

	if( is_debug_output == true ) {
		debug_port->write( c );

		return;
	}

	if( is_gui_active() ) {
		gui_put_char( c );
	} else {
		if( c != '\n' ) {
			term_put_char_at( c, term_current_color, term_current_column, term_current_row );
		} else {
			term_current_column = VGA_WIDTH - 1;	
		}

		term_current_column++;

		if( term_current_column == VGA_WIDTH ) {
			term_current_column = 0;

			term_current_row++;

			max_row = VGA_HEIGHT;

			if( term_current_row == max_row ) {
				term_current_row = max_row - 1;

				for( y = y_start; y < max_row - 1; y++ ) {
					if( is_gui_active() ) {
						gui_move_row( y, y + 1 );
					} else {
						for( x = 0; x < VGA_WIDTH; x++ ) {
							term_buffer[ (y * VGA_WIDTH) + x ] = term_buffer[ ((y + 1) * VGA_WIDTH) + x ];
						}
					}
				}

				for( x = 0; x < VGA_WIDTH; x++ ) {
					if( is_gui_active() ) {
						gui_put_char_at( ' ', x, VGA_HEIGHT - 1 );
					} else {
						const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
						term_buffer[ index ] = vga_entry( ' ', term_current_color );
					}
				}
			}
		}

		//gui_set_default_con_current_row_col( term_current_row, term_current_column );
		update_cursor( term_current_row, term_current_column );
	}
}

void term_clear_last_char( void ) {
	if( is_gui_active() ) {
		gui_clear_last_char();
	} else {
		int term_new_column = term_current_column - 1;

		//printf( "cc: %d --> nc: %d", term_current_column, term_new_column );
		if( term_new_column > 0 ) {
			term_put_char_at( ' ', term_current_color, term_new_column, term_current_row );

			term_current_column = term_new_column;
		}

		update_cursor( term_current_row, term_current_column );
	}
}

/* Outputs a string data of size the terminal
 */
void term_put_string( const char* data, size_t size ) {
	for( size_t i = 0; i < size; i++ ) {
		term_put_char( data[ i ] );
	}
}

/* Calculates the size of *data and outputs the string *data
 */
void term_write_string( const char* data ) {
	term_put_string( data, strlen( data ) );
}

void set_debug_output( bool d ) {
	is_debug_output = d;
}