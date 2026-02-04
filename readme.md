Noah Billedo CMPM 123 - Windows PC

## Connect Four Implementation
#### ConnectFour.h
- Holds the class for ConnectFour, which inherits/overrides most of the base game class functions with the addition of the following helper functions. Their names document themself.
    int getLowestEmptyRow(int);
    bool isColumnFull(int);
    void makeTurn(ChessSquare* topSquare, ChessSquare* targetSquare, int col, int targetRow, Bit* bit);


#### ConnectFour.cpp
- Contains the implementation of the functions defined in ConnectFour.h. Currently includes implementation of a random AI that generates a random non-full column number before dropping a piece down using the makeTurn() function. Save state yet to be implemented.

#### Application.cpp
- Created a button to open the connect four game, as well as two additional buttons to choose whether the AI is playing as P1 or P2. P1 is implemented in by incrementing the turn number in _gameOptions by one, since it pushes up the AI to move first. ConnectFour.cpp also determines red or yellow pieces
using a flip-flopping boolean value rather than player number so the red piece is always player one.