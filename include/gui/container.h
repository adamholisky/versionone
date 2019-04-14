class Container : public Control {
	protected:
		Control		* control_list;

	public:

		Container( char * name ) : Control( name ) { };

		void add_to_container( Control *c );
		void draw_controls( void );
		Control * find_control_at( int x, int y );
};