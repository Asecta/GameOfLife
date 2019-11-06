#include "GameOfLife.h"

// Default constructor
SerialGameOfLife::SerialGameOfLife(int width, int height) : BasicGameOfLife(width, height) {}

void SerialGameOfLife::executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState) {
    // Iterate over game sequentially and write updated cells to new game state
    for (int index = 0; index < getBoardSize(); index++) {
        newState[index] = updateCell(oldState, index) ? ALIVE : DEAD;
    }
}