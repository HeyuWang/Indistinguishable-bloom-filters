#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "bloomfiltertree.h"
#include "bloomfilter.h"
#include "dataitem.h"
#include "helper.h"
#include "key128bitGenerator.h"



int countTheNumberOfNodesAboveTheseNodes(unsigned int Numberofnodes){
    unsigned int sum = 0;
    unsigned int n = Numberofnodes;
    while( n != 2){
        if(n<2)
            break;
        sum += ((n+1)/2);
        n = (n+1)/2;
    }
    sum += 1;
    return sum;
}

short is_leaf(DOUBLENODEBLOOMFILTERTREE *bft,unsigned int i){
    unsigned int tmp = countTheNumberOfNodesAboveTheseNodes(bft->leavesSize);
    if(i > tmp)
        return 1;
    else
        return 0;
}


DOUBLENODEBLOOMFILTERTREE *init_variable_bf_tree(unsigned int node, unsigned long root_bf_size_in_bytes){
    DOUBLENODEBLOOMFILTERTREE *bft;
    if((bft = malloc(sizeof(DOUBLENODEBLOOMFILTERTREE))) == NULL){
        fprintf(stderr,"error in initializing a variable tree handle");
        exit(1);
    }
    unsigned int tmp = countTheNumberOfNodesAboveTheseNodes(node);
    bft->size = tmp + node;
    bft->leavesSize = node;

    if((bft->data = malloc ((bft->size + 1) * sizeof(DOUBLENODEBLOOMFILTER *))) == NULL)
        fprintf(stderr,"Data allocation failed");
    unsigned int Currentnumberofnodes = bft->leavesSize;
    unsigned int Abovenumberofnodes = countTheNumberOfNodesAboveTheseNodes(Currentnumberofnodes);
    unsigned int totalnodes = Currentnumberofnodes + Abovenumberofnodes;
    unsigned int level = (unsigned int)log2(totalnodes);
        
        
    for(int i = level; i > 0; i--){
        #ifdef LOGGING
            printf("---------------------level: %d----------------------------\n",i);
            printf("-----------level %d de bf size is %ld\n",i,root_bf_size_in_bytes/Currentnumberofnodes);
        #endif 
        for(unsigned int j = totalnodes; j > Abovenumberofnodes; j--){
            bft->data[j] = init_empty_BF(root_bf_size_in_bytes/Currentnumberofnodes);
            #ifdef LOGGING
                printf("bft->data[%d] is initialized\n",j);
            #endif
        }
        Currentnumberofnodes = (Currentnumberofnodes+1)/2;

        Abovenumberofnodes = countTheNumberOfNodesAboveTheseNodes(Currentnumberofnodes);

        totalnodes = Currentnumberofnodes + Abovenumberofnodes;   

    }
    bft->data[1] = init_empty_BF(root_bf_size_in_bytes);
    printf("---------------------level: %d----------------------------\n",0);
    printf("----------level %d de bf size is %ld\n",0,root_bf_size_in_bytes/Currentnumberofnodes);
    printf("bft->data[%d] is initialized\n",1);
    bft->built = true;
    bft->type = VARIABLE;
    return bft;
}



