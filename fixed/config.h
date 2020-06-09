#ifndef CONFIG_H
#define CONFIG_H

typedef unsigned char BYTE;

#define keybytenumber 16
#define NUMBEROFKEYS 3
#define SHA256_BLOCK_SIZE 32
#define SHA1_BLOCK_SIZE 20


#define LOGGING 1   

#define MAXWORD 1024

#define NUMBEROFFILES 5000








unsigned char NPlusOnekey[NUMBEROFKEYS+1][keybytenumber+1];



#endif