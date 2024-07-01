//
// Created by Federico Saitta on 29/06/2024.
//

#include "globals.h"

const char piece_char[6] =
{
    'P', 'N', 'B', 'R', 'Q', 'K'
};

const int piece_value[6] =
{
    100, 300, 300, 500, 900, 10000
};

const int init_color[64] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

const int init_board[64] =
{
    3, 1, 2, 4, 5, 2, 1, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 2, 4, 5, 2, 1, 3
};

const int col[64]=
{
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7
};

const int row[64]=
{
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7
};

const int nwdiag[64]=
{
    14,13,12,11,10, 9, 8, 7,
    13,12,11,10, 9, 8, 7, 6,
    12,11,10, 9, 8, 7, 6, 5,
    11,10, 9, 8, 7, 6, 5, 4,
    10, 9, 8, 7, 6, 5, 4, 3,
     9, 8, 7, 6, 5, 4, 3, 2,
     8, 7, 6, 5, 4, 3, 2, 1,
     7, 6, 5, 4, 3, 2, 1, 0
};

const int nediag[64]=
{
    7, 8,9,10,11,12,13,14,
    6, 7,8, 9,10,11,12,13,
    5, 6,7, 8, 9,10,11,12,
    4, 5,6, 7, 8, 9,10,11,
    3, 4,5, 6, 7, 8, 9,10,
    2, 3,4, 5, 6, 7, 8, 9,
    1, 2,3, 4, 5, 6, 7, 8,
    0, 1,2, 3, 4, 5, 6, 7
};

const int bishopRelevantBits[64]{
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
const int rookRelevantBits[64] {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

BITBOARD pawnAttacks[2][64];


BITBOARD maskPawnAttacks(const int square, const int side) {

    U64 attacks{};
    U64 board{};

    setBit(board, square);
    if (!side) { // for white
        if (notHFile & (board << 7)){ attacks |= (board << 7); }
        if (notAFile & (board << 9)){ attacks |= (board << 9); }
    }
    else { // for black
        if (notAFile & (board >> 7)){ attacks |= (board >> 7); }
        if (notHFile & (board >> 9)){ attacks |= (board >> 9); }
    }

    return attacks;
}
BITBOARD maskKnightMoves(const int square) {

    U64 attacks{};
    U64 board{};

    setBit(board, square);

    if ((board << 17) & notAFile) attacks |= board << 17;
    if ((board << 15) & notHFile) attacks |= board << 15;
    if ((board << 10) & notABFile) attacks |= board << 10;
    if ((board << 6) & notHGFile) attacks |= board << 6;

    if ((board >> 17) & notHFile) attacks |= board >> 17;
    if ((board >> 15) & notAFile) attacks |= board >> 15;
    if ((board >> 10) & notHGFile) attacks |= board >> 10;
    if ((board >> 6) & notABFile) attacks |= board >> 6;

    return attacks;
}
BITBOARD maskKingMoves(const int square) {
    U64 attacks{};
    U64 board{};

    setBit(board, square);

    if (board << 8) attacks |= (board << 8);
    if (board >> 8) attacks |= (board >> 8);
    if ((board << 1) & notAFile) {
        attacks |= (board << 1) | (board << 9) | (board >> 7);
    }
    if ((board >> 1) & notHFile) {
        attacks |= (board >> 1) | (board >> 9) | (board << 7);
    }

    return attacks;
}



// Write tests for these two functions


// This generates relevant bishop occupancy squares
// in the mask we do not consider the edge landing squares
// not storing this anywhere at the moment
BITBOARD maskBishopMoves(const int square) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; r++, f++) { attacks |= (1ULL << (r * 8 + f)); }

    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; r--, f++) { attacks |= (1ULL << (r * 8 + f)); }
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; r++, f--) { attacks |= (1ULL << (r * 8 + f)); }

    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; r--, f--) { attacks |= (1ULL << (r * 8 + f)); }

    return attacks;
}

// need to implement this
BITBOARD maskRookMoves(const int square) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1; r < 7; r++) { attacks |= (1ULL << (r * 8 + tf)); }
    for (r = tr - 1; r > 0; r--) { attacks |= (1ULL << (r * 8 + tf)); }

    for (f = tf + 1; f < 7; f++) { attacks |= (1ULL << (tr * 8 + f)); }
    for (f = tf - 1; f > 0; f--) { attacks |= (1ULL << (tr * 8 + f)); }

    return attacks;
}


void initLeaperPiecesArrays() {

    for (int i = 0; i < 64; i ++) {
        pawnAttacks[0][i] =  maskPawnAttacks(i, 0);
        pawnAttacks[1][i] =  maskPawnAttacks(i, 1);

        bitKnightMoves[i] =  maskKnightMoves(i);
        bitKingMoves[i] =  maskKingMoves(i);
    }
}


// In this function we want to hit the board edges unlike before
// at the moements lets assume we can eat the blockers
// movegenerators will deal with this difference
BITBOARD bishopAttacksOnTheFly(const int square, const U64 blocker) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }

    return attacks;
}

BITBOARD rookAttacksOnTheFly(const int square, const U64 blocker) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if ( (1ULL << (r * 8 + tf)) & blocker) break;
    }
    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if ( (1ULL << (r * 8 + tf)) & blocker) break;
    }

    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if ( (1ULL << (tr * 8 + f)) & blocker) break;
    }
    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if ( (1ULL << (tr * 8 + f)) & blocker) break;
    }

    return attacks;
}


