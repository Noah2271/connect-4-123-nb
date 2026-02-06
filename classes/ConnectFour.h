#pragma once
#include "Game.h"
#include <functional>

//
// main game class
//
class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    Player*     checkForWinner() override;
    Player*     getOwner(ChessSquare* square);
    Grid*       getGrid() override { return _grid; }
    std::string stateString() override;
    std::string initialStateString() override;
    int         getLowestEmptyRow(int);
    bool        canBitMoveFrom(Bit& bit, BitHolder& from) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& from, BitHolder& to) override;
    bool        actionForEmptyHolder(BitHolder &holer) override;
    bool        isColumnFull(int);
    bool        checkForDraw() override;
    void        setStateString(const std::string &s) override;
    void        stopGame() override;
    void        updateAI() override;
    void        makeTurn(ChessSquare* topSquare, ChessSquare* targetSquare, int col, int targetRow, Bit* bit);
    void        setUpBoard() override;

    // calculateScore getter function 
    template <typename Getter>
    int calculateScore(int row, int col, const std::string& state, Getter getPiece)
    {
        int aiPieces = 0, humanPieces = 0, empty = 0;

        for (int i = 0; i < 4; i++) {
            char p = getPiece(i);
            if (p == '2') aiPieces++;
            else if (p == '1') humanPieces++;
            else empty++;
        }

        if (aiPieces == 4) return 999999;
        if (humanPieces == 4) return -999999;

        int score = 0;
        if (aiPieces == 3 && empty == 1) score += 100;
        else if (aiPieces == 2 && empty == 2) score += 50;

        if (humanPieces == 3 && empty == 1) score -= 75;
        else if (humanPieces == 2 && empty == 2) score -= 25;

        return score;
    }

private:
    Grid*       _grid;
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index) const;
    int         negamax(std::string& state, int depth, int alpha, int beta, int playerColor);
    int         aiBoardEval(std::string& state);
    int         getLowestEmptyRowFromState(std::string& state, int col);
    bool        aiTestForTerminal(std::string& state);
    bool        isColumnFullFromState(std::string& state, int col);
};