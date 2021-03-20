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
            
            printf("start build tree\n");

            char* datasetpath = "/home/wangheyu/Documents/paper/code/dataset/NSF Research Award Abstract set/allpart";
            produceNPlusOnekeys(NPlusOnekey);

            char keypath[200] = "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/keys/NPlusOnekey";

            changepathname(keypath,NUMBEROFFILES);

            save_NPlusOneKeys(keypath);

            printf("%d\n",NUMBEROFKEYS);




            changepathname(dataitempath,NUMBEROFFILES);

            recursive_generate_di_and_save_di(datasetpath);



            start = clock();


            double ln2 = 0.6931471805599453;
            double rootnodesize = (NUMBEROFKEYS/ln2)*(NUMBEROFFILES*99.27)/8;



            DOUBLENODEBLOOMFILTERTREE *bft = init_fixed_bf_tree(NUMBEROFFILES,(unsigned long)rootnodesize);
            unsigned int tmp = countTheNumberOfNodesAboveTheNode(bft->leavesSize);


            printf("---------------------\n");
            for(int i = 1; i < bft->leavesSize + 1; i++){
                printf("bft->leavesSize: %d\n",bft->leavesSize);
                // dataitem *di = init_dataitem(i);
                add_a_dataitem_to_bloomfilter(diarray[i],bft->data[i+tmp]);
                // print_dataitem(diarray[i]);
                // saveoneDataItems("dataitem.txt",di);
                // destroy_dataitem(di);
            }


                build_bf_tree(bft);

                char treepath[200] = "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/bftree/";

                changepathname(treepath,NUMBEROFFILES);

                saveDoubleNodeBloomfilterTree(treepath,bft);

                finish = clock();
                double duration;
                duration = (double)(finish-start)/CLOCKS_PER_SEC;
                printf("the total build tree time is : %f seconds.\n",duration);

                char* filename = "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/buildtreetime/buildtime";
                savebuildtreetime(filename,duration,(unsigned long)rootnodesize);


                destroy_bftree(bft);
                destroy_all_dataitem(diarray);


                return 0;

        }

        if (strcmp(argv[1],"search") == 0){

            
            clock_t start,start2,finish,finish2;
            
            printf("start\n");


            char keypath[200] = "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/keys/NPlusOnekey";
            changepathname(keypath,NUMBEROFFILES);

            
            load_NPlusOneKeys(keypath,NPlusOnekey);


            start = clock();


            char treepath[200] = "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/bftree/";

            changepathname(treepath,NUMBEROFFILES);

            DOUBLENODEBLOOMFILTERTREE *bft = load_bf_tree(treepath);




            finish = clock();

            char *word[5] = {\
                "Soufriere",\
                "Department",\
                "phylogeny",\
                "shrinking",\
                "elucidated"
            };



            double sum = 0;
            

            char searchfilename[200] = "/home/wangheyu/Documents/paper/code/final2/FIXED_TREE_FIANL/dataoutcome/searchtreetime/searchtime.txt";
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

                fprintf(f,"searching time of word[%d]--%s is %f seconds\n ",i,word[i],searchtime);
            }

            fprintf(f,"average searching time  is %f seconds \n ",sum/5.0);

            fprintf(f,"@@@@@@@@@@@############$$$$$$$$$$$woshihualidefenjiexian$$$$$$$$$$$############@@@@@@@@@@@\n\n");

            destroy_bftree(bft);

            fclose(f);
        }

    }





    return 0;
}