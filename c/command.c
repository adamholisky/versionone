#include "kernel.h"
#include <string.h>
#include <stdlib.h>

#define __builtin_strtok( a, b ) strtok( a, b )

struct node *command_list;

char * working_dir;

char * tokens[10];

void run_it( void );
void cmd_peek( void );
void cmd_read( void );
char peek_char( char c );
void cmd_shutdown( void );
void cmd_cat( void );
void cmd_ls( void );
void cmd_cd( void );
void cmd_pwd( void );

/**
 * @brief      Setups the command line options for later use.
 */
void initalize_commands( void ) {
	working_dir = malloc( 256 );

	working_dir[0] = '/';

	add_command( "start_processes", &cmd_start_processes, "Begins the primary process test." );
	add_command( "process_ab_test", &cmd_process_ab_test, "Classic AB process test, runs infinitely." );
	add_command( "print_sizes", &cmd_print_sizes, "Prints sizes of common variable types." );
	add_command( "find_scancode", &cmd_find_scancode, "Returns the pressed scancode." );
	add_command( "mouse_test", &cmd_mouse_test, "Mouse testing." );
	add_command( "?", &cmd_list_commands, "Lists out the available commands." );
	add_command( "js", &cmd_transfer_to_javascript, "Transfers control to JavaScript." );
	add_command( "find_control_test", &cmd_find_control_test, "Tests findinga a control at pre-defined points." );
	add_command( "debug interrupts on", &cmd_debug_interrupts_on, "Turn on interrupt debugging output." );
	add_command( "debug interrupts off", &cmd_debug_interrupts_off, "Turn off interrupt debugging output." );
	add_command( "runit", &run_it, "Runs a pre-compiled binary file." );
	add_command( "peek", &cmd_peek, "Looks at a memory address." );
	add_command( "read", &cmd_read, "Reads a sector on the primary hard drive." );
	add_command( "q", &cmd_shutdown, "Special signal to quit QEMU." );
	add_command( "cat", &cmd_cat, "Outputs in the given file." );
	add_command( "ls", &cmd_ls, "Outputs the contents of a directory." );
	add_command( "cd", &cmd_cd, "Changes the working directory." );
	add_command( "pwd", &cmd_pwd, "Prints the working directory." );
}

/**
 * @brief      Starts our basic command line loop. This should never return.
 */
void launch_command_loop( void ) {
	char * command_string = malloc( sizeof(char) * 255 );
	char * token_ptr = NULL;
	int  i = 0;
	int  pos = 0;
	int  pos_temp = 0;
	char * original_string = NULL;

	while( 1 ) {
		token_ptr = NULL;
		i = 0;
		pos = 0;
		pos_temp = 0;
		original_string = NULL;

		for( int j = 0; j < 10; j++ ) {
			tokens[j] = NULL;
		}
 
		strcpy( command_string, "" );
 
		printf( "versionOne: " );
		get_string( command_string, 255 );
 
		//original_string = malloc( sizeof(char) * strlen( command_string ) + 1 );
		//strcpy( original_string, command_string );
		token_ptr = strtok( command_string, " ," );
 
		while( token_ptr != NULL && i < 10 ) {
			tokens[i] = malloc( strlen( token_ptr ) + 1 );
			strcpy( tokens[i], token_ptr );
			i++;
			token_ptr = strtok( NULL, " ," );
		}
 
		if( iterate_over_commands( tokens[0] ) == 0 ) {
			printf( "Command \"%s\" not found. Use ? to list commands.\n", tokens[0] );
		}
	}
}

/**
 * @brief      Given command_string, find the available command and execute it.
 *
 * @param      command_string  The command the user, or whatever, wants to execute
 *
 * @return     0 == unsuccessful. Anything else == success.
 */
int iterate_over_commands( char * command_string ) {
	int ret_val = 0;

	struct node *list = command_list;

	while( list != NULL ) {
		struct command * current_command;

		current_command = (struct command * )list->data;

		//printf( "\ncs: \"%s\", trig: \"%s\", loc: 0x%08X\n", command_string, current_command->trigger, current_command->main_function );

		if( strcmp(current_command->trigger, command_string) == 0 ) {
			void (* function)( void )= current_command->main_function;

			function();

			ret_val = 1;

			list = NULL;
		} else {
			list = list->next;
		}
	}

	return ret_val;
}

