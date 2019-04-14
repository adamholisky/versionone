#include "kernel.h"

idtr IDTR;
x86_interrupt IDT[256];// = (x86_interrupt*)0xD00;
byte	pic1_irq_mask;
byte	pic2_irq_mask;
bool	debug_interrupts = false;
bool 	gpf_fired = false;

void initalize_interrupts( void ) {
	remap_pic( 0x20, 0x28 );

	int divisor = 1193180 / 100;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */

	load_exceptions();

	AddInt( 0x20, int20, 0 );
	AddInt( 0x21, int21, 0 );
	AddInt( 0x22, int22, 0 );
	AddInt( 0x23, int23, 0 );
	AddInt( 0x24, int24, 0 );
	AddInt( 0x25, int25, 0 );
	AddInt( 0x26, int26, 0 );
	AddInt( 0x27, int27, 0 );
	AddInt( 0x28, int28, 0 );
	AddInt( 0x29, int29, 0 );
	AddInt( 0x2A, int2A, 0 );
	AddInt( 0x2B, int2B, 0 );
	AddInt( 0x2C, int2C, 0 );
	AddInt( 0x2D, int2D, 0 );
	AddInt( 0x2E, int2E, 0 );
	AddInt( 0x2F, int2F, 0 );
	AddInt( 0x30, int30, 0 );

	load_idtr();

	asm( "sti" );

	asm( "int %0" : : "i"(0x2E) );
	asm( "int %0" : : "i"(0x2F) );
	asm( "int %0" : : "i"(0x30) );

}

void set_debug_interrupts( bool d ) {
	debug_interrupts = d;
}

void interrupt_default_handler( unsigned long route_code, struct interrupt_stack stack ) {
	//if( route_code != 0x1001 ) debug_f( "Int Route: 0x%04X   eax: 0x%04X\n", route_code, stack.eax );

	if ( route_code == 0x1001 ) {
		timer_handler();
		pic_acknowledge( 0x20 );
	} else if ( route_code == 0x1002 ) {
		mouse_handler();
		pic_acknowledge( 0x2C );
	} else {
		unsigned int eax = (unsigned int)stack.eax;
		
		if( eax == 0x0001 ) {
			char * string = stack.edx - 0xC0100000;
			printf( string );
			debug_f( "printf syscall done\n" );
		}

		if( eax == 0xBADA ) {
			printf( "Interrupt testing called.\n" );
			printf( "  eax:  0x%08X  ebx:  0x%08X  ecx:  0x%08X  edx:  0x%08X\n", stack.eax, stack.ebx, stack.ecx, stack.edx );
			printf( "  esp:  0x%08X  ebp:  0x%08X  esi:  0x%08X  edi:  0x%08X\n", stack.esp, stack.ebp, stack.esi, stack.edi );
			printf( "  ds:   0x%04X  es:   0x%04X  fs:   0x%04X  gs:   0x%04X\n", stack.ds, stack.es, stack.fs, stack.gs );
			printf( "  eip:  0x%08X\n", stack.eip );
			//printf( "  ss:   0x%08X  sp:  0x%08X  flags: 0x%08X  cs:   0x%08X\n", stack.ss, stack.sp, stack.flags, stack.cs );
			//printf( "  flags:  0x%08X\n", stack.flags );
			//printf( "  eip:  0x%08X\n", stack.eip );
		}
		
		if( eax == 0x1000 ) {
			/*
			debug_f( "  eax:  0x%08X  ebx:  0x%08X  ecx:  0x%08X  edx:  0x%08X\n", stack.eax, stack.ebx, stack.ecx, stack.edx );
			debug_f( "  esp:  0x%08X  ebp:  0x%08X  esi:  0x%08X  edi:  0x%08X\n", stack.esp, stack.ebp, stack.esi, stack.edi );
			debug_f( "  ds:   0x%04X  es:   0x%04X  fs:   0x%04X  gs:   0x%04X\n", stack.ds, stack.es, stack.fs, stack.gs );
			debug_f( "  eip:  0x%08X\n", stack.eip );
			*/
			sys_break_handler( &stack );
			/*
			debug_f( "  eax:  0x%08X  ebx:  0x%08X  ecx:  0x%08X  edx:  0x%08X\n", stack.eax, stack.ebx, stack.ecx, stack.edx );
			debug_f( "  esp:  0x%08X  ebp:  0x%08X  esi:  0x%08X  edi:  0x%08X\n", stack.esp, stack.ebp, stack.esi, stack.edi );
			debug_f( "  ds:   0x%04X  es:   0x%04X  fs:   0x%04X  gs:   0x%04X\n", stack.ds, stack.es, stack.fs, stack.gs );
			debug_f( "  eip:  0x%08X\n", stack.eip );
			debug_f( "sys_break syscall done.\n" );
			*/
		}

		pic_acknowledge( 0x20 );
	}
}

