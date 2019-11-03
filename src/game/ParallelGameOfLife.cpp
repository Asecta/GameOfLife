#include <tbb/parallel_for.h>
#include "GameOfLife.h"

ParallelGameOfLife::ParallelGameOfLife(int width, int height, int threadCount)
        : BasicGameOfLife(width, height) {
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