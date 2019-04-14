#define MAX_PROCESSES 256

struct process {
	struct interrupt_stack 	state;
	unsigned int 			active;
};

extern "C" void sys_break( void );
void sys_break_handler( struct interrupt_stack *stack );
void initalize_processes( void );
void start_processes( void );
void start_processes_ab_test( void );
void load_process( void * process_main );
unsigned int get_next_process( void );

