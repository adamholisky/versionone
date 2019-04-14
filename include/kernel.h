#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "types.h"
#include "base.h"
#include "serial.h"
#include "term.h"
#include "ata.h"
#include "afs.h"
#include "ext2.h"
#include "fat.h"
#include "vga.h"
#include "gui/interfaces.h"
#include "gui/control.h"
#include "gui/container.h"
#include "gui/window.h"
#include "gui/console.h"
#include "gui/button.h"
#include "gui.h"
#include "interrupts.h"
#include "memory.h"
#include "keyboard.h"
#include "processes.h"
#include "command.h"
#include "multiboot.h"
#include "timer.h"

#define kernel_memory_base  0xC0000000
#define vga_memory_base		0xE0000000

#define d1 debug_f( "D-1\n" );
#define d2 debug_f( "D-2\n" );
#define d3 debug_f( "D-3\n" );
#define d4 debug_f( "D-4\n" );
#define d5 debug_f( "D-5\n" );

#define dA debug_f( "D-A\n" );
#define dB debug_f( "D-B\n" );
#define dC debug_f( "D-C\n" );
#define dD debug_f( "D-D\n" );
#define dE debug_f( "D-E\n" );

#define dv(x) debug_f( "[Debug Variable] %s = %s : %d : 0x%X\n", #x, x, x, x )