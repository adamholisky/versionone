#include "kernel.h"
#include <string.h>

#define fat_display_debug false
#define __builtin_strtok( a, b ) strtok( a, b )

char *string_trim_inplace(char *s);

HardDrive * 		primary_hard_drive;
fat_BS_t *  		boot_record;
fat_extBS_32_t * 	ext_boot_record_fat32;
fat_extBS_16_t * 	ext_boot_record_fat16;
uint32_t			first_usable_cluster;
uint32_t 			first_root_dir_sector;
uint32_t 			first_data_sector;
uint32_t *			fat;


void initalize_fat_fs( void ) {
	uint32_t	total_clusters;
	uint32_t	total_sectors;
	uint32_t	fat_size;
	uint32_t 	root_dir_sectors;
	uint32_t 	first_fat_sector;
	uint32_t 	data_sectors;
	char *		volume_label;
	char * 		fat_type_label;
	uint8_t		fat_type;

	primary_hard_drive = get_primary_hard_drive();
	boot_record = malloc( sizeof(fat_BS_t) );
	ext_boot_record_fat32 = malloc( sizeof( fat_extBS_32_t ) );

	boot_record = (fat_BS_t *)primary_hard_drive->read_bytes( 0, sizeof( fat_BS_t ), (uint8_t *)boot_record );

	total_sectors = (boot_record->total_sectors_16 == 0) ? boot_record->total_sectors_32 : boot_record->total_sectors_16;
	fat_size = (boot_record->table_size_16 == 0) ? boot_record->table_size_16 : boot_record->table_size_16;
	root_dir_sectors = ((boot_record->root_entry_count * 32) + (boot_record->bytes_per_sector - 1)) / boot_record->bytes_per_sector;
	first_data_sector = boot_record->reserved_sector_count + (boot_record->table_count * fat_size) + root_dir_sectors;
	first_fat_sector = boot_record->reserved_sector_count;
	data_sectors = total_sectors - (boot_record->reserved_sector_count + (boot_record->table_count * fat_size) + root_dir_sectors);
	total_clusters = data_sectors / boot_record->sectors_per_cluster;
	first_root_dir_sector = first_data_sector - root_dir_sectors;
	first_usable_cluster = boot_record->reserved_sector_count + ( boot_record->table_count * fat_size );

	if( total_clusters < 4085 ) {
	   fat_type = FAT12;
	} else if(total_clusters < 65525) {
	   fat_type = FAT16;
	} else if (total_clusters < 268435445){
	   fat_type = FAT32;
	} else { 
	   fat_type = ExFAT;
	}

	switch( fat_type ) {
		case FAT16:
			ext_boot_record_fat16 = (fat_extBS_16_t *)primary_hard_drive->read_bytes( sizeof( fat_BS_t ), sizeof( fat_extBS_16_t ), (uint8_t *)ext_boot_record_fat16 );
			volume_label = ext_boot_record_fat16->volume_label;
			fat_type_label = ext_boot_record_fat16->fat_type_label;
			break;
		case FAT32:
			ext_boot_record_fat32 = (fat_extBS_32_t *)primary_hard_drive->read_bytes( sizeof( fat_BS_t ), sizeof( fat_extBS_32_t ), (uint8_t *)ext_boot_record_fat32 );
			volume_label = ext_boot_record_fat32->volume_label;
			fat_type_label = ext_boot_record_fat32->fat_type_label;
			break;
		default:
			debug_f( "fat_type not supported.\n" );
	}

	volume_label[10] = 0;

	fat = malloc( ext_boot_record_fat32->table_size_32 );
	fat = (uint32_t *)primary_hard_drive->read_bytes( cluster_to_lba( ext_boot_record_fat32->root_cluster ) * 512 , ext_boot_record_fat32->table_size_32, (uint8_t *)fat );

	if( fat_display_debug ) {
		debug_f( "[FAT]     OEM Name: %s\n", boot_record->oem_name );
		debug_f( "[FAT]     bytes_per_sector: %d\n", boot_record->bytes_per_sector );
		debug_f( "[FAT]     sectors_per_cluster: %d\n", boot_record->sectors_per_cluster );
		debug_f( "[FAT]     reserved_sector_count: %d\n", boot_record->reserved_sector_count );
		debug_f( "[FAT]     table_count: %d\n", boot_record->table_count );
		debug_f( "[FAT]     root_entry_count: %d\n", boot_record->root_entry_count );
		debug_f( "[FAT]     total_sectors_16: %d\n", boot_record->total_sectors_16 );
		debug_f( "[FAT]     media_type: %d\n", boot_record->media_type );
		debug_f( "[FAT]     table_size_16: %d\n", boot_record->table_size_16 );
		debug_f( "[FAT]     sectors_per_track: %d\n", boot_record->sectors_per_track );
		debug_f( "[FAT]     head_side_count: %d\n", boot_record->head_side_count );
		debug_f( "[FAT]     hidden_sector_count: %d\n", boot_record->hidden_sector_count );
		debug_f( "[FAT]     total_sectors_32: %d\n", boot_record->total_sectors_32 );
		debug_f( "[FAT]     total_sectors: %d\n", total_sectors );
		debug_f( "[FAT]     fat_size: %d\n", fat_size );
		debug_f( "[FAT]     root_dir_sectors: %d\n", root_dir_sectors );
		debug_f( "[FAT]     first_data_sector: %d\n", first_data_sector );
		debug_f( "[FAT]     first_fat_sector: %d\n", first_fat_sector );
		debug_f( "[FAT]     data_sectors: %d\n", data_sectors );
		debug_f( "[FAT]     total_clusters: %d\n", total_clusters );
		debug_f( "[FAT]     first_root_dir_sector: %d\n", first_root_dir_sector );
		debug_f( "[FAT]     Type: %d\n", fat_type );
		debug_f( "[FAT]     volume_label: %s\n", volume_label );
		debug_f( "[FAT]     fat_type_label: %s\n", fat_type_label );
		debug_f( "[FAT]     root_cluster: %d\n", ext_boot_record_fat32->root_cluster );
		debug_f( "[FAT]     sectors_per_fat: %d\n", ext_boot_record_fat32->table_size_32 );

		for( int i = 0; i < 20; i++ ) {
			debug_f( "[FAT]     table_value[%d]: 0x%X\n", i,  fat[i] );
		}
	}
}

