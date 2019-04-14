#define color_titlebar 0x37474F
#define color_titlebar_text 0xECEFF1
#define color_window_box 0x37474F
#define color_window 0x78909C
#define color_control_background 0x546E7A
#define color_control_text 0xECEFF1

#define register_control_list(Name, name) \
	add_control_type_handles(name) \
	add_control_type(Name, name) \
	find_control_type(Name, name)

#define add_control_type_handles(name) \
	struct node * name##_handles;

#define add_control_type(Name, name) \
	void add_##name ( Name *c ) { \
		list_push( & name##_handles, c, sizeof( Name ) ); \
	}

#define find_control_type(Name, cname) \
	Name * find_##cname ( char * n ) { \
		Name * 		found_control = NULL; \
		struct node * 	list = cname##_handles; \
		while( list != NULL ) { \
			Name *	temp = (Name *)list->data; \
			if( strcmp( temp->name, n ) != 0 ) { \
				list = list->next; \
			} else { \
				found_control = temp; \
				list = NULL; \
			} \
		} \
		return found_control; \
	}

void initalize_gui( void );
void draw_base( void );
bool is_gui_active( void );
void gui_update_carrot( void );
void gui_move_row( unsigned int dest, unsigned int src );
void gui_put_char_at( char c, unsigned int x, unsigned int y );
void gui_write_string_at( const char* data, unsigned int x, unsigned int y );
void gui_flip_cursor( void );
void gui_set_default_con_current_row_col( unsigned int row, unsigned int col );
void gui_put_char( char c );
void gui_clear_last_char( void );

void console_set_default( Console * con );
Console * console_get_default( void );
void printf_console( char * name, char *fmt, ...);

void add_to_top_level_controls( Control *c );
void remove_from_top_level_controls( Control *c );

void add_control( Control *c );
void add_console( Console *c );
void remove_control( Control *c );
Control * find_control( char * n );