#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/stat.h>

#pragma pack(1)

#include "components.h"
#include "zip.h"
#include "view.h"
#include "parse.h"

struct stat st;
int *CDH_offsets;
int *LFH_offsets;


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
			puts("Extra info does not match. Else is fine.");
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
		return true;
	}
	else{
		puts("\t[!!!] Null byte detected [!!!]");
		return false;	
	}
}


// prints menu with all the files in the ZIP
void file_menu(file_str *file_list, int file_number){ 
	puts("Print EOCD or choose a file operate on");
	int i = 0;
	puts("[0] Print EOCD structure");
	puts("#### FILES IN THE ZIP FILE ####");
	// if it is a large zip then write to a txt file (more than 20 entries)
	if(file_number > 20)
	{
		if(stat("choice.txt",&st)){ // check if the file exists
		FILE* file = fopen("choice.txt","w");
		puts("Creating a file");
		while(i < file_number){
			fprintf(file, "[%d] ", i+1);
			fprintf(file, (file_list + i)->CDH_name);
			fprintf(file, "\n");
			/*fprintf(file, "\n\tCDH EXTRA : ");
			if((file_list + i)->CDH.extra_len){
				fwrite((file_list + i)->CDH_extra, sizeof(char), (file_list + i)->CDH.extra_len, file);
				fprintf(file,"\n");
			}*/
			i++;
		}
		fclose(file);
		puts("*** CHOICES HAVE BEEN WRITTEN TO TXT FILE ***");
		}
	}
	else
	{
	while(i < file_number){
		printf("[%d] ", i+1);
		if(check_null((file_list + i)->CDH_name))
			print_charstring((file_list + i)->CDH_name);
		i++;
	}
	}
	puts("###############################");
	printf("[%d] Exit\n", file_number+1);
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

// executes an operation
void execute_operation(file_str *file_list, int file, int choice, int file_number){ 
	file -=1 ;
	switch(choice){
		case 0 :
			file_menu(&(file_list[file]), file_number);
			break;
		case 1 : 
			print_CDH(&(file_list[file].CDH));
			puts("#### CDH NAME #####");
			print_charstring(file_list[file].CDH_name);
			puts("#### CDH COMMENT #####");
			printtext(file_list[file].CDH_comment, file_list[file].CDH.comment_len);
			puts("#### CDH EXTRA ####");
			printtext(file_list[file].CDH_extra, file_list[file].CDH.extra_len);
			break;
		case 2 :
			print_LFH(&(file_list[file].LFH));
			puts("\n#### LFH NAME #####");
			print_charstring(file_list[file].LFH_name);
			puts("#### LFH EXTRA #####");
			printtext(file_list[file].LFH_extra, file_list[file].LFH.extra_len);
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

// 0 -> unchanged
// 1 -> changed due to CDH
// 2 -> changed due to LFH
// 3 -> changed due to EOCD
unsigned int changed_file_number = 0;


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
	
	int size_of_EOCD = 0;
	if(file_size < EOCD_SIZE){ size_of_EOCD = file_size; }
	else{ size_of_EOCD = EOCD_SIZE; }
	
	// ------- EOCD ------- //
	
	EOCD_str EOCD;
	int EOCD_offset = search_EOCD(p,file_size, size_of_EOCD); // search EOCD offset in the file
	EOCD = read_EOCD(p,EOCD_offset);
	int i;
	/*fseek(p, EOCD_offset+4, SEEK_SET);	
	fread(&EOCD, 1, sizeof(EOCD), p);
	*/
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

	// Values that will be used to determine ZIP integrity
	CDH_offsets = (int*)malloc((file_number) * sizeof(int));
	LFH_offsets = (int*)malloc((file_number) * sizeof(int));

 	// -------- CDH -------- //
	
	//read_CDHs(p,&file_list,EOCD.CDH_offset,file_number);
	uint32_t sig;

	fseek(p, EOCD.CDH_offset, SEEK_SET);

	for(i=0; i<file_number; i++){
		fread(&sig, 1, sizeof(sig), p);
		if(sig == CENTRAL_DIRECTORY_SIG){ // CDH signature
			//*(CDH_offsets + i) = (ftell(p) - sizeof(sig));
			//print(*(CDH_offsets +i));
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
			break;
			}
		}

	/*int t = 0;
	
	while(t < file_number){
		print_charstring(file_list[t].CDH_name);
		t++;
	}
	*/

	
	// -------- LFH -------- //
	
	//read_LFHs(p, &file_list,file_number);
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
	
	if((choice < 0) || (choice > (file_number+1))) // WRONG CHOICE
	{
		continue;
	}	
	if(choice == 0){ // PRINT EOCD
		clear();
		print_EOCD(&EOCD);
		getchar();
		puts("Enter to continue");
		continue;
	}	
	if(choice == (file_number+1)){ // EXIT PROGRAM

		// FREE ALLOCATED MEMORY //
	
		for(i=0; i < file_number ; i++)
		{
		free(file_list[i].CDH_name);
		free(file_list[i].CDH_extra);
		free(file_list[i].CDH_comment);
		}	

		free(file_list);

		// FREE NEW
		free(CDH_offsets);
		free(LFH_offsets);

		remove("choice.txt");
		fclose(p);
		return 0;
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

	/*
	EXIT IN THE MENU

	// FREE ALLOCATED MEMORY //
	
	for(i=0; i < file_number ; i++){
		free(file_list[i].CDH_name);
		free(file_list[i].CDH_extra);
		free(file_list[i].CDH_comment);
	}	

	free(file_list);
	remove("choice.txt");
	fclose(p);
	*/
	return 0;
}
