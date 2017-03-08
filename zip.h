#ifndef zip_h
#define zip_h

#include <stdbool.h>

enum signatures {
	LOCAL_FILE_HEADER_SIG = 0x04034B50,
	CENTRAL_DIRECTORY_SIG = 0x02014B50,
	END_OF_CENTRAL_DIRECTORY_SIG = 0x06054B50,
};

bool check_redundancy(CDH_str *CDH, LFH_str *LFH);
bool check_null(char *str);
void file_menu(file_str *file_list, int file_number);
void compare_names(file_str *file);
void show_operations();
void execute_operation(file_str *file_list, int file, int choice, int file_number);
int search_EOCD(FILE *p, int file_size, int EOCD_SIZE);
int choose_operation(int operations);
void printtext(char *text, int size);

#endif
