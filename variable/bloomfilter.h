#ifndef BLOOMFILTER_H
#define	BLOOMFILTER_H

#include <stdbool.h>
#include "sha1.h"
#include "hmac_sha256.h"
#include "config.h"





typedef struct {

    unsigned short *array;          //recode the data is stored in double_node_bloomfilter

    unsigned long size;             //record the number of shorts in this BF

    unsigned int rb;                //a random mask number 0-65535 

    unsigned int leftparentnode;

    unsigned int rightparentnode;


    // unsigned int amount_of_keyword;
    // unsigned char *chosen_and_unchosen;


}DOUBLENODEBLOOMFILTER;










DOUBLENODEBLOOMFILTER *init_empty_BF(unsigned long size);
DOUBLENODEBLOOMFILTER *init_empty_BFv2(unsigned long size);
void destroy_bf(DOUBLENODEBLOOMFILTER *bf);



void getonehashvalue(BYTE onehashval[], BYTE key[], const char *keyword);
void getNhashvalue(BYTE Nhashvalue[][SHA256_BLOCK_SIZE],const char *keyword);



void add_one_hashval_to_bloomfilter(DOUBLENODEBLOOMFILTER *bf, BYTE onehashval[SHA256_BLOCK_SIZE]);
void add_N_hashval_to_bloomfilter(DOUBLENODEBLOOMFILTER *bf, BYTE Nhashvalue[][SHA256_BLOCK_SIZE]);
bool getChosenCellLocation(unsigned long modnumber, unsigned int rb,unsigned long size);



void get_a_trapdoor_of_a_keyword(const char *keyword,BYTE trapdoor[][SHA256_BLOCK_SIZE]);
bool is_trapdoor_in_bloomV2(DOUBLENODEBLOOMFILTER *bf, BYTE trapdoor[][SHA256_BLOCK_SIZE]);
bool getChosenCellLocationV2(BYTE hashvalueOFkplusone[],unsigned int rb);



void set_upchosenPositionToOne_downUnchosenPositionToZero(unsigned short *array,unsigned long shor_pos,short bit_pos);
void set_downchosenPositionToOne_upUnchosenPositionToZero(unsigned short *array,unsigned long shor_pos,short bit_pos);
short ChosenCellLocationIsChosen(DOUBLENODEBLOOMFILTER *bf,unsigned long shor_pos,unsigned long bitnumber);
void setChosenCellTo1(DOUBLENODEBLOOMFILTER *bf,unsigned long shor_pos,unsigned long bitnumber);





#endif