#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <sys/stat.h>

#include "helper.h"
#include "config.h"


void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
} 

void itoa(int n, char s[])
{
    int i;
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */

    s[i] = '\0';
    reverse(s);
} 

unsigned long mod32(BYTE onehashval[],unsigned long m){

    BYTE onehashval2[SHA256_BLOCK_SIZE];

    for(int i = 0; i < SHA256_BLOCK_SIZE; i++)
        onehashval2[i] = onehashval[i];
    
    unsigned long res = 0;

    for(int i = 0; i < SHA256_BLOCK_SIZE; i++){
        unsigned char a[4];
        itoa(onehashval2[i],a);
        for(int j = 0; j < strlen(a); j++){ 
            res = (res * 10 + (int)(a[j] - '0')) % m;
        }
    }
    return res;
}

unsigned long mod20(BYTE onehashval[],unsigned long m){

    BYTE onehashval2[SHA1_BLOCK_SIZE];

    for(int i = 0; i < SHA1_BLOCK_SIZE; i++)
        onehashval2[i] = onehashval[i];

    unsigned long res = 0;


    for(int i = 0; i < SHA1_BLOCK_SIZE; i++){
        unsigned char a[4];
        // snprintf(a,sizeof(a),"%d",onehashval2[i]);
        itoa(onehashval2[i],a);
        for(int j = 0; j < strlen(a); j++){ 
            res = (res * 10 + (int)(a[j] - '0')) % m;
        }
    }
    return res;
}

void save_NPlusOneKeys(char *filename){
    FILE *f = fopen(filename, "w");
    if(f == NULL){
        fprintf(stderr,"error in initiallizing file handles : %s\n",filename);
        exit(-1);
    }
    fprintf(f,"NPLUSONEKEYS,each key is stored in a newlins\n");
    for(int i = 0; i < NUMBEROFKEYS + 1; i++){
        for(int j = 0; j < keybytenumber + 1; j++){
            fputc(NPlusOnekey[i][j],f);
        }
    fputc('\n',f);
    }
    fclose(f);
}

void load_NPlusOneKeys(char *filename,BYTE NPlusOnekeyss[][keybytenumber + 1]){
    FILE *f = fopen(filename, "r");
    if(f == NULL){
        fprintf(stderr,"error in initiallizing file handles : %s\n:",filename);
        exit(-1);
    }
    int c;
    while((c = fgetc(f)) != '\n')
        ;



    for(int i = 0; i < NUMBEROFKEYS + 1; i++){
        for(int j = 0; j < keybytenumber + 1; j++){
            c = fgetc(f);
            if(c == '\n')
                c = fgetc(f);
            if(c != EOF)
                NPlusOnekeyss[i][j] = (char)c;

        }

    }
    fclose(f);
}



bool is_file(const char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool is_dir(const char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
int count_files = 0;
int print_all_file(char* filename){
    DIR *dir;
    struct dirent *ent;
    const int max_path_length = 1024;

    char *cur_dir = (char *) malloc(max_path_length);
    getcwd(cur_dir, max_path_length);

    //in case of a dir
    if (is_dir(filename)) {

        dir = opendir(filename);
        chdir(filename);

        //run through all files of the dir
        while ((ent = readdir(dir)) != NULL) {
            //if we found a file, generate hash value and add it

            if(*ent->d_name !='.' ){
                if (is_file(ent->d_name)) {
                    printf("%s\n",ent->d_name);
                    count_files++;
                } else if (is_dir(ent->d_name)) {
                    print_all_file(ent->d_name);
                }
            }
        }
        chdir(cur_dir);
        closedir(dir);
    } else if (is_file(filename)) {
        printf("%s\n",filename);
        count_files++;
    }

    return count_files;
}


int recursive_delete_links_html(char* filename){
    DIR *dir;
    struct dirent *ent;
    const int max_path_length = 1024;

    char *cur_dir = (char *) malloc(max_path_length);
    getcwd(cur_dir, max_path_length);

    //in case of a dir
    if (is_dir(filename)) {

        dir = opendir(filename);
        chdir(filename);

        //run through all files of the dir
        while ((ent = readdir(dir)) != NULL) {
            //if we found a file, generate hash value and add it

            if(*ent->d_name !='.' ){
                if (is_file(ent->d_name)) {
                    

                    if (strcmp(ent->d_name,"links.html") == 0)
                        remove(ent->d_name);
                    
                    
                    count_files++;
                } else if (is_dir(ent->d_name)) {
                    recursive_delete_links_html(ent->d_name);
                }
            }
        }
        chdir(cur_dir);
        closedir(dir);
    } else if (is_file(filename)) {
        
        if (strcmp(ent->d_name,"links.html") == 0)
            remove(ent->d_name);
        count_files++;
    }

    return count_files;
}

int stoi(char* s){
    int sum = 0;
    for (int i = 0; i < strlen(s);i++){
        sum = sum + *s*10;
        s++;
    }
    return sum;
}


void savebuildtreetime(char *filename,double time,unsigned long ros){
    FILE *f = fopen(filename,"a");
    if(f == NULL){
        fprintf(stderr,"error in open a dataitem file handle");
        exit(-1);
    }
    fprintf(f,"the total build tree time for a tree of rootnode size of %ld corresponding to %d files is : %f seconds.\n",ros,NUMBEROFFILES,time);

    fputc('\n',f);

    fclose(f);

}


