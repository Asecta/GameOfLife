#include "GameOfLife.h"

BasicGameOfLife::BasicGameOfLife(int width, int height) : IGameOfLife(width, height) {
    this->gameState = vector<uint_fast8_t>(getBoardSize(), DEAD);
}

vector<uint_fast8_t> BasicGameOfLife::getCurrentState() {
    return this->gameState;
}

void BasicGameOfLife::setCell(int x, int y, bool state) {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) return;
    gameState[y * getHeight() + x] = state ? ALIVE : DEAD; // Calc index and write modifications
}


bool BasicGameOfLife::getCellByIndex(vector<uint_fast8_t> &board, int index) {
    if (index < 0 || index >= getBoardSize()) return false;
    return board[index] == ALIVE; // Read if cell is alive
}

int BasicGameOfLife::countAliveNeighbours(vector<uint_fast8_t> &gameState, int index) {
    int count = 0;
    // Using a hard coded method to get neighbours rather than iterating over them to increase performance
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

bool BasicGameOfLife::updateCell(vector<uint_fast8_t> &gameState, int index) {
    int aliveNeighbors = countAliveNeighbours(gameState, index);
    bool state = getCellByIndex(gameState, index);
    if (state && aliveNeighbors < 2) return false;
    if (state && aliveNeighbors > 3) return false;
    if (!state && aliveNeighbors == 3) return true;
    return state;
}

void BasicGameOfLife::nextState() {
    vector<uint_fast8_t> oldState = gameState;
    vector<uint_fast8_t> newState(getBoardSize(), DEAD);
    executeStep(oldState, newState);
    gameState = newState;
}