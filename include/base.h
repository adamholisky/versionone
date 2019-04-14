/* base.h
 *
 * Basic needs of the OS that could eventually be replaced with more complex routines.
 * 
 * This file includes printf() and other derrivative work.
 *
 */

#define GET_DEFINE_STRING(x) #x
#define STRING_VALUE_OF(x) GET_DEFINE_STRING(x)

typedef void (*putcf) (void*,char);

struct node {
	void *data;
	struct node *next;
};

void list_push(struct node** head_ref, void *new_data, size_t data_size);
int list_for_each(struct node * head_ref, int (*func)(struct node * n) );
 
/* Outputs a byte to the specified hardware port */
static inline void outportb(dword port,byte value)
{
    __asm__ __volatile__ ("outb %%al,%%dx"::"d" (port), "a" (value));
}

/* Outputs a word to a port */
static inline void outportw(dword port,dword value)
{
    __asm__ __volatile__ ("outw %%ax,%%dx"::"d" (port), "a" (value));
}

/* gets a byte from a port */
static inline byte inportb(dword port)
{
    byte value;
    __asm__ __volatile__ ("inb %w1,%b0" : "=a"(value) : "d"(port));
    return value;
}

uint16_t inportw(uint16_t portid);

void putc( void *p, char c );
void update_cursor( int row, int col );
void get_string( char * string, unsigned int size );

void init_printf(void* putp,void (*putf) (void*,char));
void tfp_printf(char *fmt, ...);
void tfp_sprintf(char* s,char *fmt, ...);
void tfp_format(void* putp,void (*putf) (void*,char),char *fmt, va_list va);
#define printf tfp_printf 
#define sprintf tfp_sprintf 

void printfcomma2 (int n);
void printfcomma (int n);
static char a2i(char ch, char** src,int base,int* nump);

void * get_stdout_putp( void );
putcf get_stdout_putf( void );

void con_printf_secondary( char *fmt, va_list va );
void debug_f( char *fmt, ...);
unsigned long hex2int(char *a, unsigned int len);
char * to_lower( char * str );
bool get_global_can_update_cursor( void );