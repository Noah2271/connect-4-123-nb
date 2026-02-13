#include "ConnectFour.h"
#include <vector>
#include <list>
#include <chrono>
#include <thread>
using namespace std;

//
// constant game state variables
//
const int ROWS = 6;
const int COLUMNS = 7;
bool redPiece = false;
bool gameOver = false;


ConnectFour::ConnectFour()
{
        _grid = new Grid(7,6);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

//
// board Setup
// 
void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = ROWS;
    _gameOptions.rowY = COLUMNS;
    _grid->initializeSquares(80, "square.png");
    if(gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }
    startGame();
}

//
// player action function
// checks if the clicked holder is a chessSquare and if the requested move lies
// within the columns and rows of the board. If it does, grabs the top of the column,
// finds the target square, and creates a bit for the player before calling makeTurn.
// 
bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    ChessSquare* clickedSquare = dynamic_cast<ChessSquare*>(&holder);
    if (!clickedSquare) return false;

    int col = clickedSquare->getColumn();
    if (col < 0 || col >= 7) return false;

    if (isColumnFull(col)) return false;

    int targetRow = getLowestEmptyRow(col);
    if (targetRow == -1) return false;

    ChessSquare* topSquare = _grid->getSquare(col, 0);

    ChessSquare* targetSquare = _grid->getSquare(col, targetRow);
    if (!targetSquare) return false;

    Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (!bit) return false;

    makeTurn(topSquare, targetSquare, col, targetRow, bit);
    return true;
}

//
// player turn logic function
// takes the data from actionForEmptyHolder() and sets the bit at the top square, before moving it down to the
// target location. At the end it actually sets the bit to the position it should be resulting from the move.
//
void ConnectFour::makeTurn(ChessSquare* topSquare, ChessSquare* targetSquare, int col, int targetRow, Bit* bit){
    if (targetRow > 0 && topSquare) {
        bit->setPosition(topSquare->getPosition());
        bit->moveTo(targetSquare->getPosition());
    } else {
        bit->setPosition(targetSquare->getPosition());
    }
    targetSquare->setBit(bit);
    endTurn();
}

//
// helper function to get lowest empty row
// iterates down the column and returns the row index of the
// first empty square in the column
//
int ConnectFour::getLowestEmptyRow(int col)
{
    for(int row = ROWS - 1; row >= 0; --row){
        ChessSquare* square = _grid->getSquare(col, row);
        if(square->bit() == nullptr){
            return row;
        }
    }
    return -1;
}

//
// helper function check if the column is full
// iterates through the entire column and returns false if any instance
// where there is no bit on a square
//
bool ConnectFour::isColumnFull(int col)
{
    for(int row = ROWS -1; row >= 0; --row){
        ChessSquare* square = _grid->getSquare(col, row);
        if(square->bit() == nullptr){
            return false;
        }
    }
    return true;
}

//
// helper function returns the owner of a bit in a given square
//
Player* ConnectFour::getOwner(ChessSquare* square)
{
    if (!square || !square->bit()) return nullptr;
    return square->bit()->getOwner();
}