/**
 * @brief      Adds a command to the available command pool.
 *
 * @param      _trigger        String that the user would type to trigger the command.
 * @param[in]  _main_function  Function pointer to the command.
 * @param      _desc           Brief description of the command to show at the help prompt.
 */
void add_command( char * _trigger, void (* _main_function), char * _desc ) {
	//printf( "Adding %s %d at 0x%08X\n", _trigger, strlen(_trigger), _main_function );

	struct command * new_command = malloc( sizeof(struct command) );

	new_command->trigger = malloc( strlen(_trigger) + 1 );
	strcpy( new_command->trigger, _trigger );

	new_command->desc = malloc( strlen(_desc) + 1 );
	strcpy( new_command->desc, _desc );

	new_command->main_function = _main_function;

	list_push( &command_list, new_command, sizeof(struct command) );

	//printf( "Added %s %d at 0x%08X\n", new_command->trigger, strlen(new_command->trigger), new_command->main_function );
}

/**
 * @brief      Starts the process architecture.
 */
void cmd_start_processes( void ) {
	start_processes();
}

/**
 * @brief      Runs an infinite AB-repeating test of the process architecture.
 */
void cmd_process_ab_test( void ) {
	start_processes_ab_test();
}

/**
 * @brief      Print out sizes (in bytes) of given variables. Useful for debugging size mismatches.
 */
void cmd_print_sizes( void ) {
	printf( "\n" );
	printf( "unsigned char:             %d\n", sizeof( unsigned char ) );
	printf( "unsigned short:            %d\n", sizeof( unsigned short ) );
	printf( "unsigned int:              %d\n", sizeof( unsigned int ) );
	printf( "unsigned long:             %d\n", sizeof( unsigned long ) );
	printf( "unsigned long long:        %d\n", sizeof( long long ) );
	printf( "uint8_t:                   %d\n", sizeof( uint8_t ) );
	printf( "uint16_t:                  %d\n", sizeof( uint16_t ) );
	printf( "uint32_t:                  %d\n", sizeof( uint32_t ) );
	printf( "uint64_t:                  %d\n", sizeof( uint64_t ) );
	printf( "\n\n" );
}

/**
 * @brief      Iterate over the command list and prints out all the commands.
 */
void cmd_list_commands( void ) {
	printf( "\nAvailable commands:\n\n" );
	list_for_each( command_list, &cmd_list_commands_print );
	printf( "\n" );
}

/**
 * @brief      Prints a command that's saved in the given node n.
 *
 * @param      n     A node in a list that contains a pointer to a command struct.
 *
 * @return     Always returns 0, for now.
 */
int cmd_list_commands_print( struct node * n ) {
	struct command *c = (struct command *)n->data;

	printf( "%s -- %s\n", c->trigger, c->desc );

	return 0;
}

/**
 * @brief      Prints the scancode of the key pressed. Useful for fast debugging in the OS.
 */
void cmd_find_scancode( void ) {
	char scancode;

	printf( "Enter key for scancode readout. Press ESC to quit.\n" );

	scancode = get_scancode();

	while( scancode != 0x01 ) {
		if( scancode != 0 ) {
			printf( "Scancode: 0x%02X %d\n", scancode, scancode );
		}

		scancode = get_scancode();
	}

	printf( "Done finding scancode.\n" );
}

/**
 * @brief      Dumping ground for mouse tests.
 */
void cmd_mouse_test( void ) {
	uint32_t x = 0;
	uint32_t y = 0;
	char scancode;

	draw_mouse( 1280/2, 720/2 );

	printf( "Use arrow keys to move mouse. Press ESC to quit.\n" );

	scancode = get_scancode();

	while( scancode != 0x01 ) {
		if( scancode == 0x9 ) {
			x++;
			draw_mouse( 1280/2 + x, 720/2 + y );
		}

		if( scancode == 0xA ) {
			x--;
			draw_mouse( 1280/2 + x, 720/2 + y );
		}

		//scancode = get_scancode();
		scancode = inportb( 0x60 );
		if( scancode != 0xFFFFFF9C ) {
			//printf( "sc: 0x%X", scancode );
		}
	}

	printf( "Done with mouse test.\n" );
}

