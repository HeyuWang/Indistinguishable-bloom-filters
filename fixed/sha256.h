#ifndef SHA256_H
#define SHA256_H

/************************HEADER FILES**************/

#include <stddef.h>
#include "config.h"
/**********************MACROS****************/



/*********************DATA TYPES*****************/

         //32-bit word, change to "long" for 16-bit machines
typedef unsigned int WORD;

typedef struct{
    BYTE data[64];  // current 512-bit chunk of message data, just like a buffer
    WORD datalen;   // sign the data length of current chunk
    unsigned long long bitlen; // the bit length of the total message
    WORD state[8];  // store the middle state of hash abstract
}SHA256_CTX;


/*******************FUNCTION DECLARATIONS***************/
 

void sha256_init(SHA256_CTX *ctx);
void sha256_updata(SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);


#endif