// 
// function to check for win, returns the winner by iterating through the entire board
// and for each piece checking diagonally, vertically, and horizontally for a chain of 4
// returns nullptr if no winner
//
Player* ConnectFour::checkForWinner()
{
    if(!gameOver){
        for(int row = 0; row < ROWS; ++row){
            for(int col = 0; col < COLUMNS; ++col){
                Player* owner = getOwner(_grid->getSquare(col,row));
                if(owner != nullptr && 
                    owner == getOwner(_grid->getSquare(col+1, row)) &&
                    owner == getOwner(_grid->getSquare(col+2, row)) &&
                    owner == getOwner(_grid->getSquare(col+3, row))) {
                        gameOver = true;
                        return owner;
                }
                if(owner != nullptr && 
                    owner == getOwner(_grid->getSquare(col, row+1)) &&
                    owner == getOwner(_grid->getSquare(col, row+2)) &&
                    owner == getOwner(_grid->getSquare(col, row+3))) {
                        gameOver = true;
                        return owner;
                }
                if(owner != nullptr && 
                    owner == getOwner(_grid->getSquare(col+1, row+1)) &&
                    owner == getOwner(_grid->getSquare(col+2, row+2)) &&
                    owner == getOwner(_grid->getSquare(col+3, row+3))) {
                        gameOver = true;
                        return owner;
                }
                if(owner != nullptr && 
                    owner == getOwner(_grid->getSquare(col+1, row-1)) &&
                    owner == getOwner(_grid->getSquare(col+2, row-2)) &&
                    owner == getOwner(_grid->getSquare(col+3, row-3))) {
                        gameOver = true;
                        return owner;
                }
            }
        }
    }
    return nullptr;
}

//
// draw check function
// iterates through the entire board if there is no winner. If a empty square is found, returns false
// since the board isn't full yet, so there can't be a draw
//
bool ConnectFour::checkForDraw() 
{
    if(!checkForWinner()){
        bool isDraw = true;
            if (checkForWinner() != nullptr) return false;
            _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
                if (!square->bit()) {
                    isDraw = false;
                }
            });
        return isDraw;
    }
    return false;
}

//
// stop game function
// goes through the grid and destroys all the bits
//
void ConnectFour::stopGame(){
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
    gameOver = false;
    return;
}

