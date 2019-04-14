/**
 * \mainpage Welcome to versionONE
 * 
 * \section sec_about About versionONE
 * 
 * Version one is an x86 operating system written in C and C++ that, at its best, is a simply hobby project by Adam Holisky. It's meant to do nothing more than allow a fine level of control over the computer at whatever state it's in. It is not meant to be anything near production-ready quality.
 * 
 * The goal of the operating system is to eventually run projects on an Intel Edison board.
 * 
 * \section sec_contact Contact Info
 * 
 * Reach me on Twitter at @@adamholisky.
 * 
 * Email at: aholisky@pmc.com 
 */

/**
 * @file kernel.c
 * @author Adam Holisky
 * @date December 2017
 * @brief Primary entry and setup of the kernel.
 */

#include "kernel.h"
#include <versionone.h>

extern void call_from_c_test( void );
extern "C" void print_via_syscall( void );

/**
 * @brief      Beginning of the C kernel.
 *
 * @param[in]  multiboot_magic  The multiboot magic
 * @param      multiboot_info   The multiboot information
 */
extern "C" void kernel_main( unsigned long multiboot_magic, multiboot_info_t * multiboot_info, unsigned int kernel_page_number ) {
	initalize_multiboot( multiboot_magic, multiboot_info );
	initalize_memory();
	initalize_serial_port();
	term_setup();

	debug_f( "\nWelcome to versionONE's serial port. Enjoy your debug spam!.\n" );

	debug_f( "[Success]\tMultiboot\n" );
	debug_f( "[Success]\tMemory\n" );
	debug_f( "[Success]\tSerial Port : COM1\n" );

	initalize_paging();
	debug_f( "[Success]\tPaging\n" );

	enable_mem_debug();

	initalize_mouse();
	debug_f( "[Success]\tMouse\n" );

	initalize_interrupts();
	debug_f( "[Success]\tInterrupts\n" );

	if( D_ENABLE_GRAPHICS ) {
		initalize_vga();
	}

	debug_f( "[Success]\tVGA\n" );

	if( D_ENABLE_GRAPHICS ) {
		initalize_gui();

		debug_f( "[Success]\tGUI\n" );
		Control *todo;

		Window * win_console = new Window( "default-window", "Default Console", 10, 10, 7 * 80 + 3, 14 * 25 + 1 + 25 );
		Window * win_second = new Window( "second-window", "Secondary Console", 700, 10, 7 * 80 + 3, 14 * 25 + 1 + 25 );
		
		Console * con = new Console( "default-console", 11, 35, 7 * 80, 14 * 25, color_control_background, color_control_text );
		//Console * con = new Console( "default-console", 11, 35, 7 * 80, 14 * 25, 0xFFFFFF, 0x000000 );
		console_set_default( con );
		win_console->add_to_container( con );

		Console * con_s = new Console( "second-console", 701, 35, 7 * 80, 14 * 25, color_control_background, color_control_text );
		win_second->add_to_container( con_s );

		win_console->draw();
		win_second->draw();
	}

	printf( "                               _              ____  _   _ ______ \n" );
	printf( "                              (_)            / __ \\| \\ | |  ____|\n" );
	printf( "           __   _____ _ __ ___ _  ___  _ __ | |  | |  \\| | |__   \n" );
	printf( "           \\ \\ / / _ \\ '__/ __| |/ _ \\| '_ \\| |  | |  \\` |  __|  \n" );
	printf( "            \\ V /  __/ |  \\__ \\ | (_) | | | | |__| | |\\  | |____ \n" );
	printf( "             \\_/ \\___|_|  |___/_|\\___/|_| |_|\\____/|_| \\_|______|\n" );
	printf( "\n                   A Hobby \"Operating System\" by Adam Holisky\n\n" );

	printf( "Build %s Timestamp: %s\n\n", STRING_VALUE_OF(BUILD_NUMBER), STRING_VALUE_OF(BUILD_TIMESTAMP) );
	
	initalize_keyboard();
	debug_f( "[Success]\tKeyboard\n" );

	initalize_timer();
	debug_f( "[Success]\tTimer\n" );

	initalize_processes();
	debug_f( "[Success]\tProcesses\n" );

	initalize_ata();
	debug_f( "[Success]\tATA\n" );

	//initalize_ext2_fs();
	//debug_f( "[Success] ext2 fs\n" );
	//
	initalize_fat_fs();
	debug_f( "[Success]\tFAT32 File System\n" );

	//debug_f( "Shutting down via command in kernel.c." );
	//outportb( 0xF4, 0x00 );

	initalize_commands();
	debug_f( "[Success]\tCommands\n" );
	
	debug_f( "mem_top:\t\t0x%08X\n", get_mem_top() );
	debug_f( "kernel_page_number:\t%d\n", kernel_page_number );
	debug_f( "0xE0000000 Page Frame:\t%d\n", (0xE0000000 >> 22) );
	debug_f( "Given Frambuffer:\t0x%08X\n", multiboot_info->framebuffer_addr );
	debug_f( "Frambuffer + Paging:\t0x%08X\n", multiboot_info->framebuffer_addr | 0x00000083 );

	printf_console( "second-console", "Printing to the secondary console again.\n" );
	printf_console( "second-console", "Another line of stuff.\n" );
 
	printf( "\n" );

	debug_f( "Build %s Timestamp:\t%s\n\n", STRING_VALUE_OF(BUILD_NUMBER), STRING_VALUE_OF(BUILD_TIMESTAMP) );

	//directory_and_file_test();

	launch_command_loop();

	debug_f( "Error: End of kernel_main() reached.\n" );
}

/**
 * @brief	Test function to demonstrate Assembly -> C calling
 * 
 * @todo 	Move this to somewhere else...
 */
void call_from_asm_test( void ) {
	printf( "\nCalled from ASM.\n" );
}