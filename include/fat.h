#define FAT12 			0x00
#define FAT16 			0x01
#define FAT32 			0x02
#define ExFAT 			0x03

#define file_type_file		0x00
#define file_type_directory	0x01

typedef struct fat_BS
{
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	    bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	//unsigned char		extended_section[54];
 
}__attribute__((packed)) fat_BS_t;

typedef struct fat_extBS_32
{
	//extended fat32 stuff
	unsigned int		table_size_32;
	unsigned short		extended_flags;
	unsigned short		fat_version;
	unsigned int		root_cluster;
	unsigned short		fat_info;
	unsigned short		backup_BS_sector;
	unsigned char 		reserved_0[12];
	unsigned char		drive_number;
	unsigned char 		reserved_1;
	unsigned char		boot_signature;
	unsigned int 		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
}__attribute__((packed)) fat_extBS_32_t;
 
typedef struct fat_extBS_16
{
	//extended fat12 and fat16 stuff
	unsigned char		bios_drive_num;
	unsigned char		reserved1;
	unsigned char		boot_signature;
	unsigned int		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
}__attribute__((packed)) fat_extBS_16_t;

typedef struct fat_directory {
	char				file_name[8];
	char				ext[3];
	struct {
		union {
			uint8_t attributes;
			struct {
				uint8_t ro:1;
				uint8_t hidden:1;
				uint8_t sys:1;
				uint8_t volid:1;
				uint8_t dir:1;
				uint8_t archive:1;
				uint8_t unused:2;
			};
		};
	};
	uint8_t				reserved_1;
	uint8_t				creation_time_tenths_second;
	uint16_t			creation_time;
	uint16_t			creation_date;
	uint16_t			last_access_data;
	uint16_t			entry_first_cluster_high;
	uint16_t			modification_time;
	uint16_t			modification_date;
	uint16_t			entry_first_cluster_low;
	uint32_t			file_size;
}__attribute__((packed)) fat_directory_t;

typedef struct fat_lfn {
	uint8_t				order_of_entry;
	char				char_group_one[10];
	uint8_t				attribute;
	uint8_t				long_entry_type;
	uint8_t				checksum;
	char 				char_group_two[12];
	uint16_t 			always_zero;
	char 				char_group_three[2];
}__attribute__((packed)) fat_lfn_t;

class File {
	private:
		char *			path;
		char *			file_name;
		uint32_t		cluster_number;
		uint32_t		size;

		char * 			remove_leading_slash( char * text );

	public:
		File( char * full_path_to_file );
		File( uint32_t _cluster_number, uint32_t _size );

		char * 			get_path( void ) { return this->path; }
		void 			set_path( char * _path ) { this->path = _path; }

		char * 			get_file_name( void ) { return this->file_name; }
		void 			set_file_name( char * _file_name ) { this->file_name = _file_name; }

		uint32_t 		get_size( void ) { return this->size; }
		void 			set_size( uint32_t _size ) { this->size = _size; }

		char * 			read( char * buffer, unsigned int start = 0, int length = -1 );
		//int 			write( char * data, unsigned int start = 0, int length = -1 );
};

class DirectoryEntry {
	public:
		char * 			name;
		uint32_t 		size;
		uint8_t 		attributes;

		DirectoryEntry( void ) { ; }
		DirectoryEntry( char * _name, uint32_t _size, uint8_t _attributes );
};

class Directory {
	public:
		char *				name;
		uint32_t 			size;
		DirectoryEntry * 	entry[50];
		uint32_t 			type;

		Directory( void ) { this->size = 0; }
		Directory( char * _name );

		void 		add_entry( char * _name, uint32_t _size, uint8_t attributes );

		void 		set_type( uint32_t _type ) { this->type = _type; }
		uint32_t 	get_type( void ) { return this->type; }
};

void initalize_fat_fs( void );
void directory_and_file_test( void );
uint32_t cluster_to_lba( uint32_t cluster );
uint32_t find_cluster_in_directory( uint32_t cluster, char * name_to_find );
uint32_t find_size_of_file_in_directory( uint32_t cluster, char * name_to_find );
Directory * get_directory_from_path( char * full_path_to_directory );
Directory * get_directory_from_cluster( uint32_t cluster );
