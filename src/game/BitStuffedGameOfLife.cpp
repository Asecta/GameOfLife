#include "GameOfLife.h"

BitStuffedGameOfLife::BitStuffedGameOfLife(int width, int height) : IGameOfLife(width, height) {
    this->byteSize = sizeof(uint_fast8_t);
    this->gameState = vector<uint_fast8_t>(getBoardSize() / byteSize, 0);
}

vector<uint_fast8_t> BitStuffedGameOfLife::getCurrentState() {
    return this->gameState;
}

void BitStuffedGameOfLife::setCell(int x, int y, bool state) {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) return;
    int index = (y * getHeight() + x);
    int location = index / byteSize;
    int offset = index % byteSize;
    int value = gameState[location];
    value |= (state ? 1u : 0u) << offset;
    gameState[location] = value;
}


bool BitStuffedGameOfLife::getCellByIndex(vector<uint_fast8_t> &board, int index) {
    if (index < 0 || index >= getBoardSize()) return false;
    int byte = board[floor(index / byteSize)];
    int offset = index % byteSize;
    return (byte >> offset) & 1U == 1;
}

int BitStuffedGameOfLife::countAliveNeighbours(vector<uint_fast8_t> &gameState,
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

bool BitStuffedGameOfLife::updateCell(vector<uint_fast8_t> &gameState, int index) {
    int aliveNeighbors = countAliveNeighbours(gameState, index);
    bool state = getCellByIndex(gameState, index);
    if (state && aliveNeighbors < 2) return false;
    if (state && aliveNeighbors > 3) return false;
    if (!state && aliveNeighbors == 3) return true;
    return state;
}

void BitStuffedGameOfLife::nextState() {
    vector<uint_fast8_t> oldState = gameState;
    vector<uint_fast8_t> newState(getBoardSize() / byteSize, 0);
    executeStep(oldState, newState);
    gameState = newState;
}

void BitStuffedGameOfLife::executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState) {
    for (int location = 0; location < getBoardSize() / byteSize; location++) {
        int dataByte = gameState[location];

        for (int offset = 0; offset < byteSize; offset++) {
            dataByte |= (updateCell(oldState, location) ? 1u : 0u) << offset;
        }
        gameState[location] = dataByte;
    }
}