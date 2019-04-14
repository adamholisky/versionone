

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

void term_setup( void );
void term_setup_for_gui( void );
void term_set_color( unsigned char color );
void term_put_char( char c );
void term_put_char_at( char c, unsigned char color, unsigned int x, unsigned int y );
void term_clear_last_char( void );
void term_put_string( const char* data, size_t size );
void term_write_string( const char* data );
void test_class( void  );
void set_debug_output( bool d );
void initalize_serial_port( void );
int is_transmit_empty();
void write_serial(char a);
void initalize_serial_port( void );
void write_to_serial_port( char c );

static inline unsigned char vga_entry_color( enum vga_color fg, enum vga_color bg ) {
	return fg | bg << 4;
}
 
static inline unsigned int vga_entry( unsigned char uc, unsigned char color ) {
	return (unsigned int) uc | (unsigned int) color << 8;
}
