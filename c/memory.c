
#include "kernel.h"

extern "C" void load_kernel_managed_paging( uint32_t * page_dir );
void switch_page_directory( uint32_t * dir );

void * mem_top;

bool allow_mem_debug = false;

uint32_t page_directory[1024] __attribute__((aligned(4096)));

/**
 * @brief      Sets up the start of the memory.
 */
void initalize_memory( void ) {
	mem_top = (void *)0xC0080000;
}

void enable_mem_debug( void ) {
	allow_mem_debug = true;
}

/**
 * @brief      Sets up paging for everyone using a new page directory.
 */
void initalize_paging( void ) {
	unsigned int i;
	multiboot_info_t * multiboot_info = get_multiboot_header();

	for( i = 0; i < 1024; i++ ) {
	    page_directory[i] = 0x00000000;
	}

	page_directory[ 0 ] = 0x10000083; // loaded binary file from linker, mapped to 0
	page_directory[ kernel_memory_base >> 22 ] = 0x00000083; // Kernel 
	page_directory[ 769 ] = 0x00400083; // Second kernel page can go to physical D... because why not
	page_directory[ 770 ] = 0x00800083;
	page_directory[ 771 ] = 0x00C00083;
	page_directory[ vga_memory_base >> 22 ] = multiboot_info->framebuffer_addr | 0x00000083; // VGA

	load_kernel_managed_paging( page_directory );
}

void find_unused_page( void ) {

}

void find_page_with_free_space( void ) {

}

/**
 * @brief      Gets current top memory address.
 *
 * @return     Pointer to the current top of the memory.
 */
void * get_mem_top ( void ) {
	return mem_top;
}

/**
 * @brief      Amazingly basic malloc.
 *
 * @param[in]  size  The amount of memory to be allocated.
 *
 * @return     Void pointer to the allocated memory.
 */
void * malloc( long size ) {
	void * return_val = mem_top;

	mem_top = mem_top + size;

	// if( allow_mem_debug ) { dv( mem_top ); }

	return return_val;
}

//
// These functions are required by C++ to use the new operator.
//

void * operator new( size_t size ) {
    return malloc( size );
}
 
void * operator new[]( size_t size ) {
    return malloc(size);
}
 
void operator delete( void *p ) {
    //free(p);
}
 
void operator delete[]( void *p ) {
    //free(p);
}

void operator delete( void *p, unsigned long l ) {
	//free(p);
}