/**
 * @brief      Once we get Duktape working, this will trigger the entrance into JavaScript land.
 */
void cmd_transfer_to_javascript( void ) {
	/*
	duk_context *ctx = duk_create_heap_default();
	duk_eval_string(ctx, "1+2");
	printf("1+2=%d\n", (int) duk_get_int(ctx, -1));
	duk_destroy_heap(ctx);
	*/
}

void cmd_find_control_test( void ) {

}

void cmd_debug_interrupts_on( void ) {
	set_debug_interrupts( true );
}

void cmd_debug_interrupts_off( void ) {
	set_debug_interrupts( false );
}

void run_it( void ) {
	debug_f( "Attempting to run loaded program...\n" );
	void (* main)(void) = 0x00000000;
	main();
}

void cmd_peek( void ) {
	char str_address[255];
	char lines = 8;
	bool send_to_debug = false;

	if( tokens[1] == NULL ) {
		printf( "Usage: peek <hex address> <lines> <debug>\n" );
		return;
	}

	uint32_t address = hex2int( tokens[1], strlen( tokens[1] ) );

	if( tokens[2] != NULL ) {
		lines = atoi( tokens[2] );
	}

	if( tokens[3] != NULL ) {
		send_to_debug = true;
	}

	printf( "Looking at 0x%08X\n", address );
	
	uint8_t *ptrn = (uint8_t *)address;
	char *ptr = (char *)address;

	for( int x = 0; x < lines; x++ ) {
		if( send_to_debug ) {
			debug_f( "0x%08X    %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X    %c%c%c%c%c%c%c%c%c%c\n",
			address + (x * 10), 
			*(ptrn), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), *(ptrn + 8), *(ptrn + 9), 
			peek_char( *(ptr) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ), peek_char( *(ptr + 8) ), peek_char( *(ptr + 9) ) );
		} else {
			printf( "0x%08X    %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X    %c%c%c%c%c%c%c%c%c%c\n",
			address + (x * 10), 
			*(ptrn), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), *(ptrn + 8), *(ptrn + 9), 
			peek_char( *(ptr) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ), peek_char( *(ptr + 8) ), peek_char( *(ptr + 9) ) );
		}

		ptr = ptr + 10;
		ptrn = ptrn + 10;
	}

	printf( "\n" );
}

char peek_char( char c ) {
	if( (c > 31) && (c < 127) ) {
		return c;
	} else {
		return '.';
	}
}

void cmd_read( void ) {
	char str_address[255];
	char lines = 8;
	bool send_to_debug = false;

	if( tokens[1] == NULL ) {
		printf( "Usage: peek <sector> <lines> <debug>\n" );
		return;
	}

	uint32_t sector = hex2int( tokens[1], strlen( tokens[1] ) );

	if( tokens[2] != NULL ) {
		lines = atoi( tokens[2] );
	}

	if( tokens[3] != NULL ) {
		send_to_debug = true;
	}

	printf( "Reading sector 0x%08X\n", sector );

	HardDrive *hd = get_primary_hard_drive();
	uint8_t * data = hd->read_sector( sector );
	
	uint8_t *ptrn = data;
	char *ptr = (char *)data;

	uint8_t * byte_data = malloc( sizeof( uint8_t ) * 10 );
	byte_data = hd->read_bytes( (sector * 512) + 3, 10, byte_data );

	for( int x = 0; x < lines; x++ ) {
		if( send_to_debug ) {
			debug_f( "0x%04X    %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X    %c%c%c%c%c%c%c%c%c%c\n",
			(x * 10), 
			*(ptrn), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), *(ptrn + 8), *(ptrn + 9), 
			peek_char( *(ptr) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ), peek_char( *(ptr + 8) ), peek_char( *(ptr + 9) ) );
			
			if( x == lines - 1 ) {
				debug_f( "\n10 bytes, offset 3: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", *(byte_data), *(byte_data + 1), *(byte_data + 2), *(byte_data + 3), *(byte_data + 4), *(byte_data + 5), *(byte_data + 6), *(byte_data + 7), *(byte_data + 8), *(byte_data + 9) );
			}
		} else {
			printf( "0x%04X    %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X    %c%c%c%c%c%c%c%c%c%c\n",
			(x * 10), 
			*(ptrn), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), *(ptrn + 8), *(ptrn + 9), 
			peek_char( *(ptr) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ), peek_char( *(ptr + 8) ), peek_char( *(ptr + 9) ) );

			if( x == lines - 1 ) {
				printf( "\n10 bytes, offset 3: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", *(byte_data), *(byte_data + 1), *(byte_data + 2), *(byte_data + 3), *(byte_data + 4), *(byte_data + 5), *(byte_data + 6), *(byte_data + 7), *(byte_data + 8), *(byte_data + 9) );
			}
		}

		ptr = ptr + 10;
		ptrn = ptrn + 10;
	}

	printf( "\n" );
}

