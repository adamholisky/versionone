#ifndef Controldef
#define Controldef

class Control {
	public:
		char 			*name;
		Control			*next;
		Control			*parent;
		rect			box;

		Control( char * n );

		virtual void draw( void ) = 0;
		bool contains_point( int x, int y );
};

#endif