void load_idtr()
{
    IDTR.limit  = 256*(sizeof(x86_interrupt)-1);
    IDTR.base   = IDT;

	idtr *IDTRptr = &IDTR;

	/* load IDTR with lidt */
    asm volatile("LIDT (%0) ": : "p"(IDTRptr));
}


void AddInt(int number, void (*handler)(), dword dpl)
{
	 word selector = 0x08;
	 word settings;
     dword offset = (dword)handler;


	 /* get CS selector */
     asm volatile("movw %%cs,%0" :"=g"(selector));


	 /* set settings options depending on dpl */
	 switch(dpl)
	 {
	 case 0: settings = INT_0; break;
	 case 1:
	 case 2:
	 case 3: settings = INT_3; break;
	 }
	 
	 /* set actual values of int */
	 IDT[number].low_offset   = (offset & 0xFFFF);
	 IDT[number].selector     = selector;
	 IDT[number].settings     = settings;
     IDT[number].high_offset  = (offset >> 16);
}

void load_exceptions()
{
	/*
	 * Add all Exception Interrupts
	 */
	AddInt(0, int00, 0);
    AddInt(1, int01, 0);
    AddInt(2, int02, 0);
    AddInt(3, int03, 0);
    AddInt(4, int04, 0);
    AddInt(5, int05, 0);
    AddInt(6, int06, 0);
    AddInt(7, int07, 0);
    AddInt(8, int08, 0);
    AddInt(9, int09, 0);
    AddInt(10, int10, 0);
    AddInt(11, int11, 0);
    AddInt(12, int12, 0);
    AddInt(13, int13, 0);
    AddInt(14, int14, 0);
    AddInt(16, int16, 0);
    AddInt(17, int17, 0);
    AddInt(18, int18, 0);
    AddInt(19, int19, 0);
	AddInt(20, 0, 0);
	//AddInt(21, 0, 0);
	AddInt(22, 0, 0);
	AddInt(23, 0, 0);
	AddInt(24, 0, 0);
	AddInt(25, 0, 0);
	AddInt(26, 0, 0);
	AddInt(27, 0, 0);
	AddInt(28, 0, 0);
	AddInt(29, 0, 0);
	//AddInt(30, 0, 0);
	AddInt(31, 0, 0);
}

 /* enables/disables software interrupts */
void INTS(bool on)
{
	if(on)
	{
		asm("sti");
	}
	else
	{
		asm("cli");
	}
}

void pic_acknowledge( unsigned int interrupt ) {
	if( interrupt > 0x28 ) {
		outportb( 0xA0, 0x20 );
	}

	outportb( 0x20, 0x20 );
}

void panic(char *message, char *code, bool halt)
{
     debug_f( message );
     
	 outportb(MASTER, EOI); //send PIC EOI command
}

/*
 * Exception Handlers
 */
void int_00(void)
{
	panic("Divide By Zero Error","#00", false);
}

void int_01(void)
{
	panic("Debug Error","#DB", false);
}

void int_02(void)
{
	panic("NMI Interrupt","#--", false);
}

void int_03(void)
{
	panic("Breakpoint","#BP", false);
}

void int_04(void)
{
	panic("Overflow","#OF", false);
}

void int_05(void)
{
	panic("BOUND Range Exceeded","#BR", false);
}

void int_06(void)
{
	panic("Invalid Opcode","#UD", false);
}

void int_07(void)
{
	panic("Device Not Available","#NM", false);
}

void int_08(void)
{
	panic("Double Fault","#DF", true);
}

void int_09(void)
{
	panic("Coprocessor Segment Overrun", "#NA", false);
}

