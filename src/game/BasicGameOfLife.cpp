#include "GameOfLife.h"

BasicGameOfLife::BasicGameOfLife(int width, int height) : IGameOfLife(width, height) {
    this->gameState = vector<int>(getBoardSize(), WHITE);
}

vector<int> BasicGameOfLife::getCurrentState() {
    return this->gameState;
}

void BasicGameOfLife::setCell(int x, int y, bool state) {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) return;
    gameState[y * getHeight() + x] = state ? BLACK : WHITE;
}


bool BasicGameOfLife::getCellByIndex(vector<int> &board, int index) {
    if (index < 0 || index >= getBoardSize())
        return false;
    return board[index] == BLACK;
}

int BasicGameOfLife::countAliveNeighbours(vector<int> &gameState,
                                          int index) {
    int count = 0;
    count += getCellByIndex(gameState, index - getWidth() - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index - getWidth()) ? 1 : 0;
    count += getCellByIndex(gameState, index - getWidth() + 1) ? 1 : 0;
    count += getCellByIndex(gameState, index - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + getWidth() - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + getWidth()) ? 1 : 0;
    count += getCellByIndex(gameState, index + getWidth() + 1) ? 1 : 0;
    return count;
}

bool BasicGameOfLife::updateCell(vector<int> &gameState, int index) {
    int aliveNeighbors = countAliveNeighbours(gameState, index);
    bool state = getCellByIndex(gameState, index);
    if (state && aliveNeighbors < 2) return false;
    if (state && aliveNeighbors > 3) return false;
    if (!state && aliveNeighbors == 3) return true;
    return state;
}

void BasicGameOfLife::nextState() {
    vector<int> oldState = gameState;
    vector<int> newState(getBoardSize(), WHITE);
    executeStep(oldState, newState);
    gameState = newState;
}