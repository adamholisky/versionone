class Button : public Control {
	public:
		unsigned int	bg_color;
		unsigned int	fg_color;

		bool 			enabled;

		Button( unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int bg_color, unsigned int fg_color );

		void draw( void );

};