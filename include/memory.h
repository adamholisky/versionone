void * malloc( long size );
void initalize_memory( void );
void * get_mem_top ( void );
void * operator new( size_t size );
void * operator new[]( size_t size );
void operator delete( void *p );
void operator delete[]( void *p );

void initalize_paging( void );
void enable_mem_debug( void );