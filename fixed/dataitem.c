#include "dataitem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#
#include <dirent.h>

#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <sys/stat.h>
#include "bloomfilter.h"
#include "config.h"
#include "helper.h"


 


dataitem *init_dataitem(unsigned int k){
    dataitem *di = (dataitem *)calloc(1,sizeof(dataitem));
    if(di == NULL){
        fprintf(stderr,"[*] Error in initializing dataitem.\n");
        exit(-1);
    }



    di->numberofword = (unsigned int)getarandomint() % 10 + 1;   //1 - 11 ge keyword
    if((di->word = (char **)calloc(di->numberofword,sizeof(char *))) == NULL)
        fprintf(stderr,"[*] Error in word allocation\n");

    int i, j, run;
    char temp[10];
    
    for(i=0; i<di->numberofword; i++){ 
        run=rand()%7 + 3;
        for(j=0; j< run ; j++){ 
            temp[j]= rand()%26+'a'; 
        }
        temp[run] = '\0';
        di->word[i] =(char *)calloc(1,run+1);
        strcpy(di->word[i],temp);

    // for(int i = 0; i < di->numberofword; i++){
    //     int run = rand() % 7 + 4;
    //     for(int j = 0; j < run; j++){
    //         printf("start");
    //         di->word[i][j] = rand()%26 + 'a';
    //         printf("end");
    //     }
    //     di->word[i][run] = '\0';
    }
    return di;
}

void destroy_dataitem(dataitem *di){
    for(int i = 0; i < di->numberofword; i++)
        free(di->word[i]);
    free(di->word);
    free(di);
}

void add_a_dataitem_to_bloomfilter(dataitem *di,DOUBLENODEBLOOMFILTER *bf){


    for(int i = 0; i < di->numberofword; i++){
        BYTE Nhashvalue[NUMBEROFKEYS][SHA256_BLOCK_SIZE];

        getNhashvalue(Nhashvalue,di->word[i]);


        add_N_hashval_to_bloomfilter(bf,Nhashvalue);
        #ifdef LOGGING
            printf("dataitem(%s)->word[%d] : %s is added to bft \n",di->ID,i,di->word[i]);
        #endif 
    }
}

void saveoneDataItems(char *filename, dataitem *di){
    FILE *f = fopen(filename,"a");
    if(f == NULL){
        fprintf(stderr,"error in open a dataitem file handle");
        exit(-1);
    }
    fprintf(f,"dataitem[%s]:",di->ID);
    for(int i = 0; i < di->numberofword; i++){
        fprintf(f,"%s\t",di->word[i]);
    }
    fputc('\n',f);
    #ifdef LOGGING
        printf("dataitem[%s] is saved in file : %s \n",di->ID,filename);
    #endif 
    fclose(f);

}

// main(){
//     produceNPlusOnekeys(NPlusOnekey);
//     dataitem *di = init_dataitem(1);
//     printf("%d\n",di->numberofword);
//     for(int i = 0; i < di->numberofword; i++)
//         printf("%s\t%d\n",di->word[i],strlen(di->word[i]));
//     DOUBLENODEBLOOMFILTER *bf = init_empty_BF(16);
//     add_a_dataitem_to_bloomfilter(di,bf);
//     if(iskeyword_in_bloom(bf,"a"))
//         printf("--------------------is  in ---------------------\n");
//     else
//         printf("not in");
//     destroy_bf(bf);
//     destroy_dataitem(di);
// }




// char* getword(FILE* handle,char* word, int lim){
//     int c;
//     char* w = word;
//     while( isspace(c = fgetc(handle)))
//         ;
    
//     while (!isspace(c = fgetc(handle)) ){
//         if(c != EOF && isalpha(c)){
//             *w++ = c;
//             printf("%c\t",c);
//         }

//         if(c != EOF && !isalpha(c)){
//             *w++ = '\0';
//         }
//     }
//     return word[0];
// }


char word[MAXWORD];

int getword(FILE* handle,char *word, int lim)
{
    int c;
    char *w = word;
    while (isspace(c = fgetc(handle)))
    ;

    if (c == EOF)
        return 0;
    if (c != EOF )
        *w++ = c;
    if (!isalpha(c)) {
        *w = '\0';
        return c;
    }
    for ( ; --lim > 0; w++)
        if (!isalnum(*w = fgetc(handle))) {

            break;
    }
    *w = '\0';
    return (w - word[0]);
}



