#include "GameOfLife.h"

BitStuffedGameOfLife::BitStuffedGameOfLife(int width, int height) : IGameOfLife(width, height) {
    this->byteSize = sizeof(uint_fast8_t); // Get the size of the byte
    this->gameState = vector<uint_fast8_t>(getBoardSize() / byteSize, DEAD); // Setup blank board
}

vector<uint_fast8_t> BitStuffedGameOfLife::getCurrentState() {
    return this->gameState;
}

void BitStuffedGameOfLife::setCell(int x, int y, bool state) {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) return;
    int index = (y * getHeight() + x); // Calculate index
    int location = index / byteSize;   // Calculate byte location
    int offset = index % byteSize;     // Calculate data offset in byte
    int value = gameState[location];   // Get data bit
    value |= (state ? 1u : 0u) << offset; // modify the bit at given offset
    gameState[location] = value;       // Write new byte to array
}


bool BitStuffedGameOfLife::getCellByIndex(vector<uint_fast8_t> &board, int index) {
    if (index < 0 || index >= getBoardSize()) return false; // Sanity check
    int byte = board[floor(index / byteSize)]; // get byte containing data bit
    int offset = index % byteSize; // get offset of data bit
    return (byte >> offset) & 1U == 1; // read data bit
}

int BitStuffedGameOfLife::countAliveNeighbours(vector<uint_fast8_t> &gameState,
                                               int index) {
    // Using a hard coded method to get neighbours rather than iterating over them to increase performance
    int count = 0;
    count += getCellByIndex(gameState, index - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + 1) ? 1 : 0;
    count += getCellByIndex(gameState, index - getWidth() - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index - getWidth()) ? 1 : 0;
    count += getCellByIndex(gameState, index - getWidth() + 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + getWidth() - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + getWidth()) ? 1 : 0;
    count += getCellByIndex(gameState, index + getWidth() + 1) ? 1 : 0;
    return count;
}

bool BitStuffedGameOfLife::updateCell(vector<uint_fast8_t> &gameState, int index) {
    int aliveNeighbors = countAliveNeighbours(gameState, index); // count cell neighbours
    bool state = getCellByIndex(gameState, index); // get current cell state
    if (state && aliveNeighbors < 2) return false; // Game of life stub
    if (state && aliveNeighbors > 3) return false;
    if (!state && aliveNeighbors == 3) return true;
    return state;
}

void BitStuffedGameOfLife::nextState() {
    // verbose for the sake of readability
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