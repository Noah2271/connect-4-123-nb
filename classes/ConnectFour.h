#pragma once
#include "Game.h"

//
// connect-4 game implementation
//

//
// main game class
//
class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    void setUpBoard() override;
    Player* checkForWinner() override;
    bool checkForDraw() override;
    std::string stateString() override;
    std::string initialStateString() override;
    bool canBitMoveFrom(Bit& bit, BitHolder& from) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& from, BitHolder& to) override;
    bool actionForEmptyHolder(BitHolder &holer) override;
    void setStateString(const std::string &s) override;
    void stopGame() override;
    void updateAI() override;
    int getLowestEmptyRow(int);
    bool isColumnFull(int);
    void makeTurn(ChessSquare* topSquare, ChessSquare* targetSquare, int col, int targetRow, Bit* bit);
    Player* getOwner(ChessSquare* square);
    Grid* getGrid() override { return _grid; }
private:
    Bit * PieceForPlayer(const int playerNumber);
    Player* ownerAt(int index) const;
    
    Grid* _grid;
};