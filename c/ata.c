#include "kernel.h"

#define ata_debug_display false

bool 		primary_ready;

uint16_t	primary_status[256];
uint16_t	* sector_read_buffer;
HardDrive	* primary_hard_drive;

void initalize_ata( void ) {
	uint32_t	drive_lba_sectors;
	uint32_t	drive_size_bytes;

	sector_read_buffer = malloc( sizeof(uint16_t) * 256 );
	primary_ready = false;
	
	outportb( ata_primary_control, 4 );
	outportb( ata_primary_control, 0 );
	outportb( ata_secondary_control, 4 );
	outportb( ata_secondary_control, 0 );
	
	drive_lba_sectors = primary_status[60] | primary_status[61] << 16;
	drive_size_bytes = drive_lba_sectors * 512;

	primary_hard_drive = new HardDrive( ata_primary, ata_master, drive_size_bytes );

	if( ata_debug_display ) {
		debug_f( "[ATA]     Identify Primary, Master:   %s\n", ata_identify( ata_primary, ata_master ) ? "true" : "false"  );
		debug_f( "[ATA]     Identify Primary, Slave:    %s\n", ata_identify( ata_primary, ata_slave ) ? "true" : "false" );
		debug_f( "[ATA]     Primary 60 and 61:          %X\n", drive_lba_sectors );
		debug_f( "[ATA]     Drive size:                 %db\n", drive_size_bytes );
		debug_f( "[ATA]     Drive size:                 %dk\n", drive_size_bytes / 1024 );
		debug_f( "[ATA]     Drive size:                 %dmb\n", drive_size_bytes / 1024 / 1024 );
		debug_f( "[ATA]     byte_to_sector( 20 )        %d, %d\n", byte_to_sector(20), byte_to_offset(20) );
		debug_f( "[ATA]     byte_to_sector( 512 )       %d, %d\n", byte_to_sector(512), byte_to_offset(512) );
		debug_f( "[ATA]     byte_to_sector( 513 )       %d, %d\n", byte_to_sector(513), byte_to_offset(513) );
		//debug_f( "[ATA] Identify Secondary, Master: %d\n", ata_identify( ata_secondary, ata_master ) );
		//debug_f( "[ATA] Identify Secondary, Slave:  %d\n", ata_identify( ata_secondary, ata_slave ) );
	}
}

HardDrive * get_primary_hard_drive( void ) {
	return primary_hard_drive;
}

void ata_primary_irq( void ) {
	//debug_f( "=== IRQ PRI\n" );
	primary_ready = true;
	pic_acknowledge( 0x2E );
}

void ata_secondary_irq( void ) {
	//debug_f( "=== IRQ SEC\n" );
	pic_acknowledge( 0x2F );
}

void ata_wait_for_primary( void ) {
	uint8_t		status = 0;

	do {
		status = inportb( ata_primary_port + ata_reg_status );

		if( status & ata_sr_err ) {
			debug_f( "should ret 0\n" );
			return 0;
		}
	} while( ( !(status & ata_sr_busy) && !( status & ata_sr_drq ) ) || ( !primary_ready ) );

	primary_ready = false;

	return;
}

void ata_wait_400_ns( void ) {
	//for( int i; i < 4; ) {
	//	i++;
	//}
}

uint8_t ata_read_sector( uint8_t drive, uint8_t master_or_slave, uint32_t lba ) {
	uint16_t 	port = 0;
	uint8_t		status = 0;
	int 		result = -1;
	uint16_t	data;

	primary_ready = false;

	if( drive == ata_primary ) {
		if( master_or_slave == ata_master ) {
			//debug_f( "p: 0x%X, d: %X\n", ata_primary_port + ata_reg_hddevsel, 0xE0 | ((lba>>24) & 0x0F) );
			outportb( ata_primary_port + ata_reg_hddevsel, 0xE0 | ((lba>>24) & 0x0F) );
		} else {
			outportb( ata_primary_port + ata_reg_hddevsel, 0xB0 );
		}
	}

	if( drive == ata_secondary ) {
		if( master_or_slave == ata_master ) {
			outportb( ata_secondary_port + ata_reg_hddevsel, 0xA0 );
		} else {
			outportb( ata_secondary_port + ata_reg_hddevsel, 0xB0 );
		}
	}
	
	//debug_f( "A\n" );
	
	switch( drive ) {
		case ata_primary: 
			port = ata_primary_port;
			break;
		case ata_secondary:
			port = ata_secondary_port;
			break;
	}

	//debug_f( "port: 0x%04X\n", port );

	//outportb( port + 1, 0x00 );
	outportb( port + ata_reg_seccount0, 1 );
	outportb( port + ata_reg_lba0, (uint8_t)(lba) );
	outportb( port + ata_reg_lba1, (uint8_t)(lba >> 8) );
	outportb( port + ata_reg_lba2, (uint8_t)(lba >> 16) );

	outportb( port + ata_reg_command, ata_cmd_read_pio );

	//debug_f( "B\n" );
	ata_wait_for_primary();
	//debug_f( "C\n" );

	for( int i = 0; i < 256; i++ ) {
		data = inportw( port + ata_reg_data );
		//data = (data << 8) | (data >> 8);
		*(sector_read_buffer + i) = data;
		//debug_f( "%04X ", data );
	}
	//debug_f( "D\n" );

	ata_wait_400_ns();

	//debug_f( "E\n" );

	return 1;
}