int get_nw(char* filename){
    FILE* handle = open(filename);
    int c;
    int nw = 0;
    while((c = fgetc(handle)) != EOF ){

        if (c != 'A')
            continue;
        else if(c == 'A'){
            char temp[8];
            fgets(temp,8,handle);
            // printf("%s\n",temp);
            if (strcmp(temp,"bstract") == 0){
                // printf("11111----------\n");
                break;
            }
        }
    }
    
        while(getword(handle,word,MAXWORD) != 0){
            if (isalpha(word[0]))
                nw++;
                
    }
    fclose(handle);
    return nw;
}

dataitem *find_abstract_from_file_and_save_in_dataitem(char* filename){
    int nw = get_nw(filename);
    dataitem *di = (dataitem *)calloc(1,sizeof(dataitem));
    if(di == NULL){
        fprintf(stderr,"[*] Error in initializing dataitem.\n");
        exit(-1);
    }

    strcpy(di->ID,filename);

    di->numberofword = nw;   //1 - 11 ge keyword
    if((di->word = (char **)calloc(di->numberofword,sizeof(char *))) == NULL)
        fprintf(stderr,"[*] Error in word allocation\n");

    
    saveword_in_dataitem(filename,di);
 

    // for(int i = 0; i < di->numberofword; i++){
    //     int run = rand() % 7 + 4;
    //     for(int j = 0; j < run; j++){
    //         printf("start");
    //         di->word[i][j] = rand()%26 + 'a';
    //         printf("end");
    //     }
    //     di->word[i][run] = '\0';
    
    return di;
}

void print_dataitem(dataitem* di){
    for (int i= 0; i< di->numberofword; i++)
        printf("%d-th is %s\n",i,di->word[i]);
}


void print_allword(char* filename){
     FILE* handle = open(filename);
    int c;
    int nw = 0;
    while((c = fgetc(handle)) != EOF ){

        if (c != 'A')
            continue;
        else if(c == 'A'){
            char temp[8];
            fgets(temp,8,handle);
            // printf("%s\n",temp);
            if (strcmp(temp,"bstract") == 0){
                // printf("11111----------\n");
                break;
            }
        }
    }
    
        while(getword(handle,word,MAXWORD) != 0){
            if (isalpha(word[0])){
                int len = strlen(word);


            
                nw++;
                printf("%d-th is %s length is %d\n",nw,word,len);
            }
                
    }
    fclose(handle);

}

void saveword_in_dataitem(char* filename,dataitem* di){
    FILE* handle = open(filename);
    int c;
    int nw = 0;
    int i = 0;
    while((c = fgetc(handle)) != EOF ){

        if (c != 'A')
            continue;
        else if(c == 'A'){
            char temp[8];
            fgets(temp,8,handle);
            // printf("%s\n",temp);
            if (strcmp(temp,"bstract") == 0){
                // printf("11111----------\n");
                break;
            }
        }
    }
        
        while(getword(handle,word,MAXWORD) != 0){
            if (isalpha(word[0])){
                int len = strlen(word);

                di->word[i] =(char *)calloc(1,len+1);
                
                strcpy(di->word[i],word);
                i++;
            }
                
    }
    fclose(handle);

}


FILE* open(char* filename){
    FILE *handle = NULL;

    if ((handle = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "[*] Error in opening file: %s\n", filename);
        exit(-1);
    }

    return handle;
}

dataitem* diarray[NUMBEROFFILES+1];
int noffiles = 1;
char dataitempath[200]= "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/dataitem/";



void changepathname(char dp[],int nof){
    char inputsizeoffiles[20];
    itoa(nof,inputsizeoffiles);
    strcat(dp,inputsizeoffiles);

}


// char* dataitempath = "/home/kingofthecode/Documents/code/paper/variable_tree_code (newversion)/VARIABLE_TREE_FIANL/dataitem.txt";
void recursive_generate_di_and_save_di(char* filename){
    
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
                    if(noffiles < NUMBEROFFILES+1){
                        diarray[noffiles] = find_abstract_from_file_and_save_in_dataitem(ent->d_name);
                        printf("diarray[%d] is constructed from file %s\n",noffiles,ent->d_name);
                        
                        saveoneDataItems(dataitempath,diarray[noffiles]);
                        noffiles++;
                    }
                    else{
                        break;
                    }
                } else if (is_dir(ent->d_name)) {
                    recursive_generate_di_and_save_di(ent->d_name);
                }
            }
        }
        chdir(cur_dir);
        closedir(dir);
    } else if (is_file(filename)) {
        
        if(noffiles < NUMBEROFFILES+1){
            diarray[noffiles] = find_abstract_from_file_and_save_in_dataitem(filename);
            saveoneDataItems(dataitempath,diarray[noffiles]);
            noffiles++;
        }
        else{
            ;
        }
    }
}

void destroy_all_dataitem(dataitem* diar[]){
    for (int i = 1; i < NUMBEROFFILES+1;i++)
        destroy_dataitem(diar[i]);
}