uint32_t cluster_to_lba( uint32_t cluster )
{
	//uint32_t ret = first_usable_cluster + cluster * boot_record->sectors_per_cluster - (2 * boot_record->sectors_per_cluster);
	uint32_t ret = ((cluster - 2) * boot_record->sectors_per_cluster) + first_data_sector;

	//debug_f( "Cluster %d at lba %d\n", cluster, ret );

	return ret;
	//return ((cluster - 2) * boot_record->sectors_per_cluster) + first_data_sector;
}

void directory_and_file_test( void ) {
	unsigned int 			table_value = 666;
	uint8_t					*buffer = malloc( 64 );
	uint8_t 				*test_txt_buffer;
	uint32_t				cluster_num, test_txt_cluster, test_txt_size;
	fat_lfn_t * 			lfn = malloc( sizeof( fat_lfn_t ) );
	fat_directory_t *		dir = malloc( sizeof( fat_directory_t ) );
	char * 					name_fix = malloc( sizeof(char) * 15 );
	unsigned int 			i, z;

	i = 0;

	debug_f( "\nListing contents of root directory:\n" );
	debug_f( "-----------------------------------\n" );

	do {
		buffer = primary_hard_drive->read_bytes( cluster_to_lba( ext_boot_record_fat32->root_cluster ) * 512 + (ext_boot_record_fat32->table_size_32 * 512 * 2) + i, 64, (buffer) );

		// If the attribute is 0x0F then we have a long file name, otherwise normal
		if( buffer[11] == 0x0F ) {
			lfn = (fat_lfn_t *)buffer;
			dir = (fat_directory_t *)(buffer + 32);

			for( z = 0; z < 5; z++ ) {
				name_fix[z] = lfn->char_group_one[(z * 2)]; 
			}

			for( z = 0; z < 6; z++ ) {
				name_fix[z + 5] = lfn->char_group_two[(z * 2)]; 
			}

			for( z = 0; z < 1; z++ ) {
				name_fix[z + 11] = lfn->char_group_three[(z * 2)]; 
			}

			name_fix[12] = 0;
			i = i + 64;
		} else {
			dir = (fat_directory_t *)(buffer);
			strcpy( name_fix, dir->file_name );
			name_fix[9] = 0;
			i = i + 32;
		}

		//debug_f( "Peek dir at %d:\n", cluster_to_lba( ext_boot_record_fat32->root_cluster ) * 512 + (ext_boot_record_fat32->table_size_32 * 512 * 2) );
		//peek( (uint8_t *)dir, 10 );

			//strcpy( name_fix, dir->file_name );
		
		cluster_num = dir->entry_first_cluster_high << 16;
		cluster_num = dir->entry_first_cluster_low | cluster_num;

		if( strcmp( "test.txt", name_fix ) == 0 ) {
			test_txt_cluster = cluster_num;
			test_txt_size = dir->file_size;
		}

		debug_f( "%s\n", name_fix );

		//debug_f( "Found: \"%s\" :: 0x%2X :: %d :: %d\n", name_fix, dir->attributes, cluster_num, dir->file_size );
	} while( buffer[0] != 0 );

	if( test_txt_cluster != 0 ) {
		File * test_file = new File( "ADAM/KATIE.TXT" );
		test_txt_buffer = malloc( test_file->get_size() );
		test_txt_buffer = test_file->read( test_txt_buffer );
		debug_f( "%s\n", test_txt_buffer );
	}
}