void findParentNodeForInitializedVariableTree(DOUBLENODEBLOOMFILTERTREE *bft){
    if(bft->type == VARIABLE){
        #ifdef LOGGING
                printf("---------------------starting rendie----------------------------\n");
        #endif 
        unsigned int Currentnumberofnodes = bft->leavesSize;
        unsigned int Abovenumberofnodes = countTheNumberOfNodesAboveTheseNodes(Currentnumberofnodes);
        unsigned int totalnodes = Currentnumberofnodes + Abovenumberofnodes;
        unsigned int level = (unsigned int)log2(totalnodes);
        
        
        for(int i = level; i > 0; i--){
            #ifdef LOGGING
                printf("---------------------level: %d----------------------------\n",i);
            #endif 

            int k = 0;
            char state = 0;
            for(unsigned int j = totalnodes; j > Abovenumberofnodes; j -= 2){

                if(j != totalnodes){

                    if(state == 0){
                        bft->data[Abovenumberofnodes-k]->leftparentnode = j-1;
                        bft->data[Abovenumberofnodes-k]->rightparentnode = j;
                        #ifdef LOGGING
                            printf("bft->data[%d] is generated by leftparentnode:bft->data[%d] and rightparentnode bft->data[%d] \n",Abovenumberofnodes-k,j-1,j);
                        #endif 
                        k++;
                    }
                    else{
                        bft->data[Abovenumberofnodes-k]->leftparentnode = j;
                        bft->data[Abovenumberofnodes-k]->rightparentnode = j + 1;
                        #ifdef LOGGING
                            printf("bft->data[%d] is generated by leftparentnode:bft->data[%d] and rightparentnode bft->data[%d] \n",Abovenumberofnodes-k,j,j+1);
                        #endif 

                        k++;
                    }
                }
                else{
                    if(((j-Abovenumberofnodes)%2) == 1){

                        bft->data[Abovenumberofnodes]->leftparentnode = j;
                        #ifdef LOGGING
                            printf("bft->data[%d] is copyed from mother node : bft->data[%d] \n",Abovenumberofnodes-k,j);
                        #endif 
                        state = 1;
                        k++;
                    }
                    else{
                        bft->data[Abovenumberofnodes]->leftparentnode = j-1;
                        bft->data[Abovenumberofnodes]->rightparentnode = j;
                        #ifdef LOGGING
                            printf("bft->data[%d] is generated by leftparentnode:bft->data[%d] and rightparentnode bft->data[%d] \n",Abovenumberofnodes-k,j-1,j);
                        #endif 

                        k++;
                        state = 0;
                    }
                }             
            }
            
            Currentnumberofnodes = (Currentnumberofnodes+1)/2;

            Abovenumberofnodes = countTheNumberOfNodesAboveTheseNodes(Currentnumberofnodes);

            totalnodes = Currentnumberofnodes + Abovenumberofnodes;

        }   


    }
}

void destroy_bftree(DOUBLENODEBLOOMFILTERTREE *bft){
    unsigned int i = bft->type || bft->built ? 1 : countTheNumberOfNodesAboveTheseNodes(bft->leavesSize);
    while(i < (bft->size + 1))
        destroy_bf(bft->data[i++]);
    free(bft->data);
    free(bft);

}



void addDataitemToAVariableTreeNode(DOUBLENODEBLOOMFILTERTREE *bft,dataitem *di[],int current_node,int i){
    unsigned int AboveNumberOfLeaves = countTheNumberOfNodesAboveTheseNodes(bft->leavesSize); 
    if(is_leaf(bft,i)){
        add_a_dataitem_to_bloomfilter(di[i-AboveNumberOfLeaves],bft->data[current_node]);
        #ifdef LOGGING
            printf("@@@@@@@@@@@@@@@@!!!!!!!!!!!!!!!!!################dataitem[%s] is added to bft->data[%d]----------------------------\n",di[i-AboveNumberOfLeaves]->ID,current_node);
        #endif 
    }
    else{
        if(bft->data[i]->leftparentnode != 0)
            addDataitemToAVariableTreeNode(bft,di,current_node,bft->data[i]->leftparentnode);
        if(bft->data[i]->rightparentnode != 0)
            addDataitemToAVariableTreeNode(bft,di,current_node,bft->data[i]->rightparentnode);
    }
}




void findv2(DOUBLENODEBLOOMFILTERTREE *bft, BYTE trapdoor[][SHA256_BLOCK_SIZE], int i){
    #ifdef LOGGING
        // printf("Visit %d\n",i);
    #endif

        if(is_trapdoor_in_bloomV2(bft->data[i],trapdoor)){
            // printf("trapdoor in contained in nodes : bft->data[%d]\n",i);
            if(is_leaf(bft,i)){
                printf("@@@@@@@@@@@@@@@@trapdoor : trapdoor is found in@@@@@@@@@@@ #####Leaf nodes : %d \n",i-countTheNumberOfNodesAboveTheseNodes(bft->leavesSize));
            }
            else{
                if(bft->data[i]->leftparentnode != 0)
                    findv2(bft,trapdoor,bft->data[i]->leftparentnode);
                if(bft->data[i]->rightparentnode != 0)
                    findv2(bft,trapdoor,bft->data[i]->rightparentnode);
            }
        }
}

void findv3(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[5][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i){
    #ifdef LOGGING
        // printf("Visit %d\n",i);
    #endif

        if(is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[0]) && is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[1])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[2])\
        &&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[3])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[4]) ){
            // printf("multitrapdoor is contained in nodes : bft->data[%d]\n",i);
            if(is_leaf(bft,i)){
                printf("@@@@@@@@@@@@@@@@trapdoor : multitrapdoor is found in@@@@@@@@@@@ #####Leaf nodes : %d \n",i-countTheNumberOfNodesAboveTheseNodes(bft->leavesSize));
            }
            else{
                if(bft->data[i]->leftparentnode != 0)
                    findv3(bft,multitrapdoor,bft->data[i]->leftparentnode);
                if(bft->data[i]->rightparentnode != 0)
                    findv3(bft,multitrapdoor,bft->data[i]->rightparentnode);
            }
        }
}

