#include <vector>

#include <tbb/tbb.h>

#include "GameOfLifeInstance.h"

#pragma once

using namespace std;

IGameOfLifeInstance::IGameOfLifeInstance(int width, int height) {
    this->width = width;
    this->height = height;
    this->boardSize = width * height;
    this->gameState = vector<int>(width * height, WHITE);
}

vector<int> IGameOfLifeInstance::getCurrentState() {
    return this->gameState;
}

void IGameOfLifeInstance::setCell(int x, int y, bool state) {
    if (0 < x || x >= width || 0 < y || y >= height)return;
    gameState[y * height + x] = state ? BLACK : WHITE;
}

void IGameOfLifeInstance::nextState() {
    vector<int> oldState = gameState;
    vector<int> newState(boardSize, WHITE);
    executeStep(oldState, newState);
    gameState = newState;
}

int IGameOfLifeInstance::getBoardSize() { return boardSize; }

int IGameOfLifeInstance::getWidth() { return width; }

int IGameOfLifeInstance::getHeight() { return height; }

bool IGameOfLifeInstance::getCellByIndex(vector<int> &board, int index) {
    if (index < 0 || index >= boardSize)
        return false;
    return board[index] == BLACK;
}

int IGameOfLifeInstance::countAliveNeighbours(vector<int> &gameState,
                                              int index) {
    int count = 0;
    count += getCellByIndex(gameState, index - width - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index - width) ? 1 : 0;
    count += getCellByIndex(gameState, index - width + 1) ? 1 : 0;
    count += getCellByIndex(gameState, index - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + width - 1) ? 1 : 0;
    count += getCellByIndex(gameState, index + width) ? 1 : 0;
    count += getCellByIndex(gameState, index + width + 1) ? 1 : 0;
    return count;
}

bool IGameOfLifeInstance::updateCell(vector<int> &gameState, int index) {
    int aliveNeighbors = countAliveNeighbours(gameState, index);
    bool state = getCellByIndex(gameState, index);
    if (state && aliveNeighbors < 2)
        return false;
    if (state && aliveNeighbors > 3)
        return false;
    if (!state && aliveNeighbors == 3)
        return true;
    return state;
}

void IGameOfLifeInstance::executeStep(vector<int> &oldState, vector<int> &newState) {

}

SerialGameOfLife::SerialGameOfLife(int width, int height)
        : IGameOfLifeInstance(width, height) {}

void SerialGameOfLife::executeStep(vector<int> &oldState,
                                   vector<int> &newState) {
    for (int index = 0; index < getBoardSize(); index++) {
        newState[index] = updateCell(oldState, index) ? BLACK : WHITE;
    }
}

ParallelGameOfLife::ParallelGameOfLife(int width, int height, int threadCount)
        : IGameOfLifeInstance(width, height) {
    this->threadCount = threadCount;
    threadStride = getBoardSize() / threadCount;
}

void ParallelGameOfLife::executeStep(vector<int> &oldState, vector<int> &newState) {
    tbb::parallel_for(0, threadCount, [&](size_t threadIndex) {
        for (int index = threadIndex * threadStride;
             index < (threadIndex + 1) * threadStride; index++) {
            newState[index] = updateCell(oldState, index) ? BLACK : WHITE;
        }
    });
}