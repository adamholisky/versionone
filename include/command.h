
struct command {
	char * trigger;
	void (* main_function)( void );
	char * desc;
};

void initalize_commands( void );
void launch_command_loop( void );
int iterate_over_commands( char * command_string );
void add_command( char * trigger, void * main_function, char * desc );
void cmd_start_processes( void );
void cmd_process_ab_test( void );
void cmd_print_sizes( void );
void cmd_list_commands( void );
int cmd_list_commands_print( struct node *n );
void cmd_find_scancode( void );
void cmd_mouse_test( void );
void cmd_transfer_to_javascript( void );
void cmd_find_control_test( void );
void cmd_debug_interrupts_on( void );
void cmd_debug_interrupts_off( void );
void peek( uint8_t * address, int lines );