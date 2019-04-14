#include "kernel.h"

bool 	global_can_update_cursor = false;

/**
 * @brief      This is needed by g++ if a pure virtual function call cannot be made.
 */
extern "C" void __cxa_pure_virtual()
{
	printf( "Pure virtual function call cannot be made. Halting." );

	while( 1 ) {
		;
	}
}

bool get_global_can_update_cursor( void ) {
	return global_can_update_cursor;
}

void list_push(struct node** head_ref, void *new_data, size_t data_size) {
// Allocate memory for node
	struct node* new_node = (struct node*)malloc(sizeof(struct node));

	new_node->data  = malloc(data_size);
	new_node->next = (*head_ref);

// Copy contents of new_data to newly allocated memory.
// Assumption: char takes 1 byte.
	unsigned int i;
	for (i=0; i<data_size; i++)
		*(char *)(new_node->data + i) = *(char *)(new_data + i);

// Change head pointer as new node is added at the beginning
	(*head_ref) = new_node;
}

int list_for_each(struct node * head_ref, int (*func)(struct node * n) ) {
	int ret_val = 0;

	struct node * list = head_ref;

	while( list != NULL ) {
		ret_val = func( list );

		if( ret_val == 0 ) {
			list = list->next;
		} else {
			list = NULL;
		}
	}

	return ret_val;
}

void get_string( char * string, unsigned int size ) {
	char scancode;
	char user_char;
	unsigned int x;

	scancode = get_scancode();
	x = 0;

	global_can_update_cursor = true;

	//printf( "string *: %X\n", (void *)string );

	while( scancode != 0x1C && x != size - 1) {
		if( scancode != 0 ) {
			user_char = get_character( (unsigned int)scancode );

			switch( user_char ) {
				case 0:
					break;
				case '\b':
					if( x > 0 ) {
						term_clear_last_char();
						string[ x ] = '\0';
						x--;
					}
					break;
				default:
					term_put_char( user_char );
					string[ x ] = user_char;

					x++;
			}
		}

		scancode = get_scancode();
	}

	global_can_update_cursor = false;

	printf( "\n" );

	string[ x ] = 0;
}

/* Basic putc wrapper for printf() to put a char on the screen
*/
void putc( void *p, char c ) {
	void *file_to_write_to = p;

// Kill the gcc warning of unused var since we're not supporting different streams right now
	file_to_write_to++;

	term_put_char( c );
}


void update_cursor( int row, int col ) {
	if( global_can_update_cursor ) {
		if( !is_gui_active() ) {
			unsigned short position=(row*80) + col;

			// cursor LOW port to vga INDEX register
			outportb(0x3D4, 0x0F);
			outportb(0x3D5, (unsigned char)(position&0xFF));

			// cursor HIGH port to vga INDEX register
			outportb(0x3D4, 0x0E);
			outportb(0x3D5, (unsigned char )((position>>8)&0xFF));
		} else {
			gui_update_carrot();
		}
	}
}

/* Begin tiny printf() implementation
* Original Source and Credit: http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
*/
static putcf stdout_putf;
static void* stdout_putp;
static void ui2a(unsigned int num, unsigned int base, int uc,char * bf)
{
	int n=0;
	unsigned int d=1;
	while (num/d >= base)
		d*=base;        
	while (d!=0) {
		int dgt = num / d;
		num%= d;
		d/=base;
		if (n || dgt>0 || d==0) {
			*bf++ = dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10);
			++n;
		}
	}
	*bf=0;
}

void * get_stdout_putp( void ) {
	return stdout_putp;
}

putcf get_stdout_putf( void ) {
	return stdout_putf;
}

static void i2a (int num, char * bf)
{
	if (num<0) {
		num=-num;
		*bf++ = '-';
	}
	ui2a(num,10,0,bf);
}

static int a2d(char ch)
{
	if (ch>='0' && ch<='9') 
		return ch-'0';
	else if (ch>='a' && ch<='f')
		return ch-'a'+10;
	else if (ch>='A' && ch<='F')
		return ch-'A'+10;
	else return -1;
}


static void putchw(void* putp,putcf putf,int n, char z, char* bf)
{
	char fc=z? '0' : ' ';
	char ch;
	char* p=bf;
	while (*p++ && n > 0)
		n--;
	while (n-- > 0) 
		putf(putp,fc);
	while ((ch= *bf++))
		putf(putp,ch);
}

