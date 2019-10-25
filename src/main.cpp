#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>

using namespace std;

const int scale = 2;
const int width = 512;
const int height = 512;
const int boardSize = width * height;

const int MODE_SERIAL = 0;
const int MODE_PARALLEL = 1;
const int MODE_PARALLEL_BATCHED = 4;

const int BATCHED_THREAD_COUNT = 4;
const int BATCHED_THREAD_STRIDE = boardSize / BATCHED_THREAD_COUNT;

const int CURRENT_MODE = MODE_PARALLEL_BATCHED;


const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

vector<int> gameBoard(boardSize, WHITE);

bool getCellByIndex(vector<int> &board, int index) {
    if (index < 0 || index >= boardSize) return false;
    return board[index] == BLACK;
}

int countAliveNeighbours(vector<int> &board, int index) {
    int count = 0;
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

void serialUpdate(vector<int> &oldGameBoard, vector<int> &newGameBoard) {
    for (int index = 0; index < boardSize; index++) {
        newGameBoard[index] = updateCell(oldGameBoard, index) ? BLACK : WHITE;
    }
}

void parallelUpdate(vector<int> &oldGameBoard, vector<int> &newGameBoard) {
    tbb::parallel_for(size_t(0), size_t(oldGameBoard.size()), [&](size_t index) {
        newGameBoard[index] = updateCell(oldGameBoard, index) ? BLACK : WHITE;
    });
}

void batchedUpdate(vector<int> &oldGameBoard, vector<int> &newGameBoard) {
    tbb::parallel_for(0, BATCHED_THREAD_COUNT, [&](size_t threadIndex) {
        for (int batchIndex = threadIndex * BATCHED_THREAD_STRIDE;
             batchIndex < (threadIndex + 1) * BATCHED_THREAD_STRIDE; batchIndex++) {
            int index = +batchIndex;
            newGameBoard[index] = updateCell(oldGameBoard, index) ? BLACK : WHITE;
        }
    });
}

void updateGame() {
    vector<int> newGameBoard(boardSize, WHITE);

    switch (CURRENT_MODE) {
        case MODE_SERIAL:
            serialUpdate(gameBoard, newGameBoard);
            break;
        case MODE_PARALLEL:
            parallelUpdate(gameBoard, newGameBoard);
            break;
        case MODE_PARALLEL_BATCHED:
            batchedUpdate(gameBoard, newGameBoard);
            break;
    }

    gameBoard = newGameBoard;
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
        gameBoard[cellY * height + cellX] = BLACK;
    }
    
    initialStateFile.close();
}

int main(int argc, char *argv[]) {
    using std::cerr;
    using std::endl;

    // the following is pretty much SDL2 boilerplate

    SDL_Window *win = SDL_CreateWindow("Game of life", 100, 100, width * scale, height * scale, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);

    populate();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // Poll the even loop, otherwise windows times out the application
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        updateGame();

        // Draw our game.
        SDL_UpdateTexture(tex, nullptr, gameBoard.data(), width * sizeof(int));
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, nullptr, nullptr);
        SDL_RenderPresent(ren);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}