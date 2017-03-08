#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "parse.h"

// Look for EOCD ( End first method )
// return EOCD offset in the file
int search_EOCD(FILE *p, int file_size, int EOCD_SIZE){ 
	
	char *ptr;
	char *iobuf = (char *)malloc(EOCD_SIZE*sizeof(char));
	fseek(p, -(EOCD_SIZE),SEEK_END);
	fread(iobuf, 1, EOCD_SIZE, p);
	char *magic = "PK\5\6";
	int EOCD_offset = 0;

	for(ptr = (iobuf+(EOCD_SIZE-22)); ptr >= iobuf; ptr-=4){
		if(memcmp(magic, ptr, 4)==0){
			EOCD_offset =  file_size-(EOCD_SIZE-(ptr-iobuf));
			break;
		}
	}

	free(iobuf);
	return EOCD_offset;
}

// Return total size of CDH entry
int total_CDH_len(CDH_str *s){
	return (s->file_len + s->comment_len + s->extra_len);
}

// Return total size of LFH entry
int total_LFH_len(LFH_str *s){
	return (s->file_len + s->extra_len);
}

// Read EOCD and return struct
EOCD_str read_EOCD(FILE *p, int offset){
	EOCD_str EOCD;
	fseek(p, offset+4, SEEK_SET);
	fread(&EOCD, 1, sizeof(EOCD), p);
	return EOCD;
}

/*
int read_CDHs(FILE *p, file_str **file_list, int CDH_offset, int file_number){

	uint32_t sig;
	int i;

	fseek(p, CDH_offset, SEEK_SET);

	for(i=0; i<file_number; i++){
		fread(&sig, 1, sizeof(sig), p);
		if(sig == CENTRAL_DIRECTORY_SIG){ // CDH signature
			*(CDH_offsets + i) = (ftell(p) - sizeof(sig));
			print(*(CDH_offsets +i));
			fread(&((file_list+i)->CDH), 1, sizeof(CDH_str), p); 
			memset(&sig, 0, 4); // clear the signature
				
			// ALLOCATE MEMORY FOR SIZE-CHANGING CDH VARIABLES //

			(file_list + i)->CDH_name = (char *)malloc(((file_list+i)->CDH.file_len + 1) * sizeof(char));
			(file_list + i)->CDH_extra = (char *)malloc(((file_list+i)->CDH.extra_len) * sizeof(char));
			(file_list + i)->CDH_comment = (char *)malloc(((file_list+i)->CDH.comment_len) * sizeof(char));

			// READ NAME, EXTRA, COMMENT OF EACH FILE //
			
			fread(file_list[i].CDH_name, 1, (size_t)(file_list+i)->CDH.file_len, p); // read file(i) name
			*(file_list[i].CDH_name + (file_list+i)->CDH.file_len)='\0'; // terminate a string with a nul byte
			fread((file_list[i].CDH_extra), 1, (size_t)(file_list+i)->CDH.extra_len, p); // read extras of file(i)
			fread((file_list[i].CDH_comment), 1, (size_t)(file_list+i)->CDH.comment_len, p); // read comments of file(i)
			
			}
		else{
			printf("Warning : [%d] CDH record shoud start at this point but it does not : %ld\n", i, ftell(p));
			printf("Stop processing CDH files. Changing number of files from %d to %d\n", file_number,i);
		//	changed_file_number = 1;
			file_number = i;
		//	puts("Press any key to continue");
			return file_number;
			}
		}
	return 0;
}

int read_LFHs(FILE *p, file_str **file_list, int file_number){

	uint32_t sig;
	int i;

	for(i = 0; i < file_number ; i++){
		fseek(p, (long)((file_list + i)->CDH.LFH_offset), SEEK_SET);
		fread(&sig, 1,  sizeof(sig), p);
		if(sig == LOCAL_FILE_HEADER_SIG){
			fread(&((file_list+i)->LFH), 1, sizeof(LFH_str), p);
			memset(&sig, 0, 4); // clear the signature

			(file_list + i)->LFH_name = (char *)malloc(((file_list+i)->LFH.file_len + 1) * sizeof(char));
			(file_list + i)->LFH_extra = (char *)malloc(((file_list+i)->LFH.extra_len) * sizeof(char));
			fread(file_list[i].LFH_name, 1, (size_t)(file_list+i)->LFH.file_len, p); // read extras of file(i)
			*(file_list[i].LFH_name + (file_list+i)->LFH.file_len) ='\0';
			fread((file_list[i].LFH_extra), 1, (size_t)(file_list+i)->LFH.extra_len, p);		
		}
		else{
			printf("Warning : [%d] LFH record shoud start at this point but it does not : %ld\n", i, ftell(p));
			return 1;
		}	
	}
	return 0;	
}
*/
