#include <iostream>

#include "globals.h"
#include "misc.h"




int main() {

    BITBOARD a{0ULL};
    setBit(a, A3);
    setBit(a, F5);
    setBit(a, B8);
    printBitBoard(a);

    mirrorHorizontal(a);
    printBitBoard(a);


}
// This should contain functions for starting the UCI protocol