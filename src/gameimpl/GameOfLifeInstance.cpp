#include <vector>
#include <tbb/tbb.h>

using namespace std;


class IGameOfLifeInstance {

private:
    int width;
    int height;
    int boardSize;
    vector<int> gameState;

public:
    IGameOfLifeInstance(int width, int height) {
        this->width = width;
        this->height = height;
        this->boardSize = width * height;
        this->gameState = vector<int>(width * height, WHITE);
    }

    vector<int> getCurrentState() {
        return gameState;
    }

    void setCell(int x, int y, bool state) {
        if (0 < x || x >= width || 0 < y || y >= height) return;
        gameState[y * height + x] = state ? BLACK : WHITE;
    }

    void nextState() {
        vector<int> oldState = gameState;
        vector<int> newState(boardSize, WHITE);
        executeStep(oldState, newState);
        gameState = newState;
    }

    int getBoardSize() { return boardSize; }

    int getWidth() { return width; }

    int getHeight() { return height; }

protected:
    bool getCellByIndex(vector<int> &board, int index) {
        if (index < 0 || index >= boardSize) return false;
        return board[index] == BLACK;
    }

    int countAliveNeighbours(vector<int> &gameState, int index) {
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

    bool updateCell(vector<int> &gameState, int index) {
        int aliveNeighbors = countAliveNeighbours(gameState, index);
        bool state = getCellByIndex(gameState, index);
        if (state && aliveNeighbors < 2) return false;
        if (state && aliveNeighbors > 3) return false;
        if (!state && aliveNeighbors == 3) return true;
        return state;
    }

    virtual void executeStep(vector<int> &oldState, vector<int> &newState);
};

class SerialGameOfLife : public IGameOfLifeInstance {
public:
    SerialGameOfLife(int width, int height) : IGameOfLifeInstance(width, height) {
    }

protected:
    virtual void executeStep(vector<int> &oldState, vector<int> &newState) {
        for (int index = 0; index < getBoardSize(); index++) {
            newState[index] = updateCell(oldState, index) ? BLACK : WHITE;
        }
    }


};

class ParallelGameOfLife : public IGameOfLifeInstance {

    int threadCount;
    int threadStride;

public:
    ParallelGameOfLife(int width, int height, int threadCount) : IGameOfLifeInstance(width, height) {
        this->threadCount = threadCount;
        threadStride = getBoardSize() / threadCount;
    }

protected:
    virtual void executeStep(vector<int> &oldState, vector<int> &newState) {
        tbb::parallel_for(0, threadCount, [&](size_t threadIndex) {
            for (int index = threadIndex * threadStride;
                 index < (threadIndex + 1) * threadStride; index++) {
                newState[index] = updateCell(oldState, index) ? BLACK : WHITE;
            }
        });
    }

};