void tfp_format(void* putp,putcf putf,char *fmt, va_list va)
{
	char bf[12];

	char ch;


	while ((ch=*(fmt++))) {
		if (ch!='%') 
			putf(putp,ch);
		else {
			char lz=0;
#ifdef  PRINTF_LONG_SUPPORT
			char lng=0;
#endif
			int w=0;
			ch=*(fmt++);
			if (ch=='0') {
				ch=*(fmt++);
				lz=1;
			}
			if (ch>='0' && ch<='9') {
				ch=a2i(ch,&fmt,10,&w);
			}
#ifdef  PRINTF_LONG_SUPPORT
			if (ch=='l') {
				ch=*(fmt++);
				lng=1;
			}
#endif
			switch (ch) {
				case 0: 
				goto abort;
				case 'u' : {
#ifdef  PRINTF_LONG_SUPPORT
					if (lng)
						uli2a(va_arg(va, unsigned long int),10,0,bf);
					else
#endif
						ui2a(va_arg(va, unsigned int),10,0,bf);
					putchw(putp,putf,w,lz,bf);
					break;
				}
				case 'd' :  {
#ifdef  PRINTF_LONG_SUPPORT
					if (lng)
						li2a(va_arg(va, unsigned long int),bf);
					else
#endif
						i2a(va_arg(va, int),bf);
					putchw(putp,putf,w,lz,bf);
					break;
				}
				case 'x': case 'X' : 
#ifdef  PRINTF_LONG_SUPPORT
				if (lng)
					uli2a(va_arg(va, unsigned long int),16,(ch=='X'),bf);
				else
#endif
					ui2a(va_arg(va, unsigned int),16,(ch=='X'),bf);
				putchw(putp,putf,w,lz,bf);
				break;
				case 'c' : 
				putf(putp,(char)(va_arg(va, int)));
				break;
				case 's' : 
				putchw(putp,putf,w,0,va_arg(va, char*));
				break;
				case '%' :
				putf(putp,ch);
				default:
				break;
			}
		}
	}
	abort:;
}


void init_printf(void* putp,void (*putf) (void*,char))
{
	stdout_putf=putf;
	stdout_putp=putp;
}

void tfp_printf( char *fmt, ...)
{
	va_list va;
	va_start(va,fmt);
	tfp_format(stdout_putp,stdout_putf,fmt,va);
	va_end(va);
}

void con_printf_secondary( char *fmt, va_list va ) {
	tfp_format(stdout_putp,stdout_putf,fmt,va);
}

static void putcp(void* p,char c)
{
	*(*((char**)p))++ = c;
}

void tfp_sprintf(char* s,char *fmt, ...)
{
	va_list va;
	va_start(va,fmt);
	tfp_format(&s,putcp,fmt,va);
	putcp(&s,0);
	va_end(va);
}
/* End tiny printf() implementation */

	void printfcomma2 (int n) {
		if (n < 1000) {
			printf ("%d", n);
			return;
		}
		printfcomma2 (n/1000);
		printf (",%03d", n%1000);
	}

	void printfcomma (int n) {
		if (n < 0) {
			printf ("-");
			n = -n;
		}
		printfcomma2 (n);
	}

static char a2i(char ch, char** src,int base,int* nump)
{
	char* p= *src;
	int num=0;
	int digit;
	while ((digit=a2d(ch))>=0) {
		if (digit>base) break;
		num=num*base+digit;
		ch=*p++;
	}
	*src=p;
	*nump=num;
	return ch;
}

void debug_f( char *fmt, ...) {
	set_debug_output( true );
	va_list va;
	va_start(va,fmt);
	tfp_format(stdout_putp,stdout_putf,fmt,va);
	va_end(va);
	set_debug_output( false );
}

unsigned long hex2int(char *a, unsigned int len)
{
	int i;
	unsigned long val = 0;

	for(i=0;i<len;i++)
	   if(a[i] <= 57)
		val += (a[i]-48)*(1<<(4*(len-1-i)));
	   else
		val += (a[i]-55)*(1<<(4*(len-1-i)));

	return val;
}

uint16_t inportw(uint16_t portid)
{
	uint16_t ret;
	asm volatile("inw %%dx, %%ax":"=a"(ret):"d"(portid));
	return ret;
}

char * to_lower( char * str ) {
	if( str != NULL ) {
		while( *str != '\0' ) {
			if( *str >= 65 && *str <= 90 ) {
				*str = *str + 32;
			}

			str++;
		}
	}

	return str;
}



