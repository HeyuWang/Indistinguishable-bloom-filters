#include "config.h"
#include "bloomfilter.h"




enum tree_type{FIX,VARIABLE};

typedef struct {
    DOUBLENODEBLOOMFILTER **data;     // array of pointers to BLOOMFILTERs (BFs)
    unsigned int size; 
    unsigned int leavesSize;               // number of BFs in tree
    short built;                       // indicate whether tree has been fully built (if bottom-up construction, this will initially be false)
    enum tree_type type;              // fixed-width BFs or variable based on depth?
}DOUBLENODEBLOOMFILTERTREE;




int countTheNumberOfNodesAboveTheNode(unsigned int Numberofnodes);
short is_leaf(DOUBLENODEBLOOMFILTERTREE *bft,unsigned int i);
DOUBLENODEBLOOMFILTERTREE *init_fixed_bf_tree(unsigned int nodes, unsigned long bf_size_in_bytes);
void build_bf_tree(DOUBLENODEBLOOMFILTERTREE *bft);
void destroy_bftree(DOUBLENODEBLOOMFILTERTREE *bft);
void findv2(DOUBLENODEBLOOMFILTERTREE *bft, BYTE trapdoor[][SHA256_BLOCK_SIZE], int i);
DOUBLENODEBLOOMFILTER *copy(DOUBLENODEBLOOMFILTER *bf1);
DOUBLENODEBLOOMFILTER *bf_union(DOUBLENODEBLOOMFILTER *bf1,DOUBLENODEBLOOMFILTER *bf2);
void saveDoubleNodeBloomfilterTree(char *filename, DOUBLENODEBLOOMFILTERTREE *bft);
DOUBLENODEBLOOMFILTERTREE *load_bf_tree(char *filename);