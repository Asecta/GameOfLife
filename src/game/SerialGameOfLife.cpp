#include "GameOfLife.h"

SerialGameOfLife::SerialGameOfLife(int width, int height)
        : BasicGameOfLife(width, height) {
}

void SerialGameOfLife::executeStep(vector<int> &oldState, vector<int> &newState) {
    for (int index = 0; index < getBoardSize(); index++) {
        newState[index] = updateCell(oldState, index) ? BLACK : WHITE;
    }
}