/*
 * Interrupt types
 */
#define INT_0 0x8E00     // 1000111000000000 = present,ring0,int_gate
#define INT_3 0xEE00     // 1110111000000000 = present,ring3,int_gate

/*
 * IRQ macros
 */
#define ALL       0xFF
#define TIMER     0
#define KEYBOARD  1
#define CASCADE   2
#define COM2_4    3
#define COM1_3    4
#define LPT       5
#define FLOPPY    6
#define FREE7     7
#define CLOCK     8
#define FREE9     9
#define FREE10    10
#define FREE11    11
#define PS2MOUSE  12
#define COPROC    13
#define IDE_1     14
#define IDE_2     15

/*
 * PIC macros
 * Master = PIC chip 1
 * Slave = PIC chip 2
 */
#define MASTER          0x20
#define MASTERDATA      0x21
#define SLAVE           0xA0
#define SLAVEDATA       0xA1
#define EOI             0x20

/*
 * ICW1 and ICW4 control words are the only ones we have to worry about
 */
#define ICW1_INIT    0x10               // required for PIC initialisation
#define ICW1_EDGE    0x08               // edge triggered IRQs
#define ICW1_SINGLE  0x02               // only MASTER (not cascaded)
#define	ICW1_ICW4    0x01               // there IS an ICW4 control word

#define ICW4_SFNM    0x10               // Special Fully Nested Mode
#define ICW4_BUFFER  0x08               // Buffered Mode
#define ICW4_MASTER  0x04               // this is the Master PIC
#define ICW4_AEOI    0x02               // Auto EOI
#define ICW4_8086    0x01               // 80/86 Mode

/* structure for an interrupt */
typedef struct
{
	word low_offset;                         // low nibble of offset to handler of interrupt 
	word selector;                           // GDT selector used 
	word settings;                           // settings for int 
	word high_offset;                        // high nibble to handler code 
}  __attribute__ ((packed)) x86_interrupt;


/* structure for the IDTR */
typedef struct
{
     word limit;                             // limit or Size of IDT 
     x86_interrupt *base;                    // a pointer to the base of the IDT 
} __attribute__ ((packed)) idtr;

extern "C" void int00(void);
extern "C" void int01(void);
extern "C" void int02(void);
extern "C" void int03(void);
extern "C" void int04(void);
extern "C" void int05(void);
extern "C" void int06(void);
extern "C" void int07(void);
extern "C" void int08(void);
extern "C" void int09(void);
extern "C" void int10(void);
extern "C" void int11(void);
extern "C" void int12(void);
extern "C" void int13(void);
extern "C" void int14(void);
extern "C" void int15(void);
extern "C" void int16(void);
extern "C" void int17(void);
extern "C" void int18(void);
extern "C" void int19(void);
extern "C" void int30(void);
extern "C" void int20( void );
extern "C" void int21( void );
extern "C" void int22( void );
extern "C" void int23( void );
extern "C" void int24( void );
extern "C" void int25( void );
extern "C" void int26( void );
extern "C" void int27( void );
extern "C" void int28( void );
extern "C" void int29( void );
extern "C" void int2A( void );
extern "C" void int2B( void );
extern "C" void int2C( void );
extern "C" void int2D( void );
extern "C" void int2E( void );
extern "C" void int2F( void );

extern "C" void int_00(void);
extern "C" void int_01(void);
extern "C" void int_02(void);
extern "C" void int_03(void);
extern "C" void int_04(void);
extern "C" void int_05(void);
extern "C" void int_06(void);
extern "C" void int_07(void);
extern "C" void int_08(void);
extern "C" void int_09(void);
extern "C" void int_10(void);
extern "C" void int_11(void);
extern "C" void int_12(void);
extern "C" void int_13(void);
extern "C" void int_14( void * address );
extern "C" void int_16(void);
extern "C" void int_17(void);
extern "C" void int_18(void);
extern "C" void int_19(void);
extern "C" void int_22(void);
extern "C" void int_23(void);
extern "C" void int_24(void);
extern "C" void int_25(void);
extern "C" void int_26(void);
extern "C" void int_27(void);
extern "C" void int_28(void);
extern "C" void int_29(void);
extern "C" void int_2A(void);
extern "C" void int_2B(void);
extern "C" void int_2D(void);
extern "C" void int_2E(void);
extern "C" void int_2F(void);

void load_idtr( void );//sets up IDT by loading IDTR
void AddInt( int number, void (*handler)(), dword dpl ); //add interrupt
void load_exceptions(); // init all exceptions
void panic(char *message, char *mnemonic, bool halt); //exception panic
void remap_pic(int pic1, int pic2);      //remap PIC
void mask_irq(byte irq);                 //PIC irq masking
void unmaskIRQ(byte irq);               //PIC irq unmasking
void INTS(bool on);                     //sti or cli
void pic_acknowledge(unsigned int interrupt);
void initalize_interrupts( void );
extern "C" void interrupt_default_handler( unsigned long route_code, struct interrupt_stack stack );
void set_debug_interrupts( bool d );

struct interrupt_stack {
	unsigned int	gs;
	unsigned int	fs;
	unsigned int	es;
	unsigned int	ds;
	unsigned int	edi;
	unsigned int	esi;
	unsigned int	ebp;
	unsigned int	esp;
	unsigned int	ebx;
	unsigned int	edx;
	unsigned int	ecx;
	unsigned int	eax;
	unsigned int 	ss;
	unsigned int	useresp;
	unsigned int    cs;
	unsigned int   	eip;
};
