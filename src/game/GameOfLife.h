#ifndef GAMEOFLIFE_GAMEOFLIFE_H
#define GAMEOFLIFE_GAMEOFLIFE_H

#include <vector>

using namespace std;

const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

using namespace std;

class IGameOfLife {

protected:
    int width;
    int height;
    int boardSize;

public:
    IGameOfLife(int width, int height);

    int getWidth();

    int getHeight();

    int getBoardSize();

    virtual vector<int> getCurrentState() = 0;

    virtual void setCell(int x, int y, bool alive) = 0;

    virtual void nextState() = 0;
};

class BasicGameOfLife : public IGameOfLife {
protected:
    vector<int> gameState;

public:
    BasicGameOfLife(int width, int height);

    vector<int> getCurrentState() override;

    bool getCellByIndex(vector<int> &board, int index);

    int countAliveNeighbours(vector<int> &gameState, int index);

    bool updateCell(vector<int> &gameState, int index);

    void setCell(int x, int y, bool alive) override;

    void nextState() override;


public:
    virtual void executeStep(vector<int> &oldState, vector<int> &newState) = 0;
};

class SerialGameOfLife : public BasicGameOfLife {
public:
    SerialGameOfLife(int width, int height);

    void executeStep(vector<int> &oldState, vector<int> &newState) override;
};

class ParallelGameOfLife : public BasicGameOfLife {
protected:
    int threadCount;
    int threadStride;
public:
    ParallelGameOfLife(int width, int height, int threadCount);

    void executeStep(vector<int> &oldState, vector<int> &newState) override;
};

#endif
