#include "kernel.h"

unsigned int blink_count;
unsigned int blink_count_max;

void initalize_timer( void ) {
	blink_count = 0;
	blink_count_max = 50;
}

void timer_handler( void ) {
	if( blink_count == blink_count_max ) {
		blink_count = 0;
		gui_flip_cursor();
	} else {
		blink_count++;
	}

	pic_acknowledge( 0x20 );
}