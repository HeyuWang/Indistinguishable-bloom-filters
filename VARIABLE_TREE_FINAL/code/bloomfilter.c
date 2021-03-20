#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "bloomfilter.h"
#include "key128bitGenerator.h"
#include "csprng.h"
#include "sha1.h"
#include "helper.h"
#include "config.h"





DOUBLENODEBLOOMFILTER *init_empty_BF(unsigned long size){
    DOUBLENODEBLOOMFILTER *bf;

    if(!(bf = (DOUBLENODEBLOOMFILTER *)calloc(1,sizeof(DOUBLENODEBLOOMFILTER)))){
        fprintf(stderr,"[*] Error in initializing bloom filter.\n");
        exit(-1);
    }

    if (!(bf->array = calloc(size, sizeof(short)))) {
        fprintf(stderr, "[*] Error in initializing bloom filter array.\n");
        exit(-1);
    }

    bf->size = size;
    // bf->amount_of_keyword = 0;
    bf->rb = (unsigned int) getarandomint();

    for(unsigned long i = 0; i < size * 8; i++){
        if((getChosenCellLocation(i,bf->rb,bf->size)) == 0)
            bf->array[i/8] |= (0x2 << (2 * (i%8)));
        else
            bf->array[i/8] |= (0x1 << (2 * (i%8)));
    }

    bf->leftparentnode = bf->rightparentnode = 0;

    return bf;
}

DOUBLENODEBLOOMFILTER *init_empty_BFv2(unsigned long size){
    DOUBLENODEBLOOMFILTER *bf;

    if(!(bf = (DOUBLENODEBLOOMFILTER *)calloc(1,sizeof(DOUBLENODEBLOOMFILTER)))){
        fprintf(stderr,"[*] Error in initializing bloom filter.\n");
        exit(-1);
    }

    if (!(bf->array = calloc(size, sizeof(short)))) {
        fprintf(stderr, "[*] Error in initializing bloom filter array.\n");
        exit(-1);
    }

    bf->size = size;
    // bf->amount_of_keyword = 0;


    bf->leftparentnode = bf->rightparentnode = 0;

    return bf;
}


void destroy_bf(DOUBLENODEBLOOMFILTER *bf){
    free(bf->array);
    free(bf);
}









void getonehashvalue(BYTE onehashval[], BYTE key[], const char *keyword){
    hmac_sha256(onehashval,keyword,strlen(keyword),key,strlen(key));
}

void getNhashvalue(BYTE Nhashvalue[][SHA256_BLOCK_SIZE],const char *keyword)
{
    for(int i = 0; i < NUMBEROFKEYS; i++)
        getonehashvalue(Nhashvalue[i],NPlusOnekey[i],keyword);
}
















void add_one_hashval_to_bloomfilter(DOUBLENODEBLOOMFILTER *bf, BYTE onehashval[SHA256_BLOCK_SIZE]){
    unsigned short bit_pos;
    unsigned long shor_pos;
    unsigned long bit_postmp = mod32(onehashval,bf->size* 8);
    shor_pos = bit_postmp >> 3;
    bit_pos = bit_postmp & 0x7;

    char chosen_pos = getChosenCellLocation(bit_postmp,bf->rb,bf->size);
    if(chosen_pos == 0)
        set_downchosenPositionToOne_upUnchosenPositionToZero(bf->array,shor_pos,bit_pos);
    else 
        set_upchosenPositionToOne_downUnchosenPositionToZero(bf->array,shor_pos,bit_pos);
}



void add_N_hashval_to_bloomfilter(DOUBLENODEBLOOMFILTER *bf, BYTE Nhashvalue[][SHA256_BLOCK_SIZE]){
    for(int i = 0; i < NUMBEROFKEYS; i++)
        add_one_hashval_to_bloomfilter(bf,Nhashvalue[i]);
    // bf->amount_of_keyword++;
}

bool getChosenCellLocation(unsigned long modnumber, unsigned int rb,unsigned long size){
    BYTE hashvalueOFkplusone[SHA256_BLOCK_SIZE];
    char mod2[11]; // unsigned long max is 4294967295 10weishu

    itoa(modnumber,mod2);
    getonehashvalue(hashvalueOFkplusone,NPlusOnekey[NUMBEROFKEYS],mod2);


    for(int i = 0 ;i < 4;i++)
        hashvalueOFkplusone[31-i] = hashvalueOFkplusone[31-i] ^ ((rb >> 8*i)&0xff);

    BYTE sha1hashvalue[20];
    sha1_get(sha1hashvalue,hashvalueOFkplusone,SHA256_BLOCK_SIZE);
    bool final = mod20(sha1hashvalue,2);   
    return final;
}