void findv3_disjunctive(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[5][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i){
    #ifdef LOGGING
        // printf("Visit %d\n",i);
    #endif
    for(int i = 0; i < 5;i++)
        findv2(bft,multitrapdoor[i],1);
        // if(is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[0]) && is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[1])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[2])\
        // &&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[3])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[4]) ){
        //     // printf("multitrapdoor is contained in nodes : bft->data[%d]\n",i);
        //     if(is_leaf(bft,i)){
        //         printf("@@@@@@@@@@@@@@@@trapdoor : multitrapdoor is found in@@@@@@@@@@@ #####Leaf nodes : %d \n",i-countTheNumberOfNodesAboveTheseNodes(bft->leavesSize));
        //     }
        //     else{
        //         if(bft->data[i]->leftparentnode != 0)
        //             findv3(bft,multitrapdoor,bft->data[i]->leftparentnode);
        //         if(bft->data[i]->rightparentnode != 0)
        //             findv3(bft,multitrapdoor,bft->data[i]->rightparentnode);
        //     }
        // }
}

void findv4(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[10][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i){
    #ifdef LOGGING
        // printf("Visit %d\n",i);
    #endif

        if(is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[0]) && is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[1])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[2])\
        &&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[3])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[4])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[5])\
        &&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[6])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[7])&&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[8])\
        &&is_trapdoor_in_bloomV2(bft->data[i],multitrapdoor[9])  ){
            // printf("multitrapdoor is contained in nodes : bft->data[%d]\n",i);
            if(is_leaf(bft,i)){
                printf("@@@@@@@@@@@@@@@@trapdoor : multitrapdoor is found in@@@@@@@@@@@ #####Leaf nodes : %d \n",i-countTheNumberOfNodesAboveTheseNodes(bft->leavesSize));
            }
            else{
                if(bft->data[i]->leftparentnode != 0)
                    findv4(bft,multitrapdoor,bft->data[i]->leftparentnode);
                if(bft->data[i]->rightparentnode != 0)
                    findv4(bft,multitrapdoor,bft->data[i]->rightparentnode);
            }
        }
}

