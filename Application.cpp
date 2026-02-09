#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "classes/ConnectFour.h"

namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = nullptr;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                ImGui::Begin("Settings");

                if (gameOver) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Winner: %d", gameWinner);
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                        if(game->_gameOptions.P1AI) {
                            game->_gameOptions.currentTurnNo = 1;
                            game->updateAI();
                        }   
                    }
                }
                if (!game) {
                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->setUpBoard();
                    }
                    if(ImGui::Button("Start Connect-Four [NO AI]")) {
                        game = new ConnectFour();
                        game->_gameOptions.AIPlaying = false;
                        game->setUpBoard();
                    }
                    if(ImGui::Button("Start Connect-Four [P1 AI]")) {
                        game = new ConnectFour();
                        game->_gameOptions.AIPlaying = true;
                        game->setUpBoard();
                        game->_gameOptions.currentTurnNo += 1;
                        game->_gameOptions.P1AI = true;
                        game->updateAI();
                    }
                    if(ImGui::Button("Start Connect-Four [P2 AI]")) {
                        game = new ConnectFour();
                        game->_gameOptions.AIPlaying = true;
                        game->setUpBoard();
                    }

                //
                // game information display
                // varies for different gamemodes
                //
                } else {
                    // player 1 AI display
                    if(game->_gameOptions.P1AI && game->gameHasAI()) ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber() == 0 ? 2 : 1);
                    // player 2 AI display
                    if(!game->_gameOptions.P1AI && game->gameHasAI()) ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber()+1);
                    // player vs player display
                    if(!game->gameHasAI()) ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber()+1);
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    if (game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    game->drawFrame();
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber()+1;
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}