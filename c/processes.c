#include "kernel.h"

#define PROCESS_DEBUG 0
#define PROCESS_DEBUG_LENGTH 10

void task_a( void );
void task_b( void );
void task_c( void );

void test_ab_a( void );
void test_ab_b( void );

struct process processes[ MAX_PROCESSES ];

int	current_process;

/* Setup our initial process environment, load the first processes 
 */
void initalize_processes( void ) {
	for( int i = 0; i < MAX_PROCESSES; i++ ) {
		processes[ i ].active = 0;
		processes[ i ].state.eip = 0;
	}

	current_process = 0;
}

void start_processes( void ) {
	load_process( task_a );
	load_process( task_b );
	load_process( task_c );

	task_a();
}

void start_processes_ab_test( void ) {
	load_process( test_ab_a );
	load_process( test_ab_b );

	test_ab_a();
}

/* Given an entry point, set the process to active and initalize it's environment 
 */
void load_process( void * process_main ) {
	int x;

	for( x = 0; x < MAX_PROCESSES; x++ ) {
		if( processes[ x ].active == 0 ) {
			processes[ x ].active = 1;
			processes[ x ].state.eip = (unsigned long)process_main;

			break;
		}
	}

	debug_f( "Loading Process: %d starting at %08X on list at %08X\n", x, processes[ x ].state.eip, processes);
}

/* Finds the next process to run
 */
unsigned int get_next_process( void ) {
	int x;
	bool have_process = false;

	for( x = current_process + 1; ( x < MAX_PROCESSES ) && ( have_process == false ); x++ ) {
		if( processes[ x ].active == 1 ) {
			have_process = true;
			break;
		}
	}

	if( have_process == false ) {
		for( x = 0; ( x < current_process ) && ( have_process == false ); x++ ) {
			if( processes[ x ].active == 1 ) {
				have_process = true;
				break;
			}
		}
	}

	if( have_process == false ) {
		printf( "Cannot find process. Halting.\n" );
		while( 1 ) {
			;
		}
	}

	return x;
}

/* Handles the system break, called from the interrupt service handler
 */
void sys_break_handler( struct interrupt_stack *stack ) {
	unsigned int next_process;

	if( PROCESS_DEBUG ) {
		printf( "--==Process testing called==--\n" );
		printf( "  eax:  0x%08X  ebx:  0x%08X  ecx:  0x%08X  edx:  0x%08X\n", stack->eax, stack->ebx, stack->ecx, stack->edx );
		printf( "  esp:  0x%08X  ebp:  0x%08X  esi:  0x%08X  edi:  0x%08X\n", stack->esp, stack->ebp, stack->esi, stack->edi );
		printf( "  ds:   0x%04X  es:   0x%04X  fs:   0x%04X  gs:   0x%04X\n", stack->ds, stack->es, stack->fs, stack->gs );
		printf( "  eip:  0x%08X\n", stack->eip );
	}

	next_process = get_next_process();

	if( PROCESS_DEBUG ) {
		printf_console( "second-console", "current process: %d", current_process );
		printf_console( "second-console", "  next process: %d\n", next_process );
	}

	processes[ current_process ].state.eax = stack->eax;
	processes[ current_process ].state.ebx = stack->ebx;
	processes[ current_process ].state.ecx = stack->ecx;
	processes[ current_process ].state.edx = stack->edx;
	processes[ current_process ].state.esp = stack->esp;
	processes[ current_process ].state.ebp = stack->ebp;
	processes[ current_process ].state.esi = stack->esi;
	processes[ current_process ].state.edi = stack->edi;
	processes[ current_process ].state.ds = stack->ds;
	processes[ current_process ].state.es = stack->es;
	processes[ current_process ].state.fs = stack->fs;
	processes[ current_process ].state.gs = stack->gs;
	processes[ current_process ].state.eip = stack->eip;

	current_process = next_process;

	stack->eax = processes[ current_process ].state.eax;
	stack->ebx = processes[ current_process ].state.ebx;
	stack->ecx = processes[ current_process ].state.ecx;
	stack->edx = processes[ current_process ].state.edx;
	stack->esp = processes[ current_process ].state.esp;
	stack->ebp = processes[ current_process ].state.ebp;
	stack->esi = processes[ current_process ].state.esi;
	stack->edi = processes[ current_process ].state.edi;
	stack->ds = processes[ current_process ].state.ds;
	stack->es = processes[ current_process ].state.es;
	stack->fs = processes[ current_process ].state.fs;
	stack->gs = processes[ current_process ].state.gs;
	stack->eip = processes[ current_process ].state.eip;
}

void test_ab_a( void ) {
	long x = 0;
	while( 1 ) {
		printf( "A" );
		while( x < 2000000 ) {
			x++;
		}
		x = 0;
		sys_break();
	}
}

void test_ab_b( void ) {
	long x = 0;
	while( 1 ) {
		printf( "B" );
		while( x < 2000000 ) {
			x++;
		}
		x = 0;
		sys_break();
	}
}

void task_a( void ) {
	int x;

	x = 0;

	while ( x < PROCESS_DEBUG_LENGTH ) {
		printf( "A" );
		x++;
		sys_break();
	}

	while ( 1 ) {
		//sys_break();
	}
}

void task_b( void ) {
	int x;

	x = 0;

	while ( x < PROCESS_DEBUG_LENGTH ) {
		printf( "B" );
		x++;
		sys_break();
	}

	while ( 1 ) {
		//sys_break();
	}
}

void task_c( void ) {
	int x;

	x = 0;

	while ( x < PROCESS_DEBUG_LENGTH ) {
		printf( "C " );
		x++;
		sys_break();
	}

	while ( 1 ) {
		//sys_break();
	}
}