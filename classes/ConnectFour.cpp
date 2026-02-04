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
ConnectFour::ConnectFour()
{
        _grid = new Grid(7,6);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

//
// probably going to have to make a new function for setStateString
//
Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(redPiece ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(redPiece ? 1 : 0));
    redPiece = !redPiece;
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
    for(int row = 0; row < ROWS; ++row){
        for(int col = 0; col <= COLUMNS; ++col){
            Player* owner = getOwner(_grid->getSquare(col,row));
            if(owner != nullptr && 
                owner == getOwner(_grid->getSquare(col+1, row)) &&
                owner == getOwner(_grid->getSquare(col+2, row)) &&
                owner == getOwner(_grid->getSquare(col+3, row))) {
                    return owner;
            }
            if(owner != nullptr && 
                owner == getOwner(_grid->getSquare(col, row+1)) &&
                owner == getOwner(_grid->getSquare(col, row+2)) &&
                owner == getOwner(_grid->getSquare(col, row+3))) {
                    return owner;
            }
            if(owner != nullptr && 
                owner == getOwner(_grid->getSquare(col+1, row+1)) &&
                owner == getOwner(_grid->getSquare(col+2, row+2)) &&
                owner == getOwner(_grid->getSquare(col+3, row+3))) {
                    return owner;
            }
            if(owner != nullptr && 
                owner == getOwner(_grid->getSquare(col+1, row-1)) &&
                owner == getOwner(_grid->getSquare(col+2, row-2)) &&
                owner == getOwner(_grid->getSquare(col+3, row-3))) {
                    return owner;
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
    bool isDraw = true;
    if (checkForWinner() != nullptr) return false;
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

//
// state string functions
// 0 = empty | 1 = red | 2 = yellow
//
string ConnectFour::stateString()
{
    string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

void ConnectFour::setStateString(string const&)
{
    return;
}

string ConnectFour::initialStateString()
{
    return "000000000000000000000000000000000000000000";
}

bool ConnectFour::canBitMoveFrom(Bit& bit, BitHolder& from){
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit& bit, BitHolder& from, BitHolder& to){
    return false;
}

void ConnectFour::stopGame(){
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
    return;
}

//
// random AI function
//
void ConnectFour::updateAI(){
    static bool waiting = false;
    static std::chrono::steady_clock::time_point waitTime;
    if(checkForWinner() != nullptr || checkForDraw()) return;

    if(!waiting) {
        waiting = true;
        waitTime = std::chrono::steady_clock::now() + std::chrono::seconds(1);
        return;
    }
    if(std::chrono::steady_clock::now() < waitTime) return;
        int randomCol = rand() % 7;
        while((isColumnFull(randomCol))){
            randomCol = rand() % 7;
        }
        int targetRow = getLowestEmptyRow(randomCol);
        Bit *AIBit = PieceForPlayer(AI_PLAYER);
        ChessSquare* topSquare = _grid->getSquare(randomCol, 0);
        ChessSquare* targetSquare = _grid->getSquare(randomCol, targetRow);
        makeTurn(topSquare, targetSquare, randomCol, targetRow, AIBit);
        waiting = false;
        return;
    }
