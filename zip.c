#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <inttypes.h>
#include <stdbool.h>

#pragma pack(1)

#include "zip.h"

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

// function looks for EOCD ( End first method )
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

// this function checks the redundant data from CDH and LFH/ one can determine whether the file is corrupted or some steganography technique has been used
bool check_redundancy(CDH_str *CDH, LFH_str *LFH){ 
	bool check = false;
	unsigned int extra_check = 0;
	check += (CDH->version_extract - LFH->version_extract);
	check += (CDH->bit_flag - LFH->bit_flag);
	check += (CDH->compression - LFH->compression);
	check += (CDH->last_mod_time - LFH->last_mod_time);
	check += (CDH->last_mod_date - LFH->last_mod_date);
	check += (CDH->crc32 - LFH->crc32);
	check += (CDH->compr_size - LFH->compr_size);
	check += (CDH->uncompr_size - LFH->uncompr_size);
	check += (CDH->file_len - LFH->file_len);
	
	if(!check){
		extra_check = (CDH->extra_len - LFH->extra_len);
		if(extra_check){
			puts("Extra info does not match");
			return false;
		}
		else{
			puts("Redundant information matches");
			return true;
		}
	}
	puts("Information from CDH and LFH does not match. You should check that");
	return false;
}

// function checks whether the file name length is not a null byte
bool check_null(char *str){ 
	if(strlen(str)){
		puts("Name correct");
		return true;
	}
	else{
		puts("Null byte detected");
		return false;	
	}
}

// prints menu with all the files in the ZIP
void file_menu(file_str *file_list, int file_number){ 
	puts("Print EOCD or choose a file operate on");
	int i = 0;
	puts("[0] Print EOCD structure");
	puts("#### FILES IN THE ZIP FILE ####");
	while(i < file_number){
		printf("[%d] ", i+1);
		print_charstring((file_list + i)->CDH_name);
		i++;
	}
	puts("###############################");
	printf("[%d] Exit\n", i+1);
}

// shows two filenames that should be the same
void compare_names(file_str *file){ 
	printf("CDH filename : "); 
	print_charstring(file->CDH_name);
	printf("LFH filename : ");
	print_charstring(file->LFH_name);
}

// prints possible operations
void show_operations(){ 
	puts("[0] Return");
	puts("[1] Print CDH of the selected file");
	puts("[2] Print LFH of the selected file");
	puts("[3] Check redundant data from CDH and LFH");
	puts("[4] Compare names file names from CDH and LFH"); 
}

// asks the user to execute an operation
int choose_operation(int operations){ 
	int choice = -1;
	while(choice < 0 || choice > operations){
		scanf("%d", &choice);
	}	
	return choice; 
}

void clear(){
	system("cls");
	fflush(stdin);	
}

// executes an operation
void execute_operation(file_str *file_list, int file, int choice, int file_number){ 
	file -=1 ;
	switch(choice){
		case 0 :
			file_menu(&(file_list[file]), file_number);
			break;
		case 1 : 
			print_CDH(&(file_list[file].CDH));
			break;
		case 2 :
			print_LFH(&(file_list[file].LFH));
			break;
		case 3 : 
			check_redundancy(&(file_list[file].CDH), &(file_list[file].LFH));
			break;
		case 4 : 
			compare_names(&(file_list[file]));
			break;
	}
}

#define EOCD_SIZE 65557
#define OPERATIONS 4

int main(int argc,char *argv[]){

	char buffer[256];

	if(argc<2){
		printf("Usage:\na.exe file.zip");
		return 1;
	}

	strncpy(buffer,argv[1],256);
	printf("Opened the file: %s\n", buffer);

	FILE *p;
	p = fopen(buffer, "rb");
	
	if(p == NULL){
		perror("Cannot open the file\n");
		return 2;
	}
	
	puts("File opened successfully\n");
	fseek(p,0,SEEK_END);
	int file_size = ftell(p);
	fseek(p, 0 , SEEK_SET);
	

	// ------- EOCD ------- //
	
	EOCD_str EOCD;
	int EOCD_offset = search_EOCD(p,file_size, EOCD_SIZE);
	int i;
	fseek(p, EOCD_offset+4, SEEK_SET);	
	fread(&EOCD, 1, sizeof(EOCD), p);
	
	// --END EOCD PROCESSING--- //
	
	int file_number = EOCD.number_of_entries; // should equal to the number of files in the ZIP archive
	if(file_number <= 0){
		puts("Wrong number of files! Quitting...");
		return 3;
	}
	
	// memory allocation for appropriate number of file entries
	file_str *file_list = malloc((file_number) * sizeof(file_str));
	if(file_list == NULL){
		puts("Out of memory! Quitting...");
		return -1;
	}

 	// -------- CDH -------- //
	
	uint32_t sig;
	fseek(p, EOCD.CDH_offset, SEEK_SET);

	for(i=0; i<file_number; i++){
		fread(&sig, 1, sizeof(sig), p);
		if(sig == CENTRAL_DIRECTORY_SIG){ // CDH signature
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
			}
		}

	int t = 0;

	while(t < file_number){
		print_charstring(file_list[t].CDH_name);
		t++;
	}

	
	// -------- LFH -------- //
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
		}	
	}	

	int choice = 0;
	int choice1 = 0; 

	while(true){
		
	clear();
	file_menu(file_list, file_number);
	scanf("%d", &choice);
	
	if((choice < 0) || (choice > (file_number+1)))
	{
		continue;
	}	
	if(choice == 0){
		clear();
		print_EOCD(&EOCD);
		getchar();
		puts("Enter to continue");
		continue;
	}	
	if(choice == (file_number+1)){
		exit(1);
	}

	while(true){
		
	clear();
	show_operations();
	choice1 = choose_operation(OPERATIONS);
	if(choice1 == 0){
		break;
	}
	clear();
	execute_operation(file_list, choice, choice1, file_number);
	getchar();
		}
	}
	
	getchar();

	// FREE ALLOCATED MEMORY //
	
	for(i=0; i < file_number ; i++){
		free(file_list[i].CDH_name);
		free(file_list[i].CDH_extra);
		free(file_list[i].CDH_comment);
	}	

	free(file_list);

	fclose(p);

	return 0;
}
