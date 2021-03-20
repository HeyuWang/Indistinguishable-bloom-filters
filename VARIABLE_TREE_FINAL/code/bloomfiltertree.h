#include "config.h"
#include "bloomfilter.h"
#include "dataitem.h"




enum tree_type{FIX,VARIABLE};

typedef struct {
    DOUBLENODEBLOOMFILTER **data;     // array of pointers to BLOOMFILTERs (BFs)
    unsigned int size; 
    unsigned int leavesSize;               // number of BFs in tree
    short built;                       // indicate whether tree has been fully built (if bottom-up construction, this will initially be false)
    enum tree_type type;              // fixed-width BFs or variable based on depth?
}DOUBLENODEBLOOMFILTERTREE;




int countTheNumberOfNodesAboveTheseNodes(unsigned int Numberofnodes);
short is_leaf(DOUBLENODEBLOOMFILTERTREE *bft,unsigned int i);
DOUBLENODEBLOOMFILTERTREE *init_variable_bf_tree(unsigned int node, unsigned long root_bf_size_in_bytes);
void findParentNodeForInitializedVariableTree(DOUBLENODEBLOOMFILTERTREE *bft);
void destroy_bftree(DOUBLENODEBLOOMFILTERTREE *bft);
void addDataitemToAVariableTreeNode(DOUBLENODEBLOOMFILTERTREE *bft,dataitem *di[],int current_node,int i);
void findv2(DOUBLENODEBLOOMFILTERTREE *bft, BYTE trapdoor[][SHA256_BLOCK_SIZE], int i);
void findv3(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[5][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i);
void findv3_disjunctive(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[5][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i);
void findv4(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[10][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i);
void findv4_disjunctive(DOUBLENODEBLOOMFILTERTREE *bft, BYTE multitrapdoor[10][NUMBEROFKEYS][SHA256_BLOCK_SIZE], int i);

void saveDoubleNodeBloomfilterTree(char *filename, DOUBLENODEBLOOMFILTERTREE *bft);
DOUBLENODEBLOOMFILTERTREE *load_bf_tree(char *filename);