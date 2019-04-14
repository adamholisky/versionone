/**
 * @brief      Represents a window to draw on the screen.
 */
class Window : public Container {
	public:
		
		char 			title[100];

		unsigned int 	fg_color;
		unsigned int 	bg_color;

		Window( char * name, char * title, unsigned int x, unsigned int y, unsigned int width, unsigned int height );

		void draw( void );
};

/*
  -------------------------------------------------------   1px border
 | 25px Titlebar                                         |
  -------------------------------------------------------   
 |                                                       |
 |                                                       |
 |                                                       |
 |                                                       |
 |                                                       |
  ------------------------------------------------------- 
*/
