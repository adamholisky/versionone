#include "kernel.h"

/**
 * @brief      Adds the control c to the container list.
 *
 * @param      c     Control to add.
 */
void Container::add_to_container( Control *c ) {
	Control *current, *last_control;

	c->parent = this;

	if( this->control_list == NULL ) {
		this->control_list = c;
	} else {
		for( current = this->control_list ; current != NULL ; current = current->next ) {
			if( current->next == NULL ) {
				last_control = current;
			}
		}

		last_control->next = c;
	}
}

/**
 * @brief      Draws all controls in the container.
 */
void Container::draw_controls( void ) {
	Control *current;

	if( this->control_list != NULL ) {
		for( current = this->control_list ; current != NULL ; current = current->next ) {
			current->draw();
		}
	}	
}

/**
 * @brief      Finds the control at the coordinates within the container.
 *
 * @param[in]  x     X coordinate
 * @param[in]  y     Y coordinate
 *
 * @return     A pointer to the found control, otherwise NULL.
 */
Control * Container::find_control_at( int x, int y ) {
	Control * current;
	Control * found_control = NULL;

	if( this->control_list != NULL ) {
		for( current = this->control_list ; current != NULL ; current = current->next ) {
			// Use the lowest control in the chain that contains the point (most visible)
			if( current->contains_point( x, y ) ) {
				found_control = current;
			}
		}	
	}

	return found_control;
}