int ata_identify( uint8_t drive, uint8_t master_or_slave ) {
	uint16_t 	port = 0;
	uint8_t		status = 0;
	uint16_t	data = 0;
	int 		result = 0;

	if( drive == ata_primary ) {
		if( master_or_slave == ata_master ) {
			outportb( ata_primary_port + ata_reg_hddevsel, 0xA0 );
		} else {
			outportb( ata_primary_port + ata_reg_hddevsel, 0xB0 );
		}
	}

	if( drive == ata_secondary ) {
		if( master_or_slave == ata_master ) {
			outportb( ata_secondary_port + ata_reg_hddevsel, 0xA0 );
		} else {
			outportb( ata_secondary_port + ata_reg_hddevsel, 0xB0 );
		}
	}
	
	//debug_f( "A\n" );
	
	switch( drive ) {
		case ata_primary: 
			port = ata_primary_port;
			break;
		case ata_secondary:
			port = ata_secondary_port;
			break;
	}

	outportb( port + ata_reg_command, ata_cmd_identify );

	//debug_f( "0: %X, %X\n", port + ata_reg_command, ata_cmd_identify );

	//debug_f( "0.5\n" );

	status = inportb( port + ata_reg_status );

	//debug_f( "status: 0x%04X\n" );

	if( status ) {
		result = -2;

		while( inportb( port + ata_reg_status ) & ata_sr_busy != 0 ) {

		}

		//debug_f( "1" );
		do {
			status = inportb( port + ata_reg_status );

			if( status & ata_sr_err ) {
				//debug_f( "should ret 0\n" );
				return 0;
			}
		} while( !(status & ata_sr_busy) && !( status & ata_sr_drq ));

		//debug_f( "\nData: \n" );

		for( int i = 0; i < 256; i++ ) {
			data = inportw( port + ata_reg_data );
			primary_status[ i ] = data;
			//debug_f( "%d: %04X\n", i, data );
		}

		//debug_f( "\n" );


		result = 1;
	}

	return result;
}

HardDrive::HardDrive( uint8_t _drive, uint8_t _master_or_slave, uint8_t _size ) {
	this->drive = _drive;
	this->master_or_slave = _master_or_slave;
	this->size = _size;
}

uint8_t * HardDrive::read_sector( uint32_t sector ) {
	uint8_t * ret = NULL;

	if( ata_debug_display ) {
		debug_f( "HardDrive::read_sector( %d );\n", sector );
	}

	if( ata_read_sector( this->drive, this->master_or_slave, sector ) ) {
		ret = (uint8_t *)sector_read_buffer;
	}

	return ret;
}

uint8_t * HardDrive::read_bytes( uint32_t start_byte, uint32_t len, uint8_t * buffer ) {
	uint8_t sector_bytes[512];
	uint32_t buffer_index = 0;
	uint32_t sector_index = 0;
	uint32_t count = 0;
	uint32_t byte_to_start_at = start_byte;

	do {
		this->read_sector( byte_to_sector( byte_to_start_at ) );
		sector_index = byte_to_offset( byte_to_start_at );

		do {
			*(buffer + buffer_index) = *((uint8_t *)sector_read_buffer + sector_index );

			buffer_index++;
			sector_index++;
			count++;
			//debug_f( "." );
		} while( sector_index < 512 && buffer_index < len );

		byte_to_start_at = byte_to_start_at + count;
		sector_index = 0;
		count = 0;
		//debug_f( "," );
	} while( buffer_index < len );

	return buffer;
}