BITBOARD setOccupancies(const int index, const int bitInMask, U64 attackMask) {
    U64 occupancy{};

    for (int i=0; i < bitInMask; i++) {

        const int square {getLeastSigBitIndex(attackMask)};
        setBitFalse(attackMask, square);

        if (index & (1ULL << i)) {
            occupancy |= (1ULL << square);
        }

    }
    return occupancy;
}


// from this https://www.reddit.com/r/chessprogramming/comments/wsrf3s/is_there_any_place_i_can_copy_fancy_magic_numbers/
const U64 bishopMagics[64] {
    54045411736421449ULL,
    3467949876944314368ULL,
    1170054334914564ULL,
    9386668213868429344ULL,
    2612653001713909776ULL,
    143074487959556ULL,
    144405463509778640ULL,
    2314868913337286662ULL,
    1161933171206914308ULL,
    5102013192899726ULL,
    2306410683698270464ULL,
    2343024120192851072ULL,
    75437501641129984ULL,
    8942543438848ULL,
    576498144559498370ULL,
    9313461918075293712ULL,
    18041957234770120ULL,
    9007906867522064ULL,
    18577417326039296ULL,
    72198365919854592ULL,
    291045211837106176ULL,
    4728814801970530308ULL,
    70385941350476ULL,
    72199439661474050ULL,
    1227301284920249602ULL,
    4904472908247872790ULL,
    149533851001120ULL,
    576746625460996128ULL,
    4686136700865880064ULL,
    92330389433092353ULL,
    23648296635597120ULL,
    1232552543291392ULL,
    2348574017462305ULL,
    74311661594870048ULL,
    13835098222891107329ULL,
    2201179652224ULL,
    1152992149307130112ULL,
    144397766860079688ULL,
    1299994464869172242ULL,
    2306124761249416192ULL,
    23089753846974488ULL,
    7512568235569455104ULL,
    18579548627011656ULL,
    1126038738339843ULL,
    672935777931282ULL,
    2323875584167379009ULL,
    51793734359057408ULL,
    2594777699881387520ULL,
    1139147805032960ULL,
    4683780038219268372ULL,
    1161232523493384ULL,
    72706313822208ULL,
    653022014721892608ULL,
    10408980038721605632ULL,
    18070782873796608ULL,
    4972541478955057216ULL,
    10151825236041728ULL,
    8521389665689600ULL,
    14992505291550049312ULL,
    290271107551376ULL,
    2594076822951969037ULL,
    270224911207891468ULL,
    22817078453010948ULL,
    153157608243496480ULL
};

const U64 rookMagics[64] {
    2918332697048780928ULL,
    54044432483225602ULL,
    3206580529027613000ULL,
    324264121107222528ULL,
    432358827626201096ULL,
    144119654858752008ULL,
    504403793954218240ULL,
    108086684901442816ULL,
    10555321290359856ULL,
    73253863225839626ULL,
    9369879830954247040ULL,
    5188287542587297793ULL,
    289637785395269889ULL,
    4614500785407920208ULL,
    1442277815999267236ULL,
    291045144457675786ULL,
    306262916611457058ULL,
    1170938652432286848ULL,
    1225261674207911936ULL,
    576743327060205580ULL,
    2252349703979136ULL,
    563499751179264ULL,
    1152922608413704704ULL,
    13512997914017857ULL,
    351884523110496ULL,
    4611756390394892289ULL,
    10394325534313029632ULL,
    9313461623736238208ULL,
    5226990463546036232ULL,
    567350155805184ULL,
    284240936185921ULL,
    2270500101333060ULL,
    36028934730547264ULL,
    1193454038830563330ULL,
    140875002810368ULL,
    72092847146274048ULL,
    4512397909820416ULL,
    11241547688623212548ULL,
    2269426426610177ULL,
    4512949200749569ULL,
    1765420262378029056ULL,
    650911420596224ULL,
    27022972707602450ULL,
    2252907916300288ULL,
    18295907847045126ULL,
    10133116375040128ULL,
    577639433099280520ULL,
    9511638697972989988ULL,
    6971572361690611840ULL,
    58547070035820608ULL,
    141149814653568ULL,
    13511907521595648ULL,
    9147953923489920ULL,
    1407684159209728ULL,
    4621396913713905792ULL,
    11711048431922381312ULL,
    4611721481982148609ULL,
    3458928342135415298ULL,
    35184510506241ULL,
    9368086462124261389ULL,
    288511919881977861ULL,
    1873779031699105833ULL,
    5764643842386034820ULL,
    862571370643778ULL
};


void initSliderAttacks(const int bishop) {

    for (int square=0; square < 64; square++) {
        bitBishopMoves[square] = maskBishopMoves(square);
        bitRookMoves[square] = maskRookMoves(square);

        const U64 attackMask { bishop ? bitBishopMoves[square] : bitRookMoves[square]};

        const int relevantBitsCount{ countBits(attackMask)};
        const int occupacyIndex { (1 << relevantBitsCount) };

        for (int index=0; index < occupacyIndex; index++) {
            if (bishop) {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const int magicIndex = (occupancy * bishopMagics[square]) >> (64 - bishopRelevantBits[square]);
                bitBishopMovesTable[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);

            } else {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const int magicIndex = (occupancy * rookMagics[square]) >> (64 - rookRelevantBits[square]);
                bitRookMovesTable[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);

            }

        }

    }

}




