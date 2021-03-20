#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "bloomfilter.h"
#include "bloomfiltertree.h"
#include "config.h"
#include "csprng.h"
#include "dataitem.h"
#include "helper.h"
#include "hmac_sha256.h"
#include "key128bitGenerator.h"
#include "sha1.h"
#include  "sha256.h"



int main(int argc, char* argv[]){

    char* datasetpath = "/home/wangheyu/Documents/paper/code/dataset/NSF Research Award Abstract set/allpart";
    if (argc == 2){


        // use ./a preprocess to preprocess the data in the specified directory
        if (strcmp(argv[1],"preprocess") == 0){
            clock_t start,finish;
            start = clock();
            int nw = recursive_delete_links_html(datasetpath);

            finish = clock();
            double duration;
            duration = (double)(finish-start)/CLOCKS_PER_SEC;
            printf("the total build tree time is : %f seconds.\n",duration);
            printf("successing scan %d files\n",nw);
        }


        //use ./a build to build the tree and save the tree the size of input files need to be specified by #define NUMBEROFFILES
        if (strcmp(argv[1],"buildandsearch") == 0){
            clock_t start,finish;



            produceNPlusOnekeys(NPlusOnekey);

            char keypath[200] = "../dataoutcome/keys/NPlusOnekey";

            changepathname(keypath,NUMBEROFFILES);

            save_NPlusOneKeys(keypath);


            // print_allword(path);
            // dataitem *di = find_abstract_from_file_and_save_in_dataitem(path);
            // print_dataitem(di);
            // destroy_dataitem(di);
            printf("start\n");

            changepathname(dataitempath,NUMBEROFFILES);

            recursive_generate_di_and_save_di(datasetpath);


            start = clock();

            double ln2 = 0.6931471805599453;
            double rootnodesize = (NUMBEROFKEYS/ln2)*(NUMBEROFFILES*99.27)/8;

            DOUBLENODEBLOOMFILTERTREE *bft = init_variable_bf_tree(NUMBEROFFILES,(unsigned long)rootnodesize);
            findParentNodeForInitializedVariableTree(bft);
            unsigned tmp = countTheNumberOfNodesAboveTheseNodes(bft->leavesSize);

            for(int i = 1; i < tmp + bft->leavesSize + 1; i++){
                addDataitemToAVariableTreeNode(bft,diarray,i,i);
            }

            char treepath[200] = "../dataoutcome/bftree/";

            changepathname(treepath,NUMBEROFFILES);

            saveDoubleNodeBloomfilterTree(treepath,bft);


            finish = clock();
            double duration;
            duration = (double)(finish-start)/CLOCKS_PER_SEC;




            char* filename = "../dataoutcome/buildtreetime/buildtime";
            savebuildtreetime(filename,duration,(unsigned long)rootnodesize);

            clock_t start2,finish2,multitrapdoorfinsh;

                // char *word[5] = {\
                // "Soufriere",\
                // "Department",\
                // "phylogeny",\
                // "shrinking",\
                // "elucidated"
                // };
            char *multi[5][5] = {"Farrell","Morse","herbivorous","hostplants","foodplants",//从dataitem[a0073330.txt]中提取出来的5个关键词
            // char *multi2[5] = {
                "Differences","population","pathways","elucidated","particularly",//从dataitem[dataitem[a0073270.txt]]中提取出来的5个关键词
            // char *multi3[5] = {
                "Phosphorus","biological","limiting","ecosystems","widespread",//从dataitem[a0073851.txt]中提取出来的5个关键词
            // char *multi4[5] = {
                "Jagannathan","Ramanujam","performance","depends","how",//从dataitem[a0073800.txt]中提取出来的5个关键词
            // char *mutli5[5] = {
                "predation","fish","survivorship","antipredator","growth"};//从dataitem[a0073071.txt]中提取出来的5个关键词


            
            char searchfilename[200] = "../dataoutcome/searchtreetime/buildsearchtime.txt";
            FILE *f = fopen(searchfilename,"a");
            if(f == NULL){
                fprintf(stderr,"error in open a dataitem file handle");
                exit(-1);
            }

            double sum = 0;
            double trapdoor_sum = 0;
        
            for (int i = 0; i < 5; i++){
                start2 = clock();
                
                BYTE multitrapdoor[5][NUMBEROFKEYS][SHA256_BLOCK_SIZE];

                for(int j = 0; j<5;j++){
                    get_a_trapdoor_of_a_keyword(multi[i][j],multitrapdoor[j]);
                }
                multitrapdoorfinsh = clock();
                
                // BYTE trapdoor[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
                // get_a_trapdoor_of_a_keyword(word[i],trapdoor);
                // trapdoorfinsh = clock();

                // findv2(bft,trapdoor,1);

                findv3(bft,multitrapdoor,1);

                finish2 = clock();
                double searchtime;
                double trapdoor_cal;
                
                searchtime = (double)(finish2-start2)/CLOCKS_PER_SEC;
                trapdoor_cal = (double)(multitrapdoorfinsh-start2)/CLOCKS_PER_SEC;

                trapdoor_sum+=trapdoor_cal;
                sum += searchtime;

                fprintf(f,"searching time of multiword[%d]--%s\t%s\t%s\t%s\t%s\t is %f seconds\n",i,multi[i][0],multi[i][1],multi[i][2],multi[i][3],multi[i][4],searchtime);
            }

            fprintf(f,"average multitrapdoor construction time is %f seconds\n ",trapdoor_sum/5);

            fprintf(f,"average searching time  is %f seconds \n ",sum/5.0);

            fprintf(f,"@@@@@@@@@@@############$$$$$$$$$$$woshihualidefenjiexian$$$$$$$$$$$############@@@@@@@@@@@\n\n");

            fclose(f);



            // get_a_trapdoor_of_a_keyword("investigator",trapdoor);

            // if(is_trapdoor_in_bloomV2(bft->data[4],trapdoor))
            //     printf("111111111111111111111111\n");
            destroy_bftree(bft);
            destroy_all_dataitem(diarray);


        }

        if (strcmp(argv[1],"loadandsearch") == 0){

            
            clock_t start,finish;
            
            printf("start\n");


            char keypath[200] = "../dataoutcome/keys/NPlusOnekey";
            changepathname(keypath,NUMBEROFFILES);

            
            load_NPlusOneKeys(keypath,NPlusOnekey);


            start = clock();


            char treepath[200] = "../dataoutcome/bftree/";

            changepathname(treepath,NUMBEROFFILES);


            DOUBLENODEBLOOMFILTERTREE *bft = load_bf_tree(treepath);

            finish = clock();

            char searchfilename[200] = "../dataoutcome/searchtreetime/loadsearchtime.txt";
                FILE *f = fopen(searchfilename,"a");
                if(f == NULL){
                    fprintf(stderr,"error in open a dataitem file handle");
                    exit(-1);
                }

                        
            double loadtreetime;

            loadtreetime = (double)(finish-start)/CLOCKS_PER_SEC;
            fprintf(f,"the total loadtreetime tree time corresponding to %d files is : %f seconds.\n",NUMBEROFFILES,loadtreetime);


            clock_t start2,finish2,multitrapdoorfinsh;

            char *multi[5][5] = {"Farrell","Morse","herbivorous","hostplants","foodplants",//从dataitem[a0073330.txt]中提取出来的5个关键词
            // char *multi2[5] = {
                "Differences","population","pathways","elucidated","particularly",//从dataitem[dataitem[a0073270.txt]]中提取出来的5个关键词
            // char *multi3[5] = {
                "Phosphorus","biological","limiting","ecosystems","widespread",//从dataitem[a0073851.txt]中提取出来的5个关键词
            // char *multi4[5] = {
                "Jagannathan","Ramanujam","performance","depends","how",//从dataitem[a0073800.txt]中提取出来的5个关键词
            // char *mutli5[5] = {
                "predation","fish","survivorship","antipredator","growth"};//从dataitem[a0073071.txt]中提取出来的5个关键词


            
            

            double sum = 0;
            double trapdoor_sum = 0;
        
            for (int i = 0; i < 5; i++){
                start2 = clock();
                
                BYTE multitrapdoor[5][NUMBEROFKEYS][SHA256_BLOCK_SIZE];

                for(int j = 0; j<5;j++){
                    get_a_trapdoor_of_a_keyword(multi[i][j],multitrapdoor[j]);
                }
                multitrapdoorfinsh = clock();
                
                // BYTE trapdoor[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
                // get_a_trapdoor_of_a_keyword(word[i],trapdoor);
                // trapdoorfinsh = clock();

                // findv2(bft,trapdoor,1);

                findv3(bft,multitrapdoor,1);

                finish2 = clock();
                double searchtime;
                double trapdoor_cal;
                
                searchtime = (double)(finish2-start2)/CLOCKS_PER_SEC;
                trapdoor_cal = (double)(multitrapdoorfinsh-start2)/CLOCKS_PER_SEC;

                trapdoor_sum+=trapdoor_cal;
                sum += searchtime;

                // fprintf(f,"searching time of multiword[%d]--%s\t%s\t%s\t%s\t%s\t is %f seconds\n",i,multi[i][0],multi[i][1],multi[i][2],multi[i][3],multi[i][4],searchtime);
            }

            // fprintf(f,"average multitrapdoor construction time is %f seconds\n ",trapdoor_sum/5);

            fprintf(f,"average searching time corresponding to %d files of 5 multitrapdoor is %f seconds \n ",NUMBEROFFILES,sum/5.0);

            // fprintf(f,"@@@@@@@@@@@############$$$$$$$$$$$woshihualidefenjiexian$$$$$$$$$$$############@@@@@@@@@@@\n\n");

            fclose(f);
        }
 


    


    }

    return 0;
}