void int_10(void)
{
	panic("Invalid TSS","#TS", false);
}

void int_11(void)
{
	panic("Segment Not Present","#NP", false);
}

void int_12(void)
{
	panic("Stack Segment Fault","#SS", false);
}

void int_13(void)
{
	if( ! gpf_fired ) {
		panic("Gneral Protection Fault","#GP", false);
		gpf_fired = true;
	}
}

void int_14( void * address )
{
	write_to_serial_port( '*' );
	debug_f( "PF: 0x%08X\n", address );
	//panic("Page Fault","#PF", false);
}

void int_16(void)
{
	panic("FPU Floating-Point Error","#MF", false);
}

void int_17(void)
{
	panic("Alignment Check","#AC", false);
}

void int_18(void)
{
	panic("Machine Check","#MC", true);
}

void int_19(void)
{
	panic("SIMD Floating-Point","#XF", false);
}

void int_22( void ) {
	pic_acknowledge( 0x22 );
}

void int_23( void ) {
	pic_acknowledge( 0x23 );
}

void int_24( void ) {
	pic_acknowledge( 0x24 );
}

void int_25( void ) {
	pic_acknowledge( 0x25 );
}

void int_26( void ) {
	pic_acknowledge( 0x26 );
}

void int_27( void ) {
	pic_acknowledge( 0x27 );
}

void int_28( void ) {
	pic_acknowledge( 0x28 );
}

void int_29( void ) {
	pic_acknowledge( 0x29 );
}

void int_2A( void ) {
	pic_acknowledge( 0x2A );
}

void int_2B( void ) {
	pic_acknowledge( 0x2B );
}

void int_2D( void ) {
	pic_acknowledge( 0x2D );
}

void int_2E( void ) {
	pic_acknowledge( 0x2E );
}

void int_2F( void ) {
	pic_acknowledge( 0x2F );
}

void remap_pic(int pic1, int pic2)
{
	byte md,sd;

	md=inportb(MASTERDATA);                     // save state of MASTER DATA
	sd=inportb(SLAVEDATA);                      // save state of SLAVE DATA

	outportb(MASTER, EOI);                      // Send EOI | resets the chip

	outportb(MASTER, ICW1_INIT+ICW1_ICW4);      // ICW1 control word setup | just basic PIC stuff
	outportb(SLAVE, ICW1_INIT+ICW1_ICW4);       // see pic.h for more details about the values

	outportb(MASTERDATA, pic1);                 // ICW2 maps IRQs 0-7 to whatever kernel passes
	outportb(SLAVEDATA, pic2);                  // and same here except with IRQs 8-15 

	outportb(MASTERDATA, 0x04);                 // ICW3 
	outportb(SLAVEDATA, 0x02);

	outportb(MASTERDATA, ICW4_8086);            // ICW4 control word setup
	outportb(SLAVEDATA, ICW4_8086);

	outportb(MASTERDATA,md);                    // restore both MASTER DATA
	outportb(SLAVEDATA,sd);                     // restore SLAVE DATA
}
	
/* makes IRQ unavailable so it can't fire an interrupt */
void mask_irq( byte irq ) {
	if(irq==ALL)
	{
		outportb(MASTERDATA,0xFF);
		outportb(SLAVEDATA,0xFF);
		pic1_irq_mask = 0xFF;
		pic2_irq_mask = 0xFF;
	}
	else
	{
		irq = irq | (1<<irq);
		if(irq < 8)
			outportb(MASTERDATA, irq&0xFF);
		else
			outportb(SLAVEDATA, irq>>8);
	}
}

/* opposite of above */
void unmaskIRQ( byte irq ) {
	if(irq==ALL)
	{
		outportb(MASTERDATA,0x00);
		outportb(SLAVEDATA,0x00);
		pic1_irq_mask = 0x00;
		pic2_irq_mask = 0x00;
	}
	else
	{
		//irq = irq & (1<<irq);

		if(irq < 8) {
			pic1_irq_mask = pic1_irq_mask ^ (1<<irq);
			outportb(MASTERDATA, pic1_irq_mask );
		}
		else {
			pic2_irq_mask = pic2_irq_mask ^ (1<<irq);
			outportb(SLAVEDATA, pic2_irq_mask);
		}
	}
}