#include "kernel.h"
#include <string.h>

Control::Control( char * n ) {
	this->name = malloc( sizeof( char ) * strlen(n) + 1 );
	strcpy( this->name, n );
	add_control( this );
}

/**
 * @brief      Determines if the control contains the given point.
 *
 * @param[in]  x     X coordinate
 * @param[in]  y     Y coordinate
 *
 * @return     True if contains point, false otherwise.
 */
bool Control::contains_point( int x, int y ) {
	double pointX = x;
	double pointY = y;

	if (pointX < (this->box.x + (.5*this->box.w)) && pointX > (this->box.x - (.5*this->box.w)) && pointY < (this->box.y + (.5*this->box.h)) && pointY > (this->box.y - (.5*this->box.h))) {
		return true;
	} else {
		return false;
	}
}