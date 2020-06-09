#ifndef DATAITEM_H
#define DATAITEM_H


#include "key128bitGenerator.h"
#include "bloomfilter.h"
#include <stdio.h>

typedef struct{
    char ID[30];
    unsigned int numberofword;
    char **word; 

}dataitem;

extern dataitem* diarray[];
extern int noffiles ;
extern char dataitempath[];


dataitem *init_dataitem(unsigned int k);
void destroy_dataitem(dataitem *di);
void add_a_dataitem_to_bloomfilter(dataitem *di,DOUBLENODEBLOOMFILTER *bf);
void saveoneDataItems(char *filename, dataitem *di);
FILE* open(char* filename);
int get_nw(char* filename);

void saveword_in_dataitem(char* filename,dataitem* di);
void print_dataitem(dataitem* di);
dataitem *find_abstract_from_file_and_save_in_dataitem(char* filename);
void changepathname(char dp[],int nof);
void recursive_generate_di_and_save_di(char*);
void destroy_all_dataitem(dataitem* diar[]);

#endif