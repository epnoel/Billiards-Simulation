#include "GameStateStack.h"

GameStateStack::GameStateStack() {
    maxLimit = 0;
}

GameStateStack::GameStateStack(int maxLimit) {
    this->maxLimit = maxLimit;
    //allGameStates.resize(maxLimit);
}

void GameStateStack::addGameState(GameState& gameState) {
    if (currentGameState >= maxLimit) {
        allGameStates.push_back(gameState);
        allGameStates.erase(allGameStates.begin());
    }
    else {
        currentGameState++;
        allGameStates.push_back(gameState);
    }

    allGameStates[currentGameState] = gameState;
}

void GameStateStack::undo() {
    if (currentGameState > 0) {
        currentGameState--;
    }
    else {
        std::cout << "Can't Undo anymore!" << std::endl;
    }
}

void GameStateStack::redo() {
    if (currentGameState < allGameStates.size()-1) {
        currentGameState++;
    }
    else {
        std::cout << "Can't Redo anymore!" << std::endl;
    }
}

void GameStateStack::printMostRecentGameState()
{
    allGameStates.back().printGameState();
    std::cout << std::endl;
}

GameState GameStateStack::loadGameState() {
    return allGameStates[currentGameState];
}