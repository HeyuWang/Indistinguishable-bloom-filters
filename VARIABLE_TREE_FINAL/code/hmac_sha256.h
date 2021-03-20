#ifndef HMAC_SHA256_H
#define HMAC_SHA256_H

/************************HEADER FILES**************/
#include "sha256.h"
#include "config.h"

/**********************MACROS****************/

#define B 64
#define L (32)
#define K (32 * 2)

#define I_PAD 0x36
#define O_PAD 0x5C



/*********************DATA TYPES*****************/

          //32-bit word, change to "long" for 16-bit machines



/*******************FUNCTION DECLARATIONS***************/
 

void hmac_sha256 (BYTE out[32],
             const BYTE data[], size_t data_len,
             const BYTE key[], size_t key_len);

void reverse(char s[]);
void itoa(int n, char s[]);

#endif