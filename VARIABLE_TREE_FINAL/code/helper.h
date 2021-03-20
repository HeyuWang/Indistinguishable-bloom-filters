/*********************************************************************
* Filename:   sha1.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/

#ifndef HELPER_H
#define HELPER_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include "config.h"
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <sys/stat.h>
/****************************** MACROS ******************************/
             // SHA1 outputs a 20 byte digest

/**************************** DATA TYPES ****************************/







void reverse(char s[]);
void itoa(int n, char s[]);
unsigned long mod32(BYTE onehashval[],unsigned long m);
unsigned long mod20(BYTE onehashval[],unsigned long m);
void save_NPlusOneKeys(char *filename);
void load_NPlusOneKeys(char *filename,BYTE NPlusOnekeyss[][keybytenumber + 1]);

bool is_file(const char* path);
bool is_dir(const char* path);

int print_all_file(char* filename);

int recursive_delete_links_html(char* filename);

int stoi(char* s);
void savebuildtreetime(char *filename,double time,unsigned long ros);





#endif