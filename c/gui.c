#include "kernel.h"
#include <string.h>

Console * default_console;

struct node * control_handles;

unsigned int cursor_count;
bool _is_gui_active = false;

void initalize_gui( void ) {
	_is_gui_active = true;

	//draw_mouse( 1280/2, 720/2 );
	
	term_setup_for_gui();
}

bool is_gui_active( void ) {
	return _is_gui_active;
}

register_control_list( Console, console )

void gui_move_row( unsigned int dest, unsigned int src ) {
	vga_move_line( dest, src );
}

void gui_update_carrot( void ) {
	if( get_global_can_update_cursor() ) {
		default_console->put_char_at( '|', default_console->current_x, default_console->current_y );
		
		cursor_count = 1;
	}
}

void gui_flip_cursor( void ) {
	if( is_vga_active() ) {
		if( cursor_count == 1 ) {
			default_console->put_char_at( ' ', default_console->current_x, default_console->current_y );
			cursor_count = 0;
		} else {
			default_console->put_char_at( '|', default_console->current_x, default_console->current_y );
			cursor_count = 1;
		}
	}
}

void gui_put_char_at( char c, unsigned int x, unsigned int y ) {
	default_console->put_char_at( c, x, y );
}

void gui_put_char( char c ) {
	default_console->put_char( c );
}

void gui_put_string_at( const char* data, size_t size, unsigned int x, unsigned int y ) {
	for( size_t i = 0; i < size; i++ ) {
		gui_put_char_at( data[ i ], x + i, y );
	}
}

void gui_clear_last_char( void ) {
	default_console->clear_last_char();
}

/* Calculates the size of *data and outputs the string *data
 */
void gui_write_string_at( const char* data, unsigned int x, unsigned int y ) {
	gui_put_string_at( data, strlen( data ), x, y );
}

void gui_set_default_con_current_row_col( unsigned int row, unsigned int col ) {
	default_console->prev_y = default_console->current_y;
	default_console->prev_x = default_console->current_x;
	default_console->current_y = row;
	default_console->current_x = col;

	gui_update_carrot();
}

void console_set_default( Console * con ) {
	default_console = con;
}

Console * console_get_default( void ) {
	return default_console;
}

void printf_console( char * name, char *fmt, ...)
{
	Console * con = find_console( name );

	if( con != NULL ) {
		//printf( "Printing on %s at (%d, %d)\n", con->name, con->current_x, con->current_y );

		Console * temp = console_get_default();
		console_set_default( con );

		va_list va;
		va_start(va,fmt);
		con_printf_secondary(fmt,va);
		va_end(va);

		console_set_default( temp );
	}
}

void add_to_top_level_controls( Control *c ) {

}

void remove_from_top_level_controls( Control *c ) {

}

/**
 * @brief      Adds the control to the master handle list
 *
 * @param      c     Pointer to the control
 */
void add_control( Control *c ) {
	list_push( &control_handles, c, sizeof(Control) );
}

/**
 * @brief      Removes a control from the master handle list.
 *
 * @param      c     Pointer to the control
 */
void remove_control( Control *c ) {

}

Control * find_control( char * n ) {
	Control * 		found_control = NULL;
	struct node * 	list = control_handles;

	while( list != NULL ) {
		Control *	temp = (Control *)list->data;

		//printf( "Given: %s, Control: %s\n", n, temp->name );

		if( strcmp( temp->name, n ) != 0 ) {
			list = list->next;
		} else {
			found_control = temp;
			list = NULL;
		}
	}

	return found_control;
}

