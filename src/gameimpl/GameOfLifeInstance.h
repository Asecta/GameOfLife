#ifndef GAMEOFLIFE_GAMEOFLIFEINSTANCE_H
#define GAMEOFLIFE_GAMEOFLIFEINSTANCE_H

const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

using namespace std;

class IGameOfLifeInstance {

    int width;
    int height;
    int boardSize;
    vector<int> gameState;

public:
    IGameOfLifeInstance(int width, int height);

    vector<int> getCurrentState();

    void setCell(int x, int y, bool state);

    void nextState();

    int getBoardSize();

    int getWidth();

    int getHeight();

protected:
    bool getCellByIndex(vector<int> &board, int index);

    int countAliveNeighbours(vector<int> &gameState, int index);

    bool updateCell(vector<int> &gameState, int index);

    virtual void executeStep(vector<int> &oldState, vector<int> &newState);
};


class SerialGameOfLife : public IGameOfLifeInstance {
public:
    SerialGameOfLife(int width, int height);

    void executeStep(vector<int> &oldState, vector<int> &newState) override;
};

class ParallelGameOfLife : public IGameOfLifeInstance {
    int threadCount;
    int threadStride;
public:
    ParallelGameOfLife(int width, int height, int threadCount);

    void executeStep(vector<int> &oldState, vector<int> &newState) override;
};

#endif
