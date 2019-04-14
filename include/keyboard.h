#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

void initalize_keyboard( void );
extern "C" void keyboard_interrupt_handler( void );
char get_scancode( void );
char get_character( unsigned int scancode );

extern "C" void mouse_handler( void );
void initalize_mouse( void );
unsigned char mouse_read( void );
void mouse_write( unsigned char a_write );
void mouse_wait( unsigned char a_type );

#endif