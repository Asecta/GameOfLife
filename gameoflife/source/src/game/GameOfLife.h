#ifndef GAMEOFLIFE_GAMEOFLIFE_H
#define GAMEOFLIFE_GAMEOFLIFE_H

#include <vector>

using namespace std;

// Storing black and white as consts for better readability later
const uint_fast8_t ALIVE = 0x00;
const uint_fast8_t DEAD = 0xFF;
class IGameOfLife {

private:
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
private:
    vector<uint_fast8_t> gameState;

public:
    BasicGameOfLife(int width, int height);

    vector<uint_fast8_t> getCurrentState();

    bool getCellByIndex(vector<uint_fast8_t> &board, int index);
    void setCell(int x, int y, bool alive) override;

    bool updateCell(vector<uint_fast8_t> &gameState, int index);

    int countAliveNeighbours(vector<uint_fast8_t> &gameState, int index);
    void nextState() override;

    virtual void executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState) = 0;
};

class SerialGameOfLife : public BasicGameOfLife {
public:
    SerialGameOfLife(int width, int height);

    void executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState) override;
};

class ParallelGameOfLife : public BasicGameOfLife {
private:
    int threadPoolSize;
    int dataStride;
public:
    ParallelGameOfLife(int width, int height, int threadPoolSize);

    void executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState) override;
};

class BitStuffedGameOfLife : public IGameOfLife {

private:
    vector<uint_fast8_t> gameState;
    int byteSize;

public:

    BitStuffedGameOfLife(int width, int height);

    vector<uint_fast8_t> getCurrentState();
    bool getCellByIndex(vector<uint_fast8_t> &board, int index);
    void setCell(int x, int y, bool alive) override;

    bool updateCell(vector<uint_fast8_t> &gameState, int index);

    int countAliveNeighbours(vector<uint_fast8_t> &gameState, int index);
    void nextState() override;
    void executeStep(vector<uint_fast8_t> &oldState, vector<uint_fast8_t> &newState);
};

#endif