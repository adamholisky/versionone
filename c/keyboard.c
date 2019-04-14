#include "kernel.h"

unsigned char keyboard_map[128] = {
0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
'9', '0', '-', '=', '\b',	/* Backspace */
'\t',			/* Tab */
'q', 'w', 'e', 'r',	/* 19 */
't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
0,			/* 29   - Control */
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
'\'', '`',   0,		/* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
'm', ',', '.', '/',   0,				/* Right shift */
	'*',
0,	/* Alt */
' ',	/* Space bar */
0,	/* Caps lock */
0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
0,	/* < ... F10 */
0,	/* 69 - Num lock*/
0,	/* Scroll Lock */
0,	/* Home key */
0,	/* Up Arrow */
0,	/* Page Up */
	'-',
0,	/* Left Arrow */
	0,
0,	/* Right Arrow */
	'+',
0,	/* 79 - End key*/
0,	/* Down Arrow */
0,	/* Page Down */
0,	/* Insert Key */
0,	/* Delete Key */
	0,   0,   0,
0,	/* F11 Key */
0,	/* F12 Key */
0,	/* All other keys are undefined */
};

unsigned char keyboard_map_shift[128] = {
0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
'(', ')', '_', '+', '\b', /* Backspace */
'\t',     /* Tab */
'Q', 'W', 'E', 'R', /* 19 */
'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
0,      /* 29   - Control */
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
'"', '~',   0,    /* Left shift */
'|', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
'M', '<', '>', '?',   0,        /* Right shift */
	'*',
0,  /* Alt */
' ',  /* Space bar */
0,  /* Caps lock */
0,  /* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
0,  /* < ... F10 */
0,  /* 69 - Num lock*/
0,  /* Scroll Lock */
0,  /* Home key */
0,  /* Up Arrow */
0,  /* Page Up */
	'-',
0,  /* Left Arrow */
	0,
0,  /* Right Arrow */
	'+',
0,  /* 79 - End key*/
0,  /* Down Arrow */
0,  /* Page Down */
0,  /* Insert Key */
0,  /* Delete Key */
	0,   0,   0,
0,  /* F11 Key */
0,  /* F12 Key */
0,  /* All other keys are undefined */
};

char current_scancode;
bool is_shift;

void initalize_keyboard( void ) {
	current_scancode = 0;
	is_shift = false;
}

void keyboard_interrupt_handler( void ) {
	byte status;
	char scancode;

	status = inportb( 0x64 );

	if( status & 0x01 ) {
		scancode = inportb( 0x60 );

		//printf( " - %d - ", scancode );

		if( scancode == 42 || scancode == 54 ) {
			//printf( " +shift " );
			is_shift = true;
		} else if ( scancode == -86 || scancode == -74 ) {
			//printf( " -shift " );
			is_shift = false;
		}

		if( scancode > 0 ) {
			current_scancode = scancode;
		}
	}

	//printf( "- %d\n", scancode );

	pic_acknowledge( 0x21 );
}

char get_scancode( void ) {
	char scancode;

	scancode = current_scancode;
	current_scancode = 0;

	return scancode;
}

char get_character( unsigned int scancode ) {
	if( is_shift ) {
		return keyboard_map_shift[ scancode ];
	}

	return keyboard_map[ scancode ];
}

void mouse_wait( unsigned char a_type ) {
	unsigned int _time_out=100000;
	if(a_type==0) {
		while(_time_out--) {
			if((inportb(0x64) & 1)==1) {
				return;
			}
		}

		return;
	} else {
		while(_time_out--) {
			if((inportb(0x64) & 2)==0) {
				return;
			}
		}

		return;
	}
}

void mouse_write( unsigned char a_write ) {
	//Wait to be able to send a command
	mouse_wait(1);
	//Tell the mouse we are sending a command
	outportb(0x64, 0xD4);
	//Wait for the final part
	mouse_wait(1);
	//Finally write
	outportb(0x60, a_write);
}

unsigned char mouse_read( void ) {
	//Get's response from mouse
	mouse_wait(0);
	return inportb(0x60);
}

void initalize_mouse( void ) { 
	unsigned char _status;

	//Enable the auxiliary mouse device
	mouse_wait(1);
	outportb(0x64, 0xA8);

	//Enable the interrupts
	mouse_wait(1);
	outportb(0x64, 0x20);
	mouse_wait(0);
	_status=(inportb(0x60) | 2);
	mouse_wait(1);
	outportb(0x64, 0x60);
	mouse_wait(1);
	outportb(0x60, _status);

	//Tell the mouse to use default settings
	mouse_write(0xF6);
	mouse_read();  //Acknowledge

	//Enable the mouse
	mouse_write(0xF4);
	mouse_read();  //Acknowledge
	//debug_f( "Initalized: Mouse -- %d \n", _status );
} 

volatile unsigned int mouse_cycle = 0;
volatile char mouse_byte[ 3 ];
bool middle_button = false;
bool right_button = false;
bool left_button = false;

void mouse_handler( void ) {
	volatile char move_x = 0;
	volatile char move_y = 0;
	mouse_byte[ mouse_cycle++ ] = inportb( 0x60 );

	if( mouse_cycle == 3 ) {
		move_x = mouse_byte[1];
		move_y = mouse_byte[2];
		mouse_cycle = 0;
		middle_button = false;
		right_button = false;
		left_button = false;

		if( (mouse_byte[0] & 0x80) || (mouse_byte[0] & 0x40) ) {
			//debug_f( "Dropped due to overflow\n" );
			pic_acknowledge( 0x2C );
			return;
		}

		if( !(mouse_byte[0] & 0x20) ) {
			move_y |= 0xFFFFFF00; 				//delta-y is a negative value
		}

		if( !(mouse_byte[0] & 0x10) ) {
			move_x |= 0xFFFFFF00; 				//delta-x is a negative value
		}

		if( mouse_byte[0] & 0x4 ) {
			middle_button = true;
		}

		if( mouse_byte[0] & 0x2 ) {
			right_button = true;
		}

		if( mouse_byte[0] & 0x1 ) {
			left_button = true;
		}

		//debug_f( "At 3: (%d, %d) with M: %d, R: %d, L: %d\n", move_x, move_y, middle_button, right_button, left_button );

		if( move_x > 0 ) {
			move_mouse( cursor_right, move_x );
		} else if( move_x < 0 ) {
			move_mouse( cursor_left, move_x * -1);
		}

		if( move_y > 0 ) {
			move_mouse( cursor_up, move_y );
		} else if( move_y < 0 ) {
			move_mouse( cursor_down, move_y * -1 );
		}
	}

	//debug_f( ". %d\n", inportb( 0x60 ) );
	pic_acknowledge( 0x2C );
}