uint32_t find_cluster_in_directory( uint32_t cluster, char * name_to_find ) {
	uint8_t					*buffer = malloc( 64 );
	uint32_t 				cluster_to_return = 0;
	uint32_t				cluster_num;
	fat_lfn_t * 			lfn = malloc( sizeof( fat_lfn_t ) );
	fat_directory_t *		dir = malloc( sizeof( fat_directory_t ) );
	char * 					name_fix = malloc( sizeof(char) * 15 );
	unsigned int 			i = 0;
	unsigned int 			z = 0;
	unsigned int 			x = 0;

	do {
		buffer = primary_hard_drive->read_bytes( cluster_to_lba( cluster ) * 512 + (ext_boot_record_fat32->table_size_32 * 512 * 2) + i, 64, (buffer) );

		// If the attribute is 0x0F then we have a long file name, otherwise normal
		if( buffer[11] == 0x0F ) {
			lfn = (fat_lfn_t *)buffer;
			dir = (fat_directory_t *)(buffer + 32);

			for( z = 0; z < 5; z++ ) {
				name_fix[z] = lfn->char_group_one[(z * 2)]; 
			}

			for( z = 0; z < 6; z++ ) {
				name_fix[z + 5] = lfn->char_group_two[(z * 2)]; 
			}

			for( z = 0; z < 1; z++ ) {
				name_fix[z + 11] = lfn->char_group_three[(z * 2)]; 
			}

			name_fix[12] = 0;
			i = i + 64;
		} else {
			for( x = 0; x < 15; x++ ) {
				name_fix[x] = 0;
			}
			dir = (fat_directory_t *)(buffer);
			strncat( name_fix, string_trim_inplace(dir->file_name), 8 );
			if( dir->dir ) {
				name_fix[9] = 0;
			} else {
				strcat( string_trim_inplace(name_fix), "." );
				strncat( string_trim_inplace(name_fix), string_trim_inplace(dir->ext), 3 );
				name_fix[14] = 0;
			}
			//strcpy( name_fix, dir->file_name );
			
			i = i + 32;
		}
		
		cluster_num = dir->entry_first_cluster_high << 16;
		cluster_num = dir->entry_first_cluster_low | cluster_num;

		to_lower( name_fix );

		//debug_f( "Comparing: \"%s\" against \"%s\"\n", string_trim_inplace(name_to_find), string_trim_inplace(name_fix) );
		if( strcmp( string_trim_inplace(name_to_find), string_trim_inplace(name_fix)) == 0 ) {
			return cluster_num;
		}
	} while( buffer[0] != 0 );

	return 0;
}

