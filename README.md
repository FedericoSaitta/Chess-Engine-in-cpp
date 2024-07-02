## Chess engine in c++

- Throughout magic bitboard initialization process the tables are referred to 
as bitKnightAttacks, bitPawnAttacks, etc. as in this stage we are assuming that when a blocker piece is encountered we are always able to capure it.
The distinction between attack and move is made in the move generator where at run-time we check if the blocker is part of 
ours or our enemy's bitboard.