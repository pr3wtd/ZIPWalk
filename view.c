#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>

#include "components.h"
#include "view.h"

void clear(){
	system("cls");
	fflush(stdin);	
}

void print(int a){ // prints an integer

	printf("%d\n", a);
}

void print_charstring(char *a){ // check the lenght of the name to avoid null byte attack
	char *pos = a;
	while(*pos != '\0'){
		printf("%c", *(pos++));
	}
	printf("\n");
}

void printtext(char *text, int size){
	int i;
	for(i=0; i < size; i++){
		putchar(*(text + i));
	}
}

void print_EOCD(EOCD_str *ptr){
	puts("------END OF CENTRAL DIRECTORY------\n\n");
	printf("# of disk: " "%" PRIu16 "\n", ptr->number_of_disk);
	printf("# of disk with the start of central directory: " "%" PRIu16 "\n", ptr->number_of_disk_CDH);
	printf("# of entries in the central directory on this disk: " "%" PRIu16 "\n", ptr->number_of_entries_disk);
	printf("# of entries in the central directory: " "%" PRIu16 "\n", ptr->number_of_entries);
	printf("Size of the CDH: " "%" PRIu32 "\n", ptr->CDH_size);
	printf("CDH offset with respect to the starting disk number: " "%" PRIu32 "\n", ptr->CDH_offset);
}

void print_CDH(CDH_str *ptr){
	puts("------CENTRAL DIRECTORY HEADER------\n");
	printf("Version made by : " "%" PRIu16 "\n\n", ptr->version);
	puts("---Redundant data from LFH header---");
	printf("Version needed to extract  : " "%" PRIu16 "\n", ptr->version_extract);
	printf("General purpose bit flag : " "%" PRIu16 "\n", ptr->bit_flag);
	printf("Compression method : " "%" PRIu16 "\n", ptr->compression);
	printf("Last mod file time : " "%" PRIu16 "\n", ptr->last_mod_time);
	printf("Last mod file date : " "%" PRIu16 "\n", ptr->last_mod_date);
	printf("CRC-32 : " "%" PRIu32 "\n", ptr->crc32);
	printf("Compressed size : " "%" PRIu32 "\n", ptr->compr_size);
	printf("Uncompressed size : " "%" PRIu32 "\n", ptr->uncompr_size);
	printf("File name length : " "%" PRIu16 "\n", ptr->file_len);
	printf("Extra field length : " "%" PRIu16 "\n", ptr->extra_len);
	puts("---End of redundant data---\n");
	printf("File comment length : " "%" PRIu16 "\n", ptr->comment_len);
	printf("Disk number start : " "%" PRIu16 "\n", ptr->disk_number);
	printf("Internal file attributes : " "%" PRIu16 "\n", ptr->inter_attr);
	printf("External file attributes : " "%" PRIu32 "\n", ptr->exter_attr);
	printf("Relative OFFSET of local header : " "%" PRIu32 "\n\n", ptr->LFH_offset);
}

void print_LFH(LFH_str *ptr){
	puts("------LOCAL FILE HEADER------\n");
	printf("Version needed to extract  : " "%" PRIu16 "\n", ptr->version_extract);
	printf("General purpose bit flag : " "%" PRIu16 "\n", ptr->bit_flag);
	printf("Compression method : " "%" PRIu16 "\n", ptr->compression);
	printf("Last mod file time : " "%" PRIu16 "\n", ptr->last_mod_time);
	printf("Last mod file date : " "%" PRIu16 "\n", ptr->last_mod_date);
	printf("CRC-32 : " "%" PRIu32 "\n", ptr->crc32);
	printf("Compressed size : " "%" PRIu32 "\n", ptr->compr_size);
	printf("Uncompressed size : " "%" PRIu32 "\n", ptr->uncompr_size);
	printf("File name length : " "%" PRIu16 "\n", ptr->file_len);
	printf("Extra field length : " "%" PRIu16 "\n", ptr->extra_len);
}