uint32_t find_size_of_file_in_directory( uint32_t cluster, char * name_to_find ) {
	uint8_t					*buffer = malloc( 64 );
	uint32_t 				cluster_to_return = 0;
	uint32_t				cluster_num;
	fat_lfn_t * 			lfn = malloc( sizeof( fat_lfn_t ) );
	fat_directory_t *		dir = malloc( sizeof( fat_directory_t ) );
	char * 					name_fix = malloc( sizeof(char) * 15 );
	unsigned int 			i = 0;
	unsigned int 			z = 0;
	unsigned int 			x = 0;

	do {
		buffer = primary_hard_drive->read_bytes( cluster_to_lba( cluster ) * 512 + (ext_boot_record_fat32->table_size_32 * 512 * 2) + i, 64, (buffer) );

		// If the attribute is 0x0F then we have a long file name, otherwise normal
		if( buffer[11] == 0x0F ) {
			lfn = (fat_lfn_t *)buffer;
			dir = (fat_directory_t *)(buffer + 32);

			for( z = 0; z < 5; z++ ) {
				name_fix[z] = lfn->char_group_one[(z * 2)]; 
			}

			for( z = 0; z < 6; z++ ) {
				name_fix[z + 5] = lfn->char_group_two[(z * 2)]; 
			}

			for( z = 0; z < 1; z++ ) {
				name_fix[z + 11] = lfn->char_group_three[(z * 2)]; 
			}

			name_fix[12] = 0;
			i = i + 64;
		} else {
			for( x = 0; x < 15; x++ ) {
				name_fix[x] = 0;
			}
			dir = (fat_directory_t *)(buffer);
			strncat( name_fix, string_trim_inplace(dir->file_name), 8 );
			if( dir->dir ) {
				name_fix[9] = 0;
			} else {
				strcat( string_trim_inplace(name_fix), "." );
				strncat( string_trim_inplace(name_fix), string_trim_inplace(dir->ext), 3 );
				name_fix[14] = 0;
			}
			//strcpy( name_fix, dir->file_name );
			
			i = i + 32;
		}
		
		cluster_num = dir->entry_first_cluster_high << 16;
		cluster_num = dir->entry_first_cluster_low | cluster_num;

		to_lower( name_fix );

		//debug_f( "Comparing: \"%s\" against \"%s\"\n", string_trim_inplace(name_to_find), string_trim_inplace(name_fix) );
		if( strcmp( string_trim_inplace(name_to_find), string_trim_inplace(name_fix)) == 0 ) {
			return dir->file_size;
		}
	} while( buffer[0] != 0 );

	return 0;
}

File::File( char * full_path_to_file ) {
	this->cluster_number = 0;

	char * 		tokens[ 10 ];
	char * 		token_ptr = NULL;
	char * 		file_path = malloc( strlen( full_path_to_file ) + 1 );
	int  		i = 0;
	int 		j = 0;
	int 		n = 0;
	uint32_t 	found_cluster;
	uint32_t	old_found_cluster;

	for( int j = 0; j < 10; j++ ) {
		tokens[j] = NULL;
	}

	strcpy( file_path, full_path_to_file );
	this->remove_leading_slash( file_path );
	
	token_ptr = strtok( file_path, "/" );
	 
	while( token_ptr != NULL && i < 10 ) {
		tokens[i] = malloc( strlen( token_ptr ) + 1 );
		strcpy( tokens[i], token_ptr );
		i++;
		token_ptr = strtok( NULL, "/" );
	}
	 
	n = i - 1;
	found_cluster = ext_boot_record_fat32->root_cluster; // we start traversing the path at root
	do {
		// Find the file in the directory, if it exists and n == 0, then it's the file, otherwise it's a directory
		// if it doesn't exist, file not found
		old_found_cluster = found_cluster;
		found_cluster = find_cluster_in_directory( found_cluster, tokens[j] );
		 
		if( found_cluster != 0 ) {
			if( n == 0 ) {
				this->cluster_number = found_cluster;
				this->size = find_size_of_file_in_directory( old_found_cluster, tokens[j] );
			}
		} else {
			n = -1;
			debug_f( "File not found." );
		}
		 
		j++;
		n--;
	} while( n >= 0 );
}

