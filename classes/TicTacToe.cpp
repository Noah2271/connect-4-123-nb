#include "TicTacToe.h"
#include <vector>
#include <list>
using namespace std;

TicTacToe::TicTacToe()
{
    _grid = new Grid(3, 3);
}

TicTacToe::~TicTacToe()
{
    delete _grid;
}

//
// make an X or an O
//
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "o.png" : "x.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void TicTacToe::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        bit->setPosition(holder.getPosition());
        holder.setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}

bool TicTacToe::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index ) const
{
    auto square = _grid->getSquare(index % 3, index / 3);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* TicTacToe::checkForWinner()
{
    static const int kWinningTriples[8][3] =  { {0,1,2}, {3,4,5}, {6,7,8},  // rows
                                                {0,3,6}, {1,4,7}, {2,5,8},  // cols
                                                {0,4,8}, {2,4,6} };         // diagonals
    for( int i=0; i<8; i++ ) {
        const int *triple = kWinningTriples[i];
        Player *player = ownerAt(triple[0]);
        if( player && player == ownerAt(triple[1]) && player == ownerAt(triple[2]) )
            return player;
    }
    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString()
{
    std::string s = "000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 3 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y*3 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
        } else {
            square->setBit( nullptr );
        }
    });
}


//
// AI Board scoring function
// takes the vector of winningTiples and iterates through them, if a winning triple is detected returns
// score of -1 or +1, else 0
//
int aiBoardEval(const std::string& state) 
{
    static const list<vector<int>> winningTriples = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };    

    for (const auto& triple : winningTriples) {
        char player = state[triple[0]];
        if (player != '0' &&
            player == state[triple[1]] &&
            player == state[triple[2]])
        {
            return (state[triple[0]] == '2') ? AI_PLAYER : HUMAN_PLAYER;
        }
    }
    return 0;
}

//
// AI terminal test state detecting function
// checks if the board is evaluated to have a winner or if the board is full, else it is not terminal
//
bool aiTestForTerminal(const std::string& state) 
{
    if (aiBoardEval(state) != 0) return true;
    if (state.find('0') == string::npos) return true;
    return false;
}
// 
// negamax recursive function
// first checks if the current state is terminal, if it does it starts returns the score of the board
// minus the depth to push the AI to take the earliest win. Terminality is also the base case
// the main loop simulates a game tree/path utilizing negamax recursive calls, and filters out the best
// score based on the return value when the score is passed back up after recursion
//
int TicTacToe::negamax(std::string& state, int depth, int playerColor) 
{
    if (aiTestForTerminal(state)) {
        return (aiBoardEval(state) - depth);
    }

    int bestVal = -9999;

    for (int i = 0; i < 9; i++) {
        if (state[i] == '0') {
            state[i] = (playerColor == HUMAN_PLAYER) ? '1' : '2';
            int val = -negamax(state, depth - 1, -playerColor);
            bestVal = std::max(bestVal, val);
            state[i] = '0';
        }
    }
    return bestVal;
}
//
// update AI function
// for every possible position where the AI can move next, place the AI there and perform root call
// of negamax to simulate every possible state the rest of the game can take
// filter out the best value position returned by negamax recursive calls for each position AI can take
// and take that position
//
void TicTacToe::updateAI() 
{
    std::string state = stateString();
    int bestVal = -9999;
    ChessSquare* bestMove = nullptr;

    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 3 + x;

        if (state[index] == '0') {
            state[index] = '2';
            int newVal = -negamax(state, 9, HUMAN_PLAYER);
            state[index] = '0';

            if (newVal > bestVal) {
                bestVal = newVal;
                bestMove = square;
            }
        }
    });

    if (bestMove) {
        actionForEmptyHolder(*bestMove);
    }
}