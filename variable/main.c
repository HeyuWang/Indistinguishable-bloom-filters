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

    if (argc == 2){


        // use ./a preprocess to preprocess the data in the specified directory
        if (strcmp(argv[1],"preprocess") == 0){
            clock_t start,finish;
            start = clock();
            char* datasetpath = "/home/wangheyu/Documents/paper/code/dataset/NSF Research Award Abstract set/allpart";
            int nw = recursive_delete_links_html(datasetpath);

            finish = clock();
            double duration;
            duration = (double)(finish-start)/CLOCKS_PER_SEC;
            printf("the total build tree time is : %f seconds.\n",duration);
            printf("successing scan %d files\n",nw);
        }


        //use ./a build to build the tree and save the tree the size of input files need to be specified by #define NUMBEROFFILES
        if (strcmp(argv[1],"buildtree") == 0){
            clock_t start,finish;



            char* datasetpath = "/home/wangheyu/Documents/paper/code/dataset/NSF Research Award Abstract set/allpart";
            produceNPlusOnekeys(NPlusOnekey);

            char keypath[200] = "/home/wangheyu/Documents/paper/code/final2/VARIABLE_TREE_FINAL/dataoutcome/keys/NPlusOnekey";

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

            char treepath[200] = "/home/wangheyu/Documents/paper/code/final2/VARIABLE_TREE_FINAL/dataoutcome/bftree/";

            changepathname(treepath,NUMBEROFFILES);

            saveDoubleNodeBloomfilterTree(treepath,bft);


            finish = clock();
            double duration;
            duration = (double)(finish-start)/CLOCKS_PER_SEC;




            char* filename = "/home/wangheyu/Documents/paper/code/final2/VARIABLE_TREE_FINAL/dataoutcome/buildtreetime/buildtime";
            savebuildtreetime(filename,duration,(unsigned long)rootnodesize);



            // get_a_trapdoor_of_a_keyword("investigator",trapdoor);

            // if(is_trapdoor_in_bloomV2(bft->data[4],trapdoor))
            //     printf("111111111111111111111111\n");
            destroy_bftree(bft);
            destroy_all_dataitem(diarray);


        }

        if (strcmp(argv[1],"search") == 0){

            
            clock_t start,start2,finish,finish2;
            
            printf("start\n");


            char keypath[200] = "/home/wangheyu/Documents/paper/code/final2/VARIABLE_TREE_FINAL/dataoutcome/keys/NPlusOnekey";
            changepathname(keypath,NUMBEROFFILES);

            
            load_NPlusOneKeys(keypath,NPlusOnekey);


            start = clock();


            char treepath[200] = "/home/wangheyu/Documents/paper/code/final2/VARIABLE_TREE_FINAL/dataoutcome/bftree/";

            changepathname(treepath,NUMBEROFFILES);


            DOUBLENODEBLOOMFILTERTREE *bft = load_bf_tree(treepath);

            finish = clock();

            char *word[5] = {\
                "Soufriere",\
                "Department",\
                "phylogeny",\
                "shrinking",\
                "monoclinic"
            };



            double sum = 0;
            

            char searchfilename[200] = "/home/wangheyu/Documents/paper/code/final2/VARIABLE_TREE_FINAL/dataoutcome/searchtreetime/searchtime.txt";
            FILE *f = fopen(searchfilename,"a");
            if(f == NULL){
                fprintf(stderr,"error in open a dataitem file handle");
                exit(-1);
            }
            
            
            double loadtreetime;

            loadtreetime = (double)(finish-start)/CLOCKS_PER_SEC;
            fprintf(f,"the total loadtreetime tree time corresponding to %d files is : %f seconds.\n",NUMBEROFFILES,loadtreetime);




            for (int i = 0; i < 5; i++){
                start2 = clock();
                
                BYTE trapdoor[NUMBEROFKEYS][SHA256_BLOCK_SIZE];
                get_a_trapdoor_of_a_keyword(word[i],trapdoor);


                findv2(bft,trapdoor,1);


                finish2 = clock();
                double searchtime;


                

                
                searchtime = (double)(finish2-start2)/CLOCKS_PER_SEC;
                sum += searchtime;

                fprintf(f,"searching time of word[%d]--%s is %f seconds\n ",i,word[0],searchtime);
            }

            fprintf(f,"average searching time  is %f seconds \n ",sum/5.0);

            fprintf(f,"@@@@@@@@@@@############$$$$$$$$$$$woshihualidefenjiexian$$$$$$$$$$$############@@@@@@@@@@@\n\n");

            destroy_bftree(bft);




            fclose(f);



        }



    


    }

    return 0;
}