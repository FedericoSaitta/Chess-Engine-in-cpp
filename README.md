# Chess engine in c++

To play against this engine head to: https://lichess.org/@/Aramis-Bot.

This project has been largely inspired by my previously built python chess engine which I estimate to play around 1400 - 1500 ELO.
Because of the performance improvement brought by the C++ compiler, this engine has already surpassed its predecessor with 
version 1 achieving a rating of 2050 ELO in bullet. Please contact me for any advice, critique or question about the bot :) 

If you are also thinking on embarking on the journey of building a chess engine
I'd advise to take is slowly and enjoy watching a stupid machine learn to play some decent moves, furthermore I advise to start
by implementing simple algorithms at first, and only once speed matters, to change them. Move generation speed might seem like 
a deciding factor of a chess engine's strength, but it only accounts for around 5 - 10% of the time spent by the program, 
good move ordering and aggressive pruning techniques with a good evaluation matter a lot more for reaching higher plies.

## Technical Details:

#### Move Generation:
- Plain Magic Bitboards for move generation of all sliding and leaping pieces.
- Pawn pushes, En-Passant and Castling move generation is done on the fly.
- As opposed to updating a board with a Make-UnMake move approach, a Copy-Restore board approach has been 
implemented. This is a simpler though slower approach due to the creation and destruction of bitboard objects.

#### Board Evaluation:
- Uses simple scores for each piece on the board
- Uses simple piece square tables

#### Searching Algorithm:
- Move ordering by: PV move, MVV-LVA, Killer 1, Killer 2, History moves, remaining moves.
- Negamax with quiescence search algorithm capped at 64 plies (32 moves)
- Iterative deepening with Principal Variation Search principle with a zero-width window

#### Time Management:
- simple time control that decides allowed thinking time based on how much of the total game time has been exhausted
- search is only stopped once the current depth has been searched fully

#### Performance BenchMarks:
- These test have been run on 1,6 GHz Dual-Core Intel Core i5 (Macbook Air 2017)

##### Perft Test:

Depth: 5 Nodes: 4'865'609 Time: 0.164552s MNodes/s: 29.5688 FEN: start position
Depth: 5 Nodes: 193'690'690 Time: 6.06453s MNodes/s: 31.9383 FEN: kiwipete
Depth: 5 Nodes: 164'075'551 Time: 5.37829s MNodes/s: 30.507 FEN: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10

##### Node Pruning Test:

This test consists of searching for a mate in 5 (10 plies), FEN: "N1bk3r/P5pp/3b1p2/3B4/R2nP1nq/3P3N/1BP3KP/4Q2R b - - 0 1"
Here is the UCI output of the iterative deepening search, we know the mate has been successfully found as the
evaluation shoots up to (50'000 - 9) = 48991.

info score cp -620 depth 1 nodes 63 nps 1'863'574 pv h4e1  
info score cp -620 depth 2 nodes 336 nps 1'767'211 pv h4e1  h1e1  
info score cp -620 depth 3 nodes 2'298 nps 5'086'017 pv h4e1  h1e1  d4c2  
info score cp -615 depth 4 nodes 10'869 nps 1'655'877 pv h4e1  h1e1  d4c2  e1c1  
info score cp -600 depth 5 nodes 68'036 nps 5'800'927 pv h4e1  h1e1  d4c2  e1c1  c2e3  
info score cp -415 depth 6 nodes 293'208 nps 1'552'472 pv h4e1  h1e1  d4c2  e1g1  g4e3  g2h1  
info score cp -415 depth 7 nodes 1'590'135 nps 4'861'336 pv h4e1  h1e1  d4c2  e1g1  g4e3  g2h1  e3d5  
info score cp -225 depth 8 nodes 6'215'900 nps 1'572'247 pv h4h3  g2h3  g4e3  d5e6  c8e6  h3h4  d4f3  h4h5  
info score cp -225 depth 9 nodes 28'527'536 nps 5'212'807 pv h4h3  g2h3  g4e3  d5e6  c8e6  h3h4  d4f3  h4h5  f3e1  
info score cp 48991 depth 10 nodes 98'297'385 nps 1'607'238 pv h4h3  g2h3  g4e3  d5e6  c8e6  h3h4  d4f3  h4h5  e6g4  
bestmove h4h3

Mating Move: h4h3 Time taken: 71.1204ss

#### Engine vs. Engine Match Results: 

V1 vs. V2:
--------------------------------------------------
Results of ChessEngine_V1 vs ChessEngine (10+1, NULL, NULL, openings.epd):
Elo: -260.49 +/- 43.07, nElo: -418.02 +/- 48.15
LOS: 0.00 %, DrawRatio: 15.00 %, PairsRatio: 0.01
Games: 200, Wins: 4, Losses: 131, Draws: 65, Points: 36.5 (18.25 %)
Ptnml(0-2): [44, 40, 15, 1, 0]
--------------------------------------------------

V2.1 vs. V2: 
--------------------------------------------------
Results of ChessEngine vs ChessEngine_Bugless (10+1, NULL, NULL, scrapedOpenings.epd):
Elo: 277.00 +/- 40.90, nElo: 345.17 +/- 34.05
LOS: 100.00 %, DrawRatio: 17.00 %, PairsRatio: 22.71
Games: 400, Wins: 307, Losses: 42, Draws: 51, Points: 332.5 (83.12 %)
Ptnml(0-2): [4, 3, 34, 42, 117]
--------------------------------------------------


#### Future Improvements:
- Symmetry evaluation test suite
- More complex time management for longer time controls
- Opening book
- Evaluation of king safety and mobility
- Evaluation of pawn structure and passed pawns

#### Credits: 
This engine has been largely inspired by the work of many successful programmers such as Sebastian Lague, Code_Monkey_King and 
Bluefever Software who all have amazing videos on chess engine programming on youtube. Lastly, this would not have been 
possible without the resources provided by the Chess Programming Wiki: https://www.chessprogramming.org/Main_Page. 