void cmd_shutdown( void ) {
	debug_f( "Shutting down." );
	outportb( 0xF4, 0x00 );
}

void peek( uint8_t * address, int lines ) {
	debug_f( "Looking at 0x%08X\n", address );
	
	uint8_t *ptrn = address;
	char *ptr = (char *)address;

	for( int x = 0; x < lines; x++ ) {
		debug_f( "0x%08X    %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X    %c%c%c%c%c%c%c%c%c%c\n",
		address + (x * 10), 
		*(ptrn), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), *(ptrn + 8), *(ptrn + 9), 
		peek_char( *(ptr) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ), peek_char( *(ptr + 8) ), peek_char( *(ptr + 9) ) );

		ptr = ptr + 10;
		ptrn = ptrn + 10;
	}

	debug_f( "\n" );
}

void cmd_cat( void ) {
	if( tokens[1] == NULL ) {
		printf( "Usage: cat <file name>\n" );
		return;
	}
 
	char * file_buffer;
	File * test_file = new File( tokens[1] );
	file_buffer = malloc( test_file->get_size() );
	file_buffer = test_file->read( file_buffer );
	printf( "%s\n", file_buffer );

}

void cmd_ls( void ) {
	Directory *		dir = NULL;
	bool 			show_entry;
	uint32_t 		file_type;
	char 			* path = malloc( 256 );

	dv( tokens[1] );

	if( tokens[1] == NULL ) {
		strcpy( path, working_dir );
	} else {
		strcpy( path, tokens[1] );
	}

	dir = get_directory_from_path( path );

	if( dir != NULL ) {
		printf( "Directory of %s\n\n", dir->name );

		for( int x = 0; x < dir->size; x++ ) {
			show_entry = true;
			file_type = file_type_file;

			DirectoryEntry *e = dir->entry[ x ];

			if( dir->type == FAT32 ) {
				// Does it have a '~' in it?
				if( strstr( e->name, "~" ) != NULL ) {
					show_entry = false;
				}

				if( e->attributes == 0x0 || (e->attributes & 0x04) || e->attributes & 0x08 || (e->attributes & (0x01|0x02|0x04|0x08) ) ) {
					show_entry = false;
				}

				if( e->attributes & 0x10 ) {
					file_type = file_type_directory;
				}
			}

			if( show_entry ) {
				printf( "\t" );

				switch( file_type ) {
					case file_type_directory:
						break;
					default:
						printf( "%d", e->size );
				}

				printf( "\t" );

				switch( file_type ) {
					case file_type_file:
						printf( "FILE" );
						break;
					case file_type_directory:
						printf( "DIR" );
						break;
				}

				

				printf( "\t%s\n", e->name );
			}
		}
	} else {
		printf( "Directory not found.\n" );
	}
}

void cmd_pwd( void ) {
	printf( "%s\n", working_dir );
}

