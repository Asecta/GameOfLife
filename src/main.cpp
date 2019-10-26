#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;

const bool HEADLESS = false;
const int GUI_SCALE = 3;
const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

const int MODE_SERIAL = 0;
const int MODE_PARALLEL = 1;
const int MODE_PARALLEL_BATCHED = 4;
const int CURRENT_MODE = MODE_PARALLEL_BATCHED;

int width = 224;
int height = 224;
int boardSize = width * height;

int threadCount = 1;
int threadStride = boardSize / threadCount;


// Storing the game as integers rather than booleans, as it's easier to render and i think c++ stores bools as 16 bits anyway.
vector<int> gameState(boardSize, WHITE);

bool getCellByIndex(vector<int> &board, int index) {
    if (index < 0 || index >= boardSize) return false;
    return board[index] == BLACK;
}

int countAliveNeighbours(vector<int> &board, int index) {
    int count = 0;
    // Hard-Coded neighbour iteration since idk how c++ unwraps for loops and it's easier on a 1d array.
    count += getCellByIndex(board, index - width - 1) ? 1 : 0;
    count += getCellByIndex(board, index - width) ? 1 : 0;
    count += getCellByIndex(board, index - width + 1) ? 1 : 0;
    count += getCellByIndex(board, index - 1) ? 1 : 0;
    count += getCellByIndex(board, index + 1) ? 1 : 0;
    count += getCellByIndex(board, index + width - 1) ? 1 : 0;
    count += getCellByIndex(board, index + width) ? 1 : 0;
    count += getCellByIndex(board, index + width + 1) ? 1 : 0;
    return count;
}

bool updateCell(vector<int> &board, int index) {
    int aliveNeighbors = countAliveNeighbours(board, index);
    bool state = getCellByIndex(board, index);

    if (state && aliveNeighbors < 2) return false;
    if (state && aliveNeighbors > 3) return false;
    if (!state && aliveNeighbors == 3) return true;
    return state;
}

void serialUpdate(vector<int> oldGameState, vector<int> &newGameState) {
    for (int index = 0; index < boardSize; index++) {
        newGameState[index] = updateCell(oldGameState, index) ? BLACK : WHITE;
    }
}

void parallelUpdate(vector<int> oldGameState, vector<int> &newGameState) {
    tbb::parallel_for(size_t(0), size_t(oldGameState.size()), [&](size_t index) {
        newGameState[index] = updateCell(oldGameState, index) ? BLACK : WHITE;
    });
}

void batchedUpdate(vector<int> oldGameState, vector<int> &newGameState) {
    tbb::parallel_for(0, threadCount, [&](size_t threadIndex) {
        for (int index = threadIndex * threadStride;
             index < (threadIndex + 1) * threadStride; index++) {
            newGameState[index] = updateCell(oldGameState, index) ? BLACK : WHITE;
        }
    });
}

void updateGame() {
    batchedUpdate(gameState, gameState);

    switch (CURRENT_MODE) {
        case MODE_SERIAL:
            serialUpdate(gameState, gameState);
            break;
        case MODE_PARALLEL:
            parallelUpdate(gameState, gameState);
            break;
        case MODE_PARALLEL_BATCHED:
            batchedUpdate(gameState, gameState);
            break;
    }
}


void populate() {
    ifstream initialStateFile;
    initialStateFile.open("../initialstate.txt");

    vector<int> vectorArray;

    int in;
    while (initialStateFile >> in) {
        vectorArray.push_back(in);
    }

    for (int i = 0; i < vectorArray.size(); i += 2) {
        int cellX = vectorArray[i];
        int cellY = vectorArray[i + 1];
        gameState[cellY * height + cellX] = BLACK;
    }

    initialStateFile.close();
}


void runGUI() {
    // the following is pretty much SDL2 boilerplate
    SDL_Window *window = SDL_CreateWindow("Game of life", 100, 100, width * GUI_SCALE, height * GUI_SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    populate();

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        // Poll the even loop, otherwise windows times out the application
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        updateGame();
        // Draw our game.
        SDL_UpdateTexture(texture, nullptr, gameState.data(), width * sizeof(int));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void runHeadless() {
    int stepsPerSimulation = 1000;
    int simulationStartSize = 64;
    int simulationSpaceStep = 64;

    int simulationCount = 16;
    int threadTestCount = 16;
    int threadStep = 2;

    cout << "Running game of life in headless mode..." << endl << flush;
    cout << "Steps per simulation: " << stepsPerSimulation << endl << flush;
    cout << "Number of simulations: " << simulationCount << endl << flush;
    cout << "Simulation space size step: " << simulationSpaceStep << endl << flush;

    ofstream dataFile;
    dataFile.open("../data.csv");

    for (int i = 0; i < simulationCount; i++) {
        int size = simulationStartSize + simulationSpaceStep * i;
        dataFile << "," << size << "x" << size;
    }

    for (threadCount = 1; threadCount <= threadTestCount * threadStep; threadCount += threadStep) {
        cout << "Testing on " << threadCount << " thread(s)" << endl << flush;
        dataFile << endl << flush << threadCount;

        for (int simulationNo = 0; simulationNo < simulationCount; simulationNo++) {
            width = simulationStartSize + simulationSpaceStep * simulationNo;
            height = simulationStartSize + simulationSpaceStep * simulationNo;
            boardSize = width * height;
            gameState = vector<int>(boardSize, WHITE);
            threadStride = boardSize / threadCount;

            populate();

            cout << "Running simulation of size " << width << "x" << height << " on " << threadCount << " threads... " << std::flush;

            high_resolution_clock::time_point startTime = high_resolution_clock::now();

            for (int step = 0; step < stepsPerSimulation; step++) {
                updateGame();
            }

            high_resolution_clock::time_point endTime = high_resolution_clock::now();
            double time = duration_cast<duration<double>>(endTime - startTime).count();

            cout << "Took " << time << " seconds " << "(" << stepsPerSimulation / time << "steps p/s)\n" << std::flush;

            dataFile << "," << time;
        }
    }

    cout << "Finished!" << endl << flush;
    dataFile.close();

}

int main(int argc, char *argv[]) {
    if (HEADLESS) {
        runHeadless();
    } else {
        runGUI();
    }
    return EXIT_SUCCESS;
}