File::File( uint32_t _cluster_number, uint32_t _size ) {
	this->cluster_number = _cluster_number;
	this->size = _size;
}

char * File::read( char * buffer, unsigned int start = 0, int length = -1 ) {
	if( this->cluster_number == 0 ) {
		return NULL;
	}

	buffer = primary_hard_drive->read_bytes( cluster_to_lba( this->cluster_number ) * 512 + (ext_boot_record_fat32->table_size_32 * 512 * 2), this->size, buffer );

	return buffer;
}

char * File::remove_leading_slash( char * text ) {
	int x;
	int text_len = strlen( text );

	if( text_len > 2 && text[0] == '/' ) {
		for( x = 0; x < text_len - 1; x++ ) {
			text[x] = text[x + 1];
		}
		text[x] = 0;
	}

	return text;
}

char *string_trim_inplace(char *s) {
  while( *s == ' ' ) s++;
  if (*s) {
    char *p = s;
    while (*p) p++;
    while (' ' == *(--p));
    p[1] = '\0';
  }
  return s;
}

Directory * get_directory_from_path( char * full_path_to_directory ) {
	char * 		tokens[ 10 ];
	char * 		token_ptr = NULL;
	int  		i = 0;
	int 		j = 0;
	int 		n = 0;
	int 		x = 0;
	uint32_t 	found_cluster;
	uint32_t	old_found_cluster;
	bool 		only_show_root = false;
	Directory * dir_object = NULL;

	if( strcmp( full_path_to_directory, "" ) == 0 ) {
		only_show_root = true;
	}

	if( strcmp( full_path_to_directory, "/" ) == 0 ) {
		only_show_root = true;
	}

	if( only_show_root ) {
		//printf( "Directory of /\n\n", full_path_to_directory );
		dir_object = get_directory_from_cluster( ext_boot_record_fat32->root_cluster );
		dir_object->name = malloc( sizeof("/") );
		strcpy( dir_object->name, "/" );

		return dir_object;
	}

	to_lower( full_path_to_directory );

	if( strlen( full_path_to_directory ) > 2 && full_path_to_directory[0] == '/' ) {
		for( x = 0; x < strlen( full_path_to_directory ) - 1; x++ ) {
			full_path_to_directory[x] = full_path_to_directory[x + 1];
		}
		full_path_to_directory[x] = 0;
	}

	for( j = 0; j < 10; j++ ) {
		tokens[j] = NULL;
	}
	
	token_ptr = strtok( full_path_to_directory, "/" );

	while( token_ptr != NULL && i < 10 ) {
		tokens[i] = malloc( strlen( token_ptr ) + 1 );
		strcpy( tokens[i], token_ptr );
		i++;
		token_ptr = strtok( NULL, "/" );
	}

	n = i - 1;
	j = 0;
	found_cluster = ext_boot_record_fat32->root_cluster; // we start traversing the path at root
	do {
		old_found_cluster = found_cluster;
		found_cluster = find_cluster_in_directory( found_cluster, tokens[j] );
		
		if( found_cluster != 0 ) {
			if( n == 0 ) {
				//printf( "Directory of %s\n\n", full_path_to_directory );
				dir_object = get_directory_from_cluster( found_cluster );
				dir_object->name = malloc( sizeof( full_path_to_directory ) );
				strcpy( dir_object->name, full_path_to_directory );
			}
		} else {
			n = -1;
			dir_object = NULL;
			//printf( "Directory not found.\n" );
			// dir_object should return NULL here
		}

 		j++;
		n--;
	} while( n >= 0 );

	return dir_object;
}

