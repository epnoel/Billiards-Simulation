#pragma once

#include "GameState.h"

class GameStateStack {

public:
    GameStateStack();

    GameStateStack(int maxLimit);

    void addGameState(GameState& gameState);

    void undo();

    void redo();

    void printMostRecentGameState();

    GameState loadGameState();

public:
    int maxLimit;
    int currentGameState = -1;
    std::vector<GameState> allGameStates;
};