#ifndef parse_h
#define parse_h

#include "components.h"
//#include "zip.h"

int search_EOCD(FILE*,int,int);
int total_CDH_len(CDH_str*);
int total_LFH_len(LFH_str*);
EOCD_str read_EOCD(FILE*,int);
//int read_CDHs(FILE*,file_str**,int,int);
//int read_LFHs(FILE*,file_str**,int);

#endif