//
// state string functions
// 0 = empty | 1 = red | 2 = yellow
// the AI player will always be yellow for my sanity
//
string ConnectFour::stateString()
{
    string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

//
// game load function for saves
// not implemented
//
void ConnectFour::setStateString(string const&)
{
    return;
}

//
// empty state string
//
string ConnectFour::initialStateString()
{
    return "000000000000000000000000000000000000000000";
}

//
// bit move functions for chess/checkers. Not used here.
//
bool ConnectFour::canBitMoveFrom(Bit& bit, BitHolder& from){
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit& bit, BitHolder& from, BitHolder& to){
    return false;
}

//
// negamax + alpha beta pruning functions
//
// column full check function for state strings
// for arg column, iterates down the the entire column and checks for empty spaces
//
bool ConnectFour::isColumnFullFromState(string& state, int col){ 
    for(int row = 0; row < ROWS; row++){
        int index = row * COLUMNS + col;
        if(state[index] == '0')
            return false;
    }
    return true;
}

//
// getter for lowest empty row for a argument column
// iterates from the bottom of the column because if iterating from the top,
// you'll get the highest empty row
//
int ConnectFour::getLowestEmptyRowFromState(string& state, int col){
    for(int row = ROWS -1; row >= 0; --row){
        int index = row * COLUMNS + col;
        if(state[index] == '0'){
            return row;
        }
    }
    return -1;
}

//
// updateAI function
// utilizes chrono to make the AI wait a turn before taking its move
// iterates through each lowest empty row position and starts recursively calling negamax to simulate game
// filters out the best score based on the game stemming from that move and takes it
//
void ConnectFour::updateAI(){
    static bool AIWaiting = false;
    static chrono::steady_clock::time_point waitClock;

    if(!AIWaiting) {
        AIWaiting = true;
        waitClock = chrono::steady_clock::now() + chrono::seconds(1);
        return;
    }

    if(chrono::steady_clock::now() < waitClock) return;

    string currentState = stateString();
    int bestVal = -9999999;
    int bestCol = -1;

    for(int i = 0; i < COLUMNS; i++){
        if (!isColumnFullFromState(currentState, i)){
            int legalRow = getLowestEmptyRowFromState(currentState, i);
            int index = legalRow * COLUMNS + i;
            currentState[index] = '2';

            // if win condition for current move detected take it instantly, no need to call negamax
            if(aiBoardEval(currentState) >= 999999) {
                bestCol = i;
                currentState[index] = '0';
                break;
            }
            // depth 6 for efficiency, anything higher will noticeably exceed the 1 second timer already in place
            int newVal = -negamax(currentState, 6, -99999, 99999, HUMAN_PLAYER);
            currentState[index] = '0';

            if (newVal >= bestVal){
                bestVal= newVal;
                bestCol = i;
            }
        }
    }
    
    if(bestCol != -1){
        int targetRow = getLowestEmptyRow(bestCol);
        Bit *AIBit = PieceForPlayer(AI_PLAYER);
        ChessSquare* topSquare = _grid->getSquare(bestCol, 0);
        ChessSquare* targetSquare = _grid->getSquare(bestCol, targetRow);
        makeTurn(topSquare, targetSquare, bestCol, targetRow, AIBit);
    }
    AIWaiting = false;
    return;
}

//
// board scoring function
// scores the current game state with a preference for playing the middle
// scores by counting consecutive pieces horizontally, vertically, and diagonally
// the more consecutive AI pieces (up to 4 for terminal), the better the score for AI.
// if consecutive player pieces, accrues bad score
//
int ConnectFour::aiBoardEval(string& state) {
    int score = 0;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            // calculateScore() is implemented in ConnectFour.h as a template function for efficiency
            if (col + 3 < COLUMNS) {
                // passed in function checks for horizontal consecutives
                int val = calculateScore(row, col, state, [&](int i) { return state[row * COLUMNS + (col + i)]; });
                if (val == 999999 || val == -999999) return val;
                score += val;
            }

            if (row + 3 < ROWS) {
                // passed in function checks for vertical consecutives
                int val = calculateScore(row, col, state, [&](int i) { return state[(row + i) * COLUMNS + col]; });
                if (val == 999999 || val == -999999) return val;
                score += val;
            }

            if (row + 3 < ROWS && col + 3 < COLUMNS) {
                // passed in function checks for diagonal consecutives (left-to-right) down
                int val = calculateScore(row, col, state, [&](int i) { return state[(row + i) * COLUMNS + (col + i)]; });
                if (val == 999999 || val == -999999) return val;
                score += val;
            }

            if (row + 3 < ROWS && col - 3 >= 0) {
                // passed in function checks for diagonal consecutives (right-to-left) down
                int val = calculateScore(row, col, state, [&](int i) { return state[(row + i) * COLUMNS + (col - i)]; });
                if (val == 999999 || val == -999999) return val;
                score += val;
            }
        }
    }

    return score;
}

// 
// check for terminal function
// if gameboard full or aiBoardEval returns some sort of consecutive four, the game is terminal
//
bool ConnectFour::aiTestForTerminal(string& state){
    return(state.find('0') == string::npos || aiBoardEval(state) >= 999999 ||  aiBoardEval(state) <= -999999);
}

//
// negamax function
// same process as updateAI() for move simulation but flips the playerColor and has alpha beta pruning to filter out irrelevant moves
//
int ConnectFour::negamax(string& state, int depth, int alpha, int beta, int playerColor){
    if (aiTestForTerminal(state) || depth == 0) return aiBoardEval(state) * playerColor;

    int bestVal = -9999999;

    for(int i = 0; i < COLUMNS; i++){
        if(!isColumnFullFromState(state, i)){
            int legalRow = getLowestEmptyRowFromState(state, i);
            int index = legalRow * COLUMNS + i;
            state[index] = (playerColor == AI_PLAYER) ? '2' : '1';
            int val = -negamax(state, depth -1, -beta, -alpha, -playerColor);
            state[index] = '0';               
            bestVal = max(bestVal, val);
            // stores best seen score for current player of the negamax call
            alpha = max(alpha, val);
            // if max score of minimizing player less than minimum score of maximizing player, prune
            if (alpha >= beta) break;
        }
    }
    return bestVal;
}
