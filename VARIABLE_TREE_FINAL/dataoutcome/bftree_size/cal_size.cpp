#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;



int countTheNumberOfNodesAboveTheNode(unsigned int Numberofnodes){
    unsigned int sum = 0;
    unsigned int n = Numberofnodes;
    while( n != 2){
        if(n<2)
            break;
        sum += ((n+1)/2);
        n = (n+1)/2;
    }
    sum += 1;
    return sum;
}

int main(){
    double ln2 = 0.6931471805599453;
    int NUMBEROFKEYS = 3;

    string filename{"fixed_size"};
    ofstream ofs{filename,ios_base::app};

    for(int i = 1; i < 11; i++){
        int nof = 500*i;
        double rootnodesize = (NUMBEROFKEYS/ln2)*(nof*99.27)/8;
        int above = countTheNumberOfNodesAboveTheNode(nof);
        int totalnode = above+nof;
        int level = (unsigned int)log2(totalnode)+1;

        double size = (level)*rootnodesize*2;
        ofs<<"the variable tree size of "<<nof<<" leaf node is "<<size<<" byte\n";

    }

}