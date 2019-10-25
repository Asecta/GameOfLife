#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>

using namespace std;

const int GUI_SCALE = 2;
const int WIDTH = 512;
const int HEIGHT = 512;
const int BOARD_SIZE = WIDTH * HEIGHT;

const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

const int MODE_SERIAL = 0;
const int MODE_PARALLEL = 1;
const int MODE_PARALLEL_BATCHED = 4;

const int BATCHED_THREAD_COUNT = 4;
const int BATCHED_THREAD_STRIDE = BOARD_SIZE / BATCHED_THREAD_COUNT;

const int CURRENT_MODE = MODE_PARALLEL_BATCHED;

// Storing the game as integers rather than booleans, as it's easier to render and i think c++ stores bools as 16 bits anyway.
vector<int> currentGameState(BOARD_SIZE, WHITE);

bool getCellByIndex(vector<int> &board, int index) {
    if (index < 0 || index >= BOARD_SIZE) return false;
    return board[index] == BLACK;
}

int countAliveNeighbours(vector<int> &board, int index) {
    int count = 0;
    // Hard-Coded neighbour iteration since idk how c++ unwraps for loops and it's easier on a 1d array.
    count += getCellByIndex(board, index - WIDTH - 1) ? 1 : 0;
    count += getCellByIndex(board, index - WIDTH) ? 1 : 0;
    count += getCellByIndex(board, index - WIDTH + 1) ? 1 : 0;
    count += getCellByIndex(board, index - 1) ? 1 : 0;
    count += getCellByIndex(board, index + 1) ? 1 : 0;
    count += getCellByIndex(board, index + WIDTH - 1) ? 1 : 0;
    count += getCellByIndex(board, index + WIDTH) ? 1 : 0;
    count += getCellByIndex(board, index + WIDTH + 1) ? 1 : 0;
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

void serialUpdate(vector<int> &oldGameState, vector<int> &newGameState) {
    for (int index = 0; index < BOARD_SIZE; index++) {
        newGameState[index] = updateCell(oldGameState, index) ? BLACK : WHITE;
    }
}

void parallelUpdate(vector<int> &oldGameState, vector<int> &newGameState) {
    tbb::parallel_for(size_t(0), size_t(oldGameState.size()), [&](size_t index) {
        newGameState[index] = updateCell(oldGameState, index) ? BLACK : WHITE;
    });
}

void batchedUpdate(vector<int> &oldGameState, vector<int> &newGameState) {
    tbb::parallel_for(0, BATCHED_THREAD_COUNT, [&](size_t threadIndex) {
        for (int index = threadIndex * BATCHED_THREAD_STRIDE;
             index < (threadIndex + 1) * BATCHED_THREAD_STRIDE; index++) {
            newGameState[index] = updateCell(oldGameState, index) ? BLACK : WHITE;
        }
    });
}

void updateGame() {
    vector<int> newGameState(BOARD_SIZE, WHITE);

    switch (CURRENT_MODE) {
        case MODE_SERIAL:
            serialUpdate(currentGameState, newGameState);
            break;
        case MODE_PARALLEL:
            parallelUpdate(currentGameState, newGameState);
            break;
        case MODE_PARALLEL_BATCHED:
            batchedUpdate(currentGameState, newGameState);
            break;
    }

    currentGameState = newGameState;
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
        currentGameState[cellY * HEIGHT + cellX] = BLACK;
    }

    initialStateFile.close();
}

int main(int argc, char *argv[]) {
    // the following is pretty much SDL2 boilerplate
    SDL_Window *window = SDL_CreateWindow("Game of life", 100, 100, WIDTH * GUI_SCALE, HEIGHT * GUI_SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
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
        SDL_UpdateTexture(texture, nullptr, currentGameState.data(), WIDTH * sizeof(int));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}