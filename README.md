# <div align="center">Aramis</div>

<div align="center">
  <img src="AramisLogo.png" alt="Logo" />
</div>

<div align="center">

[![lichess-bullet](https://lichess-shield.vercel.app/api?username=Aramis-Bot&format=bullet)](https://lichess.org/@/Aramis-Bot/perf/bullet)
[![lichess-blitz](https://lichess-shield.vercel.app/api?username=Aramis-Bot&format=blitz)](https://lichess.org/@/Aramis-Bot/perf/blitz)
[![lichess-rapid](https://lichess-shield.vercel.app/api?username=Aramis-Bot&format=rapid)](https://lichess.org/@/Aramis-Bot/perf/rapid)
</div>

**[Play against Aramis on Lichess](https://lichess.org/@/Aramis-Bot)**

## Introduction

Aramis is an uci-compliant chess engine that does not have its own GUI. This project has been largely inspired by my previously built python chess engine which I estimate to play around 1400 - 1500 ELO.
Thanks to the speed brought by c++ many more optimizations were squeezed in the game-tree search bringing Aramis_v1_3_0 to a rating of 2407 in CCRL Blitz.
The latest code for v1.5.0 can be found in the respective branch, though development has been slower due to the demanding computing resources and time required to test new patches, one day I wish to return to this project
and aim for higher spots in the global leaderboards. Please contact me for any advice, critique or question about the bot :)

## Table of Contents

- [Running the Engine](#Running-the-Engine)
- [Build Guide](#build-guide)
  - [Prerequisites](#prerequisites)
  - [Cloning the Repository](#cloning-the-repository)
  - [Building the Project](#building-the-project)
  - [Running the Executable](#running-the-executable)
- [User Details](#User-Details)
- [UCI Commands](#uci-commands)
- [Extra-UCI Commands](#extra-uci-commands)
- [Technical Details](#technical-details)
  - [Move Generation](#move-generation)
  - [Board Evaluation](#board-evaluation)
  - [Searching Algorithm Features](#searching-algorithm-features)
  - [Time Management](#time-management)
  - [Engine Strength](#engine-strength)
- [Credits](#credits)
- [Room For Improvement](#room-for-improvement)

## Running the Engine:
If you are on windows-x64 or macos-x64 head to the 'Releases' section and download the executable.
If you want to build this project, follow the build guide below.

## Build Guide
### Prerequisites
This project uses **CMake** for build configuration.
Before you start, make sure you have the following software installed:
- **CMake**: Version 3.15 or higher. [Download CMake](https://cmake.org/download/)
- **Compiler**: Ensure you have a C++ compiler installed (GCC, Clang, or MSVC).
- **Git**: For cloning the repository. [Download Git](https://git-scm.com/)

### Cloning the Repository
To get started, clone the repository using the following command:
```
git clone https://github.com/FedericoSaitta/Chess-Engine-in-cpp
cd ChessEngine
```
### Building the project
Head to the directory where the repository is located and run:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
If you want to build the project in debug mode, use `-DCMAKE_BUILD_TYPE=Debug ..` instead. Note that in Debug mode
a log-file is generated in the working directory and many run-time tests and sanity checks are performed causing the
engine to slow significantly, Debug mode should not be used for play against humans or other engines.
### Running the executable
For macOS and Linux: `./ChessEngine`
For windows:`ChessEngine.exe`

## User Details:
### UCI commands:
`position`:
- `startpos`
- `fen`
- `fen moves`

`go`:
- `depth x`
- `perft x`
- `wtime x btime x`:
  - `movetime x`
  - `winc x binc x`
  - `movestogo x`

`setoption name Hash value x`, by default the hash size is of 256 MegaBytes. \
`uci`, `isready`, `ucinewgame`, `quit`.

### Extra-UCI commands:
`bench` to run a pre-established search-test suite
`moveOrdering` to display the pseudo-legal moves in the current position and their relative scores
`display` to print the current board-state in the terminal

## Technical Details:
The quoted ELO gains are from SPRT so the figures may not be accurate. 
SPSA and Texel-Tuning brought over 200 ELO so far.

#### Move Generation:
- [X] Plain Magic Bitboards for all sliding and leaping pieces.
- [X] Pawn pushes, En-Passant and Castling move generation is done on the fly.
- [X] Copy-Restore approach is used to play and undo moves on the board
- [X] Make-UnMake move and full Position class implementation
- [ ] Staged move generation
- [ ] Legal move generation


#### Board Evaluation:
- [X] Hand-Crafted-Evaluation tuned with Texel tuner
- [X] Tapered evaluation which considers piece position on the board, and their mobility
- [X] Rooks and Queens gain bonuses for being on semi-open and open files
- King evaluation:
  - [X] semi-open and open file malus (+10)
  - [X] pawn-shield bonus (+10)
- Pawn evaluation:
  - [X] isolated, passed and double pawns (+40)
  - [X] pawn phalanx and protected pawns (+20)
  - [ ] unstoppable passer

#### Searching Algorithm Features:
- [X] Quiescence search
- [X] Iterative deepening
- [X] Principal Variation Search
- [X] Check Extension
- [X] Late Move reductions
- [X] Razoring
- [X] Aspiration windows (+50)
- [X] History malus and gravity (+30)
- [X] Fail-soft Negamax (+20)
- [X] Null Move Pruning (+100)
- [X] Reverse Futility Pruning (+50)
- [X] Delta Pruning (+30)
- [X] Late Move Pruning (+60)
- [X] Transposition table in quiesce (+20)
- [X] SEE pruning in quiesce (+30)
- [X] SEE PVS pruning in negamax (+20)
- [X] SEE move ordering (captures only) (+10)
- [ ] Improving heuristic
- [ ] 50-move rule and insufficient material draws

#### Time Management:
- [X] Hard bound given by (Time / 30) + Increment
- [X] Soft bound given by (Time Per Move) / 3
- [ ] Best move stability


#### Engine Strength:
The Engine matches were run in fast-chess with a book from Fish-Test with positions of 80 cp or less.

| Version | ELO Estimate | CCRL Blitz ELO |
|---------|--------------|----------------|
| 1.4.0   | 2500         | 2749           |
| 1.3.0   | 2400         | 2407           |
| 1.1.0   | 1800         | ----           |

### Credits:
- Maksim Korzh for his incredible chess programming in c series which was invaluable while implementing bitboard move-generation and transposition table.
- Gediminas Masaitis for his Texel tuner without which the evaluation would be much weaker.
- Sebastian Lague for his chess videos and small-chessbot tournament which sparked my interest in chess programming.
- The Chess Programming Wiki: https://www.chessprogramming.org/Main_Page.
- [tissatussa](https://github.com/tissatussa) for testing compiles for Linux, using Xubuntu.

### Room For Improvement:
- Logical inaccuracies:
  - negamax and quiesce classify noisy moves with different criteria, quiet-promotions are scored with history
  - tt table and RFP use fail-hard while search is fail-soft
- Simplifications:
  - Make/Un-Make brought a negligible speed improvement over Copy/Make though the code is much more complex
  - Revert to a simpler move ordering though it loses ELO, to better keep track of which moves lead to pruning
- Search / Eval improvements:
  - Plenty of ELO is to be gained with TT bucketing, ageing and replacement schemes.
  - Better data generation could lead to better Texel-Tuning
  - A few search features in the road-map have not been implemented
  - Engine does not understand insufficient material draws
  - Possible small bugs within TT.
- Quality of Life improvements: 
  - nps is 0 when less than 1000 nodes searched
  - code coverage should be increased and core functions should 
    have asser statements in key branching points
  - uci tests should be written such that new releases work on multiple GUIs