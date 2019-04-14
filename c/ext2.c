#include "kernel.h"

superblock_t *		superblock;
HardDrive *			hd;

void initalize_ext2_fs( void ) {
	superblock = malloc( sizeof( superblock_t ) );
	hd = get_primary_hard_drive();

	superblock = (superblock_t *) hd->read_bytes( 1024, 1024, (uint8_t *)superblock );

	peek( (uint8_t *)superblock, 30 );
}