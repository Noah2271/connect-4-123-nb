Noah Billedo CMPM 123 - Windows PC

## Connect Four Implementation
#### ConnectFour.h
- Holds the class for ConnectFour, which inherits/overrides most of the base game class functions with the addition of the following helper functions. Their names document themself.
    int getLowestEmptyRow()
    bool isColumnFull()
    void makeTurn()
    isColumnFullFromState()
    getLowestEmptyRowFromState()
    calculateScore() <-- template


#### ConnectFour.cpp
- Contains the implementation of the functions defined in ConnectFour.h. Connect four grid is represented by a grid of chessSquares with index [0][0] at the top left and [6][5] at the bottom right. Players are able to make moves in this implementation by clicking on any empty space is a non-full column; where the piece will start at the top square of the column and move to its resting place at the targeted square for the move. Wins are tracked by scanning the entire board and for each piece checking for four consecutive pieces in any horizontal, vertical, or diagonal direction.
- Negamax AI with alpha-beta pruning and search depth of six is also implemented. The following helper functions were created alongside the function from TicTacToe for its implementation: isColumnFullFromState(), getLowestEmptyRowFromState(), and calculateScore(). The first two work similarly to isColumnFull() and getLowestEmptyRow(), but instead of utilizing the _grid it indices and checks the state string. calculateScore() is used in the evalFunction where for each piece within a buffer from the edge of the board, sequences of 4 pieces are scanned and scored based on the order of the pieces. This score taken from calculateScore() is returned to aiBoardEval() which returns the score to be passed to the negamax call.

#### Application.cpp
- Created a button to open the connect four game. How the player first AI is implemented has changed since last commit to just calling updateAI() for the first time on game reset and game initialization, and setting the turn to the first turn beforehand. AI will also always be the yellow player no matter what turn they move on. No implementation for saving/loading with the state string yet.