void findv4_disjunctive(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[10][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i){
    for(int i = 0; i < 10; i++)
        findv2(bft,multitrapdoor[i],1);
}




void saveDoubleNodeBloomfilterTree(char *filename, DOUBLENODEBLOOMFILTERTREE *bft){
    FILE *f = fopen(filename, "w");
    if(f == NULL){
        fprintf(stderr,"error in initializing a file handle");
        exit(-1);
    }
    fprintf(f,"%c%lu:%u:",bft->type == VARIABLE ? 'v' : 'f',bft->data[1]->size,bft->leavesSize);
    for(int i = 1; i < bft->size + 1; i++){
        fprintf(f,"%u:",bft->data[i]->rb);
        fprintf(f,"%hu:",bft->data[i]->leftparentnode);
        fprintf(f,"%hu:",bft->data[i]->rightparentnode);
        for(int j = 0; j < bft->data[i]->size; j++){
            fprintf(f,"%hx",bft->data[i]->array[j]);
        }
    #ifdef LOGGING
        printf("bft->data[%d] is saved in file : %s\n",i,filename);
    #endif 
    }
    
    fclose(f);
}

DOUBLENODEBLOOMFILTERTREE *init_variable_bf_treev2(unsigned int node, unsigned long root_bf_size_in_bytes){
    DOUBLENODEBLOOMFILTERTREE *bft;
    if((bft = malloc(sizeof(DOUBLENODEBLOOMFILTERTREE))) == NULL){
        fprintf(stderr,"error in initializing a variable tree handle");
        exit(1);
    }
    unsigned int tmp = countTheNumberOfNodesAboveTheseNodes(node);
    bft->size = tmp + node;
    bft->leavesSize = node;

    if((bft->data = malloc ((bft->size + 1) * sizeof(DOUBLENODEBLOOMFILTER *))) == NULL)
        fprintf(stderr,"Data allocation failed");
    unsigned int Currentnumberofnodes = bft->leavesSize;
    unsigned int Abovenumberofnodes = countTheNumberOfNodesAboveTheseNodes(Currentnumberofnodes);
    unsigned int totalnodes = Currentnumberofnodes + Abovenumberofnodes;
    unsigned int level = (unsigned int)log2(totalnodes);
        
        
    for(int i = level; i > 0; i--){
        #ifdef LOGGING
            printf("---------------------level: %d----------------------------\n",i);
            printf("-----------level %d de bf size is %ld\n",i,root_bf_size_in_bytes/Currentnumberofnodes);
        #endif 
        for(unsigned int j = totalnodes; j > Abovenumberofnodes; j--){
            bft->data[j] = init_empty_BFv2(root_bf_size_in_bytes/Currentnumberofnodes);
            #ifdef LOGGING
                printf("bft->data[%d] is initialized\n",j);
            #endif
        }
        Currentnumberofnodes = (Currentnumberofnodes+1)/2;

        Abovenumberofnodes = countTheNumberOfNodesAboveTheseNodes(Currentnumberofnodes);

        totalnodes = Currentnumberofnodes + Abovenumberofnodes;   

    }
    bft->data[1] = init_empty_BF(root_bf_size_in_bytes);
    printf("---------------------level: %d----------------------------\n",0);
    printf("----------level %d de bf size is %ld\n",0,root_bf_size_in_bytes/Currentnumberofnodes);
    printf("bft->data[%d] is initialized\n",1);
    bft->built = true;
    bft->type = VARIABLE;
    return bft;
}

DOUBLENODEBLOOMFILTERTREE *load_bf_tree(char *filename){
    FILE *f = fopen(filename,"r");
    if (f == NULL) {
        fprintf(stderr, "File not found: %s\n", filename);
        exit(1);
    }

    char type = fgetc(f);
    int c;
    unsigned long bfdata1size;
    unsigned int bftleavessize;
    fscanf(f,"%lu:%u:",&bfdata1size,&bftleavessize);

    DOUBLENODEBLOOMFILTERTREE *bft;

    if (type == 'v')
        bft = init_variable_bf_treev2(bftleavessize, bfdata1size);
    else {
        ;
    }

    for(int i = 1; i < bft->size + 1; i++){
        fscanf(f,"%u:",&(bft->data[i]->rb));
        fscanf(f,"%u:",&(bft->data[i]->leftparentnode));
        fscanf(f,"%u:",&(bft->data[i]->rightparentnode));
        for(int j = 0; j < bft->data[i]->size; j++){
            fscanf(f,"%4hx",&(bft->data[i]->array[j]));
        }
    #ifdef LOGGING
        printf("bft->data [%d] is loaded----------------------------\n",i);
    #endif 
    }
    fclose(f);
return bft;
}




// int main(){
//     clock_t start,finish;
//     start = clock();
//     printf("start\n");
//     produceNPlusOnekeys(NPlusOnekey);
//     // #ifdef LOGGING
//     //     for(int i = 0;i < NUMBEROFKEYS + 1;i++){

//     //             printf("---------------------------keys[%d] : ",i);

//     //         for(int j = 0; j < strlen(NPlusOnekey[0]); j++){
//     //             printf("%c",NPlusOnekey[i][j]);
//     //         }
//     //         printf("\n");
//     //     }
//     // #endif 
//     // save_NPlusOneKeys("NPlusOnekey.txt");

//     char* path = "/home/kingofthecode/Documents/code/paper/variable_tree_code (newversion)/awards_1990/awd_1990_00";

//     DOUBLENODEBLOOMFILTERTREE *bft = init_variable_bf_tree(NUMBEROFFILES,2500);
//     findParentNodeForInitializedVariableTree(bft);

//     unsigned tmp = countTheNumberOfNodesAboveTheseNodes(bft->leavesSize);

//     // dataitem *di[bft->leavesSize + 1];
//     // for(int i = 1; i < bft->leavesSize + 1; i++){
//     //     di[i] = init_dataitem(i);
//     //     saveoneDataItems("dataitem.txt",di[i]);
//     // }

//     recursive_generate_di_and_save_di(path);

//     for(int i = 1; i < tmp + bft->leavesSize + 1; i++){
//         addDataitemToAVariableTreeNode(bft,diarray,i,i);
//     }

//     // for(int i = 1; i < bft->leavesSize + 1; i++){
//     //     destroy_dataitem(di[i]);
//     //     printf("dataitem[%d] is destroyed\n",i);
//     // }
//     saveDoubleNodeBloomfilterTree("bft_tree.txt",bft);
//     save_NPlusOneKeys("NPlusOnekey.txt");


//     destroy_all_dataitem(diarray);
//     destroy_bftree(bft);

//     finish = clock();
//     double duration;
//     duration = (double)(finish-start)/CLOCKS_PER_SEC;
//     printf("the total build tree time is : %f seconds.\n",duration);

//     return 0;
// }


// int main(){

//     clock_t start,start2,finish;
    
//     printf("start\n");


//     // load_NPlusOneKeys("NPlusOnekey.txt",NPlusOnekey);
//     // #ifdef LOGGING
//     //     for(int i = 0;i < NUMBEROFKEYS + 1;i++){

//     //             printf("---------------------------keys[%d] : ",i);

//     //         for(int j = 0; j < strlen(NPlusOnekey[0]); j++){
//     //             printf("%c",NPlusOnekey[i][j]);
//     //         }
//     //         printf("\n");
//     //     }
//     // #endif 

//     start = clock();
//     DOUBLENODEBLOOMFILTERTREE *bft = load_bf_tree("bft_tree.txt");

//     start2 = clock();
//     BYTE trapdoor[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
//     get_a_trapdoor_of_a_keyword("supports",trapdoor);


//     findv2(bft,trapdoor,1);

//     finish = clock();
//     double searchtime,loadtreetime;


//     destroy_bftree(bft);

//     loadtreetime = (double)(finish-start)/CLOCKS_PER_SEC;
//     searchtime = (double)(finish-start2)/CLOCKS_PER_SEC;
//     printf("the total load tree time is : %f seconds.\n",loadtreetime);
//     printf("the total search tree time is : %f seconds.\n",searchtime);

//     return 0;
// }



// int main(){

    // clock_t start,finish;
    // start = clock();


    // char* path = "/home/kingofthecode/Documents/code/paper/variable_tree_code (newversion)/awards_1990";
    // produceNPlusOnekeys(NPlusOnekey);

    // // print_allword(path);
    // // dataitem *di = find_abstract_from_file_and_save_in_dataitem(path);
    // // print_dataitem(di);
    // // destroy_dataitem(di);
    // printf("start\n");
    // recursive_generate_di_and_save_di(path);

    // DOUBLENODEBLOOMFILTERTREE *bft = init_variable_bf_tree(NUMBEROFFILES,2500);
    // findParentNodeForInitializedVariableTree(bft);
    // unsigned tmp = countTheNumberOfNodesAboveTheseNodes(bft->leavesSize);

    // for(int i = 1; i < tmp + bft->leavesSize + 1; i++){
    //     addDataitemToAVariableTreeNode(bft,diarray,i,i);
    // }

    // saveDoubleNodeBloomfilterTree("bft_tree.txt",bft);
    // save_NPlusOneKeys("NPlusOnekey.txt");

    // finish = clock();
    // double duration;
    // duration = (double)(finish-start)/CLOCKS_PER_SEC;
    // printf("the total build tree time is : %f seconds.\n",duration);


    // BYTE trapdoor[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
    // get_a_trapdoor_of_a_keyword("investigator",trapdoor);

    // if(is_trapdoor_in_bloomV2(bft->data[4],trapdoor))
    //     printf("111111111111111111111111\n");
    // destroy_bftree(bft);
    // destroy_all_dataitem(diarray);







//     clock_t start,start2,finish;
    
//     printf("start\n");


//     load_NPlusOneKeys("NPlusOnekey.txt",NPlusOnekey);


//     start = clock();
//     DOUBLENODEBLOOMFILTERTREE *bft = load_bf_tree("bft_tree.txt");

//     start2 = clock();
    
//     BYTE trapdoor[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
//     get_a_trapdoor_of_a_keyword("feasibility",trapdoor);


//     findv2(bft,trapdoor,1);

//     BYTE trapdoor1[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
//     get_a_trapdoor_of_a_keyword("feasibility",trapdoor1);

//     if(is_trapdoor_in_bloomV2(bft->data[1],trapdoor1))
//         printf("111111111111111111111111\n");

//     finish = clock();
//     double searchtime,loadtreetime;


//     destroy_bftree(bft);

//     loadtreetime = (double)(finish-start)/CLOCKS_PER_SEC;
//     searchtime = (double)(finish-start2)/CLOCKS_PER_SEC;
//     printf("the total load tree time is : %f seconds.\n",loadtreetime);
//     printf("the total search tree time is : %f seconds.\n",searchtime);

//     return 0;   
// }