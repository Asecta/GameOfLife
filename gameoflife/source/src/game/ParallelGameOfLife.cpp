#include <tbb/parallel_for.h>
#include "GameOfLife.h"

using namespace tbb;

ParallelGameOfLife::ParallelGameOfLife(int width, int height, int threadPoolSize) : BasicGameOfLife(width, height) {
    this->threadPoolSize = threadPoolSize;
    dataStride = getBoardSize() / threadPoolSize;
}

void ParallelGameOfLife::executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState) {
    // Run thread pool with threadPoolSize* pool size
    parallel_for(0, threadPoolSize, [&](size_t threadIndex) {
        // Read data offset by the thread index and data stride
        for (int index = threadIndex * dataStride; index < (threadIndex + 1) * dataStride; index++) {
            // Calculate new cell state and update newState
            newState[index] = updateCell(oldState, index) ? ALIVE : DEAD;
        }
    });
}