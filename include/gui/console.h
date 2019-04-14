#ifndef Consoledef
#define Consoledef

/**
 * @brief      Represents a text console that can be printed to and recieve input at.
 */
class Console : public Control {
	public:
		rect			box;

		unsigned int 	width;
		unsigned int 	height;

		unsigned int 	fg_color;
		unsigned int 	bg_color;

		char 			*buffer;

		unsigned int 	current_x;
		unsigned int 	current_y;

		unsigned int 	prev_x;
		unsigned int 	prev_y;

		bool 			waiting_on_input;
		bool 			can_update_cursor;

		Console( char * name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int bg_color, unsigned int fg_color );

		void draw( void );
		void put_char_at( char c, unsigned int x, unsigned int y );
		void put_char( char c );
		void clear_last_char( void );
};

#endif