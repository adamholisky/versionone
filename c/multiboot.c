#include "kernel.h"

multiboot_info_t *multiboot_header;
unsigned long mb_magic;

void initalize_multiboot( unsigned long _mb_magic, multiboot_info_t *mbh ) {
	mb_magic = _mb_magic;
	multiboot_header = mbh;
}

multiboot_info_t * get_multiboot_header( void ) {
	return multiboot_header;
}