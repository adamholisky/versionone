class Serial_Port {
	private:
		uint32_t	port;

	public:

		static const uint32_t COM1 = 0x3F8;
		static const uint32_t COM2 = 0x2F8;
		static const uint32_t COM3 = 0x3E8;
		static const uint32_t COM4 = 0x2E8;

		Serial_Port( uint32_t port_number );
		void write( char c );
		char read( void );
};