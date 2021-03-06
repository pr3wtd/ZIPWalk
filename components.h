#ifndef components_h
#define components_h

struct __attribute__((packed)) EOCD {
	uint16_t number_of_disk;
	uint16_t number_of_disk_CDH;
	uint16_t number_of_entries_disk;
	uint16_t number_of_entries;
	uint32_t CDH_size;
	uint32_t CDH_offset;
};

typedef struct __attribute((packed)) EOCD EOCD_str;

struct __attribute__((packed)) CDH {
	uint16_t version;
	uint16_t version_extract;
	uint16_t bit_flag;
	uint16_t compression;
	uint16_t last_mod_time;
	uint16_t last_mod_date;
	uint32_t crc32;
	uint32_t compr_size;
	uint32_t uncompr_size;
	uint16_t file_len;
	uint16_t extra_len;
	uint16_t comment_len;
	uint16_t disk_number;
	uint16_t inter_attr;
	uint32_t exter_attr;
	uint32_t LFH_offset;
};

typedef struct __attribute((packed)) CDH CDH_str;

struct __attribute__((packed)) LFH {
	uint16_t version_extract;
	uint16_t bit_flag;
	uint16_t compression;
	uint16_t last_mod_time;
	uint16_t last_mod_date;
	uint32_t crc32;
	uint32_t compr_size;
	uint32_t uncompr_size;
	uint16_t file_len;
	uint16_t extra_len;	
};

typedef struct __attribute((packed)) LFH LFH_str;

struct __attribute__((packed)) file {
	CDH_str CDH;
	char *CDH_name;
	char *CDH_extra;
	char *CDH_comment;
	LFH_str LFH;
	char *LFH_name;
	char *LFH_extra;

};

typedef struct __attribute((packed)) file file_str;

#endif