void cmd_cd( void ) {
	char		* path = malloc( 255 );
	char 		* wd = malloc( 255 );
	char 		* wd_reassemble = malloc( 255 );
	char * 		original_wd;
	char * 		arg;
	char *		tokens_path[10];
	char *		token_ptr;
	char * 		tokens_wd[10];
	char *		token_ptr_wd;
	char * 		wd_ptr;
	int 		h, i, n, j, x, y;
	bool 		ignore_first = true;

	strcpy( original_wd, working_dir );
	
	if( tokens[1] == NULL ) {
		strcpy( working_dir, "/" );
		return;
	} else {
		arg = tokens[1];
		
		if( strcmp( arg, "/" ) == 0 ) {
			strcpy( working_dir, "/" );
			return;
		}
		
		for( int x = 0; x < 255; x++ ) {
			path[ x ] = 0;
		}
		
		strcpy( path, arg );
		
		// First tokenize the path we're given
		
	   	to_lower( path );
		
		if( strlen( path ) > 2 && path[0] == '/' ) {
			for( x = 0; x < strlen( path ) - 1; x++ ) {
				path[x] = path[x + 1];
			}
			
			path[x] = 0;
		}
		
		for( j = 0; j < 10; j++ ) {
			tokens_path[j] = NULL;
		}
		
		token_ptr = strtok( path, "/" );
		
		while( token_ptr != NULL && i < 10 ) {
			tokens_path[i] = malloc( strlen( token_ptr ) + 1 );
			strcpy( tokens_path[i], token_ptr );
						
			i++;
			token_ptr = strtok( NULL, "/" );
		}

		// Second tokenize the working directory
		
		strcpy( wd, working_dir );
		h = 0;
				
		if( strlen( wd ) > 0 && wd[0] == '/' ) {
			for( x = 0; x < strlen( wd ) - 1; x++ ) {
				wd[x] = wd[x + 1];
			}
			
			wd[x] = 0;
		}
		
		for( j = 0; j < 10; j++ ) {
			tokens_wd[j] = NULL;
		}
		
		if( wd[0] != 0 ) {
			token_ptr_wd = strtok( wd, "/" );
			
			while( token_ptr_wd != NULL && h < 10 ) {
				tokens_wd[h] = malloc( strlen( token_ptr_wd ) + 1 );
				strcpy( tokens_wd[h], token_ptr_wd );
								
				h++;
				token_ptr_wd = strtok( NULL, "/" );
			}
		}
		
		// Third traverse the new tokens, apply changes to the working directory
		
		n = 0;
		
		while( tokens_path[ n ] != NULL ) {
			if( strcmp( tokens_path[ n ], "." ) == 0 ) {
				// do nothing
			} else if( strcmp( tokens_path[ n ], ".." ) == 0 ) {
				// go up a dir
				if( h != 0 ) {
					tokens_wd[ h ] = NULL;
					h--;
				}
			} else {
				if( h < 10 ) {
					tokens_wd[ h ] = malloc( strlen( tokens_path[ n ] ) + 1 );
					strcpy( tokens_wd[ h ], tokens_path[ n ] );
					h++;
				}
			}
			n++;
		}		
		
		// Forth reassemble the working directory
		wd_ptr = wd_reassemble;
		x = 1;
		h--;
		y = h;
		
		// We're traversing back to root
		if( h == -1 ) {
			h = 0;
			y = 0;
			
			tokens_wd[ 0 ] = malloc( 1 );
			strcpy( tokens_wd[ 0 ], "" );
		}
		
		while( h != -1 ) {
			*(wd_reassemble) = '/';
			wd_reassemble++;
			
			strcpy( wd_reassemble, tokens_wd[ y - h ] );
			wd_reassemble += strlen( tokens_wd[ y - h ] );
						
			h--;
		}
		
		wd_reassemble++;
		*(wd_reassemble) = '0';

		wd_reassemble = wd_ptr;

		strcpy( working_dir, wd_reassemble );

		if( get_directory_from_path( wd_reassemble ) == NULL ) {
			printf( "Directory not found.\n" );
			strcpy( working_dir, original_wd );
		}
	}
}