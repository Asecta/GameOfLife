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

    virtual void setCell(int x, int y, bool alive) = 0;

    virtual void nextState() = 0;
};

class BasicGameOfLife : public IGameOfLife {
protected:
    vector<int> gameState;

public:
    BasicGameOfLife(int width, int height);

    vector<int> getCurrentState();

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

class BitStuffedGameOfLife : public IGameOfLife {

protected:
    vector<uint_fast8_t> gameState;
    int byteSize;

public:

    BitStuffedGameOfLife(int width, int height);

    vector<uint_fast8_t> getCurrentState();

    bool getCellByIndex(vector<uint_fast8_t> &board, int index);

    int countAliveNeighbours(vector<uint_fast8_t> &gameState, int index);

    bool updateCell(vector<uint_fast8_t> &gameState, int index);

    void setCell(int x, int y, bool alive) override;

    void nextState() override;

    void executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState);

};

#endif
