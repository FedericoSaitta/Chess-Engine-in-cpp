# <div align="center">Aramis</div>
<div align="center">

[![lichess-bullet](https://lichess-shield.vercel.app/api?username=Aramis-Bot&format=bullet)](https://lichess.org/@/Aramis-Bot/perf/bullet)
[![lichess-blitz](https://lichess-shield.vercel.app/api?username=Aramis-Bot&format=blitz)](https://lichess.org/@/Aramis-Bot/perf/blitz)
[![lichess-rapid](https://lichess-shield.vercel.app/api?username=Aramis-Bot&format=rapid)](https://lichess.org/@/Aramis-Bot/perf/rapid)
</div>

**[Play against Aramis on Lichess](https://lichess.org/@/Aramis-Bot)**

---

## Introduction

Aramis is a uci-compliant chess engine that does not have its own GUI. This project has been largely inspired by my previously built python chess engine which I estimate to play around 1400 - 1500 ELO.
Because of the performance improvement brought by the C++ compiler, this engine has already surpassed its predecessor and will
be soon sent to CCRL to test its performance against other engines. Please contact me for any advice, critique or question about the bot :)
---

## Table of Contents

- [Build Guide](#build-guide)
  - [Prerequisites](#prerequisites)
  - [Cloning the Repository](#cloning-the-repository)
  - [Building the Project](#building-the-project)
  - [Running the Executable](#running-the-executable)
- [UCI Commands](#uci-commands)
- [Extra-UCI Commands](#extra-uci-commands)
- [Technical Details](#technical-details)
  - [Move Generation](#move-generation)
  - [Board Evaluation](#board-evaluation)
  - [Searching Algorithm Features](#searching-algorithm-features)
  - [Time Management](#time-management)
  - [Perft Test](#perft-test)
  - [Engine vs. Engine Match Results](#engine-vs-engine-match-results)
- [Patches](#patches)
- [Credits](#credits)

## Build Guide:
This project uses **CMake** for build configuration.

### Prerequisites
Before you start, make sure you have the following software installed:
- **CMake**: Version 3.15 or higher. [Download CMake](https://cmake.org/download/)
- **Compiler**: Ensure you have a C++ compiler installed (GCC, Clang, or MSVC).
- **Git**: For cloning the repository. [Download Git](https://git-scm.com/)

### Cloning the Repository
To get started, clone the repository using the following command:
```bash
git clone https://github.com/FedericoSaitta/Chess-Engine-in-cpp
cd ChessEngine
```
### Building the project
Head to the directory where the repository is located and run:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
### Running the executable
```bash
./ChessEngine
```

### UCI commands:
`position`:
- `startpos`
- `fen`
- `fen moves`

`go`:
- `depth x`
- `perft x`
- `wtime x btime x`:
  - `winc x binc x`
  - `movestogo x`

`setoption name Hash value x`, by default the hash size is of 256 MegaBytes. \
`uci`, `isready`, `ucinewgame`, `quit`.

### Extra-UCI commands:
`bench` to run a pre-established search-test suite.
- It is useful for checking the correct engine behaviour in the case you build the project on your own. 
As of v1.3.0 (25/07/2024) the correct results are: 
- Nodes: 7.2748 Million, Branching Ratio: 3.14648 and the Time is depended on hardware, though the test should not take
more than 15 seconds on most machines.

`display` to print the current board-state in the terminal, on unix machines pieces will be displayed, on windows, letters will be used
to represent the pieces.

## Technical Details:

#### Move Generation:
- [X] Plain Magic Bitboards for all sliding and leaping pieces.
- [X] Pawn pushes, En-Passant and Castling move generation is done on the fly.
- [X] Copy-Restore approach is used to play and undo moves on the board
- [ ] Make-UnMake move and full Position class implementation
- [ ] Legal move generation
- [ ] Staged move generation

#### Board Evaluation:
- [X] Hand-Crafted-Evaluation tuned with Texel tuner
- [X] Tapered evaluation which considers piece position on the board, and their mobility
- [X] Rooks and Queens gain bonuses for being on semi-open and open files
- King evaluation:
  - [X] semi-open and open file malus
  - [X] pawn-shield bonus
- Pawn evaluation:
  - [X] isolated, passed and double pawns
  - [ ] pawn phalanx
  - [ ] unstoppable passer

#### Searching Algorithm Features:
- [X] Move ordering by: Hash move, MVV-LVA, Killer 1, Killer 2, History moves, remaining moves.
- [X] History table ageing after each turn.
- [X] Fail-soft Negamax
- [X] Quiescence search
- [X] Iterative deepening
- [X] Principal Variation Search.
- [X] Aspiration windows
- [X] Check Extension
- [X] Late Move reductions
- [X] Null Move Pruning
- [X] Reverse Futility Pruning
- [X] Delta Pruning
- [X] Razoring
- [X] Late Move Pruning
- [ ] Tuning of Late Move Pruning
- [ ] Futility pruning
- [ ] Improving heuristic 
- [ ] Modern history scores and bonuses
- [ ] SEE pruning in quiesce
- [ ] 50-move rule and insufficient material draws
- [ ] Tuning of search parameters

#### Time Management:
- [X] Hard bound given by (Time / 30) + Increment
- [X] Soft bound given by (Time Per Move) / 3

#### Perft Test:
On a 1.6 GHz Dual-Core Intel Core i5 (Macbook Air 2017), passes the standard.epd test with 4.8 Billion nodes in 184.3 s, so 26 MNps.

#### Engine vs. Engine Match Results: 

Here are the results of each version tested against the previous one. The tests were performed using fast-chess and 
randomized positions from a small book 5 to 10 moves deep and less than 100 centipawn advantage for each side (thanks Stockfish).
Please note that though the versions are improving over time, the gain is not linear, hence the estimate ELO values 
are a rough measure of playing strength, and will only be confirmed or denied once the engine participates in larger 
testing suites.

| Version | ELO Gain | Estimated ELO |
|---------|----------|---------------|
| 1.3.0   | 115 ± 15 | 2250          |
| 1.2.7   | 15 ± 10  | 2140          |
| 1.2.6   | 36 ± 16  | 2130          |
| 1.2.5   | 46 ± 18  | 2110          |
| 1.2.4   | 69 ± 20  | 2080          |
| 1.2.3   | 35 ± 20  | 2030          |
| 1.2.2   | 65 ± 20  | 2010          |
| 1.2.1   | 223 ± 41 | 1950          |
| 1.2.0   | 200 ± 43 | 1730          |
| 1.1.0   | -------- | 1550          |


#### Patches:
- v1.1.0: iterative deepening, negamax search with PVS and quiesce for captures and promotions, MVV-LVA, killer and history move ordering. Simple piece square table evaluation.
- v1.2.0: Zobrist hashing for three-fold check and TT-table, null move pruning and basic LMR.
- v1.2.1: Additions of Pesto Tables and tapered evaluation, aspiration windows.
- v1.2.2: King-safety and pawn consideration (isolated, doubled, passed), piece mobility in middle and endgame.
- v1.2.3: Razoring and static null move pruning
- v1.2.4: Improvement to move sorting algorithm (from O(N^2) to O(NlogN)), added delta pruning
- v1.2.5: More aggressive LMR (now reduces moves by dept/3 after the first six have been searched)
- v1.2.6: Improved LMR formula based on Berserk engine (log * log)
- v1.2.7: Added basic LMP (though this needs to be re-tuned)
- v1.3.0: Few performance improvements in evaluation (+20), switch to fail-soft negamax (+20), bishop pair
bonus and texel tuning (+70)

#### Credits:
- Maksim Korzh for his incredible chess programming in c series which was invaluable while implementing bitboard move-generation and transposition table.
- Gediminas Masaitis for his Texel tuner without which the evaluation would be much weaker.
- Sebastian Lague for his chess videos and small-chessbot tournament which sparked my interest in chess programming.
- The Chess Programming Wiki: https://www.chessprogramming.org/Main_Page. 
