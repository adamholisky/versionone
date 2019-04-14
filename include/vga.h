#define color_red 			0xFF0000
#define color_green 		0x00FF00
#define color_blue 			0x0000FF
#define color_white 		0xFFFFFF
#define color_black			0x000000
#define color_adams_blue	0x3753D1

#define cursor_up		1
#define cursor_down		2
#define	cursor_left		3
#define cursor_right	4	

struct _point {
	unsigned int x;
	unsigned int y;
};
typedef struct _point point;

struct _rect {
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
};
typedef struct _rect rect;

struct __vga_info {
	unsigned char * fbuffer;
	unsigned int pitch;
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned int pixel_width;

	unsigned int char_width;
	unsigned int char_height;

	unsigned int fg_color;
	unsigned int bg_color;
};
typedef struct __vga_info vga_info;

void initalize_vga( void );
bool is_vga_active( void );
vga_info * get_vga_info( void );
void putpixel( uint8_t * buffer, unsigned int x,unsigned int y, int color) __attribute__((always_inline));
void fillrect( uint8_t * buffer, uint32_t color, unsigned int x, unsigned int y, unsigned int w, unsigned int h) __attribute__((always_inline));
void vga_put_char( unsigned char c, unsigned int x, unsigned int y );
void vga_move_line( unsigned int dest_y, unsigned int src_y );
void move_rect( rect dest, rect src ) __attribute__((always_inline));
void draw_char( uint8_t * buffer, unsigned int x, unsigned int y, uint32_t fg, uint32_t bg, unsigned int font_char );
void draw_string( char * string, unsigned int x, unsigned int y, unsigned int fg_color, unsigned int bg_color );
void draw_rect( rect r, unsigned int color );
void draw_mouse( unsigned int x, unsigned int y );
void move_mouse( unsigned int direction, unsigned int amount );
