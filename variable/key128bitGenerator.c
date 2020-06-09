#include <stdio.h>
#include "csprng.h"
#include "key128bitGenerator.h"

 // 16*8 = 128
void key128bitGenerator(key k){
    CSPRNG rng = csprng_create( rng );
    if (!rng)
  {
    fprintf( stderr, "%s\n", "No CSPRNG! Fooey!" );
   
  }
  int n;
  for (n = 0; n < keybytenumber; n++)
  {
    char c = ((unsigned)csprng_get_int( rng ) % 95) + 32;
    k[n] = c;
  }
  k[keybytenumber] = (char)0;
  rng = csprng_destroy( rng );

}

unsigned int getarandomint(){
    unsigned int a;
    CSPRNG rng = csprng_create();
    if (!rng){
        fprintf( stderr, "%s\n", "Alas, failed to initialize the OS CSPRNG." );
    return 1;
    }
    a = (unsigned int)csprng_get_int( rng ) ;
    
    rng = csprng_destroy( rng );  

    return a;  
}


void produceNPlusOnekeys(BYTE KplusOneKEY[][keybytenumber+1]){ 
    #ifdef LOGGING
        printf("---------------------------start generate keys--------------------------\n");
    #endif 
    for(int i = 0; i < NUMBEROFKEYS+1; i++)
        key128bitGenerator(KplusOneKEY[i]);
}
// main(){
//     key k1,k2;
//     key128bitGenerator(k1);
//     key128bitGenerator(k2);
//     for (int n = 0; n < 16; n++)
//         printf("k1[%d] = 0x%x\tk2[%d] = 0x%x\n",n,k1[n],n,k2[n]);
// }