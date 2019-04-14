#include "kernel.h"

Serial_Port::Serial_Port( uint32_t port_number ) {
	this->port = port_number;

	outportb( this->port + 1, 0x00);    // Disable all interrupts
	outportb( this->port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outportb( this->port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outportb( this->port + 1, 0x00);    //                  (hi byte)
	outportb( this->port + 3, 0x03);    // 8 bits, no parity, one stop bit
	outportb( this->port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outportb( this->port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void Serial_Port::write( char c ) {
	// Make sure the transmit queue is empty
	while( ( inportb( this->port + 5) & 0x20 ) == 0 ) {
		;
	}
 
	outportb( this->port, c );
}

char Serial_Port::read( void ) {
	// Wait until we can get something
	while( ( inportb( this->port + 5) & 1 ) == 0 ) {
		;
	}
 
	return inportb( this->port );
}
