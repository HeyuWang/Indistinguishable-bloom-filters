#ifndef KEY128BITGENERATOR_H
#define KEY128BITGENERATOR_H
#include "config.h"

typedef unsigned char key[keybytenumber+1]; 

void key128bitGenerator(key k);
unsigned int getarandomint();
void produceNPlusOnekeys(BYTE KplusOneKEY[][keybytenumber+1]);

#endif