Directory * get_directory_from_cluster( uint32_t cluster ) {
	uint8_t					*buffer = malloc( 64 );
	uint32_t 				cluster_to_return = 0;
	uint32_t				cluster_num;
	fat_lfn_t * 			lfn = malloc( sizeof( fat_lfn_t ) );
	fat_directory_t *		dir = malloc( sizeof( fat_directory_t ) );
	char * 					name_fix = malloc( sizeof(char) * 15 );
	unsigned int 			i = 0;
	unsigned int 			z = 0;
	unsigned int 			x = 0;
	bool					show_entry, show_dir;
	Directory * 			dir_object = new Directory();

	dir_object->set_type( FAT32 );

	do {
		show_dir = false;
		show_entry = true;

		buffer = primary_hard_drive->read_bytes( cluster_to_lba( cluster ) * 512 + (ext_boot_record_fat32->table_size_32 * 512 * 2) + i, 64, (buffer) );

		// If the attribute is 0x0F then we have a long file name, otherwise normal
		if( buffer[11] == 0x0F ) {
			lfn = (fat_lfn_t *)buffer;
			dir = (fat_directory_t *)(buffer + 32);

			for( z = 0; z < 5; z++ ) {
				name_fix[z] = lfn->char_group_one[(z * 2)]; 
			}

			for( z = 0; z < 6; z++ ) {
				name_fix[z + 5] = lfn->char_group_two[(z * 2)]; 
			}

			for( z = 0; z < 1; z++ ) {
				name_fix[z + 11] = lfn->char_group_three[(z * 2)]; 
			}

			name_fix[12] = 0;
			i = i + 64;
		} else {
			for( x = 0; x < 15; x++ ) {
				name_fix[x] = 0;
			}
			dir = (fat_directory_t *)(buffer);
			strncat( name_fix, string_trim_inplace(dir->file_name), 8 );
			if( dir->dir ) {
				name_fix[9] = 0;
			} else {
				strcat( string_trim_inplace(name_fix), "." );
				strncat( string_trim_inplace(name_fix), string_trim_inplace(dir->ext), 3 );
				name_fix[14] = 0;
			}
			//strcpy( name_fix, dir->file_name );
			
			i = i + 32;
		}
		
		to_lower( name_fix );

		if( dir->attributes != 0xF ) {
			dir_object->add_entry( name_fix, dir->file_size, dir->attributes );
		}

		/*
		if( dir->dir == 1 ) {
			show_dir = true;
		}

		if( (dir->hidden == 1) || (dir->sys == 1) || (dir->volid == 1) || dir->attributes == 0x0 ) {
			show_entry = false;
		}

		if( show_entry ) {
			if( show_dir ) {
				printf( "\tDIR\t" );
			} else {
				printf( "\t   \t" );
			}

			printf( "0x%02X\t%d\t%s\n", dir->attributes, dir->file_size, name_fix );
		}
		*/

	} while( buffer[0] != 0 );

	return dir_object;
}

DirectoryEntry::DirectoryEntry( char * _name, uint32_t _size, uint8_t _attributes ) {	 
	this->name = malloc( strlen( _name ) + 1 );
	 
	strcpy( this->name, _name );
	 
	this->size = _size;
	 
	this->attributes = _attributes;
}

Directory::Directory( char * _name ) {
	this->name = malloc( strlen( _name ) + 1 );
	strcpy( this->name, _name );
	this->size = 0;
}

void Directory::add_entry( char * _name, uint32_t _size, uint8_t attributes ) {
	this->entry[ this->size ] = new DirectoryEntry( _name, _size, attributes );

	this->size++;
}