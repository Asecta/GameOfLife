#ifndef GAMEOFLIFE_GAMEOFLIFEINSTANCE_H
#define GAMEOFLIFE_GAMEOFLIFEINSTANCE_H

const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

using namespace std;

class IGameOfLifeInstance {
private:
    int width;
    int height;
    int boardSize;
    vector<int> gameState;

public:
    IGameOfLifeInstance(int width, int height);

    vector<int> getCurrentState();

    int getWidth() { return this->width; };

    int getHeight() { return this->height; };

    int getBoardSize() { return this->boardSize; };

    void setCell(int x, int y, bool state);

    void nextState();

protected:
    int countAliveNeighbours();

    bool getCellByIndex(vector<int> &gameState, int index);

    bool updateCell(vector<int> &gameState, int index);

    virtual void executeStep(vector<int> &oldState, vector<int> &newState);
};

class SerialGameOfLife : public IGameOfLifeInstance {
public:
    SerialGameOfLife(int width, int height) : IGameOfLifeInstance(width, height) {}
};

class ParallelGameOfLife : public IGameOfLifeInstance {
public:
    ParallelGameOfLife(int width, int height, int threadCount) : IGameOfLifeInstance(width, height) {}
};

#endif