void get_a_trapdoor_of_a_keyword(const char *keyword,BYTE trapdoor[][SHA256_BLOCK_SIZE]){
    getNhashvalue(trapdoor,keyword);
}

bool is_trapdoor_in_bloomV2(DOUBLENODEBLOOMFILTER *bf, BYTE trapdoor[][SHA256_BLOCK_SIZE]){
    short count = 0;
    for(int i = 0; i < NUMBEROFKEYS; i++){
        unsigned short bit_pos;
        unsigned long shor_pos;
        unsigned long bit_postmp = mod32(trapdoor[i],bf->size*8);

        shor_pos = bit_postmp >> 3;
        bit_pos = bit_postmp &0x7;

        BYTE hashvalueOFkplusone[SHA256_BLOCK_SIZE];
        char mod2[12]; // unsigned long max is 4294967295 10weishu

        itoa(bit_postmp,mod2);
        getonehashvalue(hashvalueOFkplusone,NPlusOnekey[NUMBEROFKEYS],mod2);
        char chosen_pos = getChosenCellLocationV2(hashvalueOFkplusone,bf->rb);

        if(chosen_pos == 0)
            if (((bf->array[shor_pos]>>(2*bit_pos))&0x01) == 1){

                count++;
            }
            else
                return 0;
                
        else if(chosen_pos == 1)
            if (((bf->array[shor_pos]>>(2*bit_pos))&0x02)==2){
                count++;
            }
            else 
                return 0; 
    }

    if(count == NUMBEROFKEYS)
        return 1;
}

bool getChosenCellLocationV2(BYTE hashvalueOFkplusone[],unsigned int rb){

    BYTE hashvalueOFkplusonetmp[32];
    for(int i = 0; i < 28;i++)
        hashvalueOFkplusonetmp[i] = hashvalueOFkplusone[i];
    for(int i = 0 ;i < 4;i++)
        hashvalueOFkplusonetmp[31-i] = hashvalueOFkplusone[31-i] ^ ((rb >> 8*i)&0xff);


    BYTE sha1hashvalue[20];
    sha1_get(sha1hashvalue,hashvalueOFkplusonetmp,SHA256_BLOCK_SIZE);
    char final = mod20(sha1hashvalue,2);   
    return final;
}








void set_upchosenPositionToOne_downUnchosenPositionToZero(unsigned short *array,unsigned long shor_pos,short bit_pos)
{
    unsigned short tmp1 = (unsigned short)(array[shor_pos]>>2*(bit_pos+1))<<2*(bit_pos+1);
    unsigned short tmp2 = (unsigned short)(array[shor_pos]<<(16-2*bit_pos))>>(16-2*bit_pos);
    unsigned short tmp3 = 2<<(2*bit_pos);
    array[shor_pos] = tmp1|tmp2|tmp3;
}


void set_downchosenPositionToOne_upUnchosenPositionToZero(unsigned short *array,unsigned long shor_pos,short bit_pos)
{
    unsigned short tmp1 = (unsigned short)(array[shor_pos]>>2*(bit_pos+1))<<2*(bit_pos+1);
    unsigned short tmp2 = (unsigned short)(array[shor_pos]<<(16-2*bit_pos))>>(16-2*bit_pos);
    unsigned short tmp3 = 1<<(2*bit_pos);
    array[shor_pos] = tmp1|tmp2|tmp3;
}








short ChosenCellLocationIsChosen(DOUBLENODEBLOOMFILTER *bf,unsigned long shor_pos,unsigned long bitnumber){
    char chosen_pos = getChosenCellLocation(bitnumber,bf->rb,bf->size);
    unsigned char bit_pos = bitnumber &0x7;
    if(chosen_pos == 0)
        if ((((bf->array[shor_pos])>>(2*bit_pos))&0x01) == 1){
            return 1;
        }
        else
            return 0;
            
    else if(chosen_pos == 1)
        if ((((bf->array[shor_pos])>>(2*bit_pos))&0x02)==2){
            return 1;
        }
        else 
            return 0; 

}

void setChosenCellTo1(DOUBLENODEBLOOMFILTER *bf,unsigned long shor_pos,unsigned long bitnumber){
    char chosen_pos = getChosenCellLocation(bitnumber,bf->rb,bf->size);
    
    unsigned char bit_pos = bitnumber &0x7;

    if(chosen_pos == 0)
        set_downchosenPositionToOne_upUnchosenPositionToZero(bf->array,shor_pos,bit_pos);

    else 
        set_upchosenPositionToOne_downUnchosenPositionToZero(bf->array,shor_pos,bit_pos);
}

