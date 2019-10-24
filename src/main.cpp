#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>

using namespace std;

const int MODE_SERIAL = 0;
const int MODE_PARALLEL = 1;
const int MODE_PARALLEL_BATCHED = 2;

const int CURRENT_MODE = MODE_PARALLEL;

const int scale = 8;
const int width = 64;
const int height = 64;
const int boardSize = width * height;

const int BLACK = 0x0;
const int WHITE = 0xFFFFFF;

int *gameBoard = new int[boardSize];

bool getCellByIndex(int *board, int index) {
    if (index < 0 || index > boardSize) {
        return false;
    }
    return board[index] == BLACK;
}

int countAliveNeighbours(int *board, int index) {
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

bool updateCell(int *board, int index) {
    int aliveNeighbors = countAliveNeighbours(board, index);
    bool state = getCellByIndex(board, index);

    if (state && aliveNeighbors < 2) return false;
    if (state && aliveNeighbors > 3) return false;
    if (!state && aliveNeighbors == 3) return true;

    return state;
}

void serialUpdate(int *oldGameBoard, int *newGameBoard) {
    for (int index = 0; index < boardSize; index++) {
        newGameBoard[index] = updateCell(oldGameBoard, index) ? BLACK : WHITE;
    }
}

void parallelUpdate(int *oldGameBoard, int *newGameBoard) {
    tbb::parallel_for(size_t(0), size_t(oldGameBoard), [&](int i) {
        newGameBoard[i] = updateCell(oldGameBoard, i) ? BLACK : WHITE;
    });
}

void batchedUpdate(int *oldGameBoard, int *newGameBoard) {

}

void updateGame() {
    int *newGameBoard = new int[boardSize];

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

    char c;
    int index = 0;
    while (initialStateFile >> c) {
        if (c != 'x' && c != '-') continue;
        gameBoard[index++] = c == 'x' ? BLACK : WHITE;
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
        // Update the game board.

        // Draw our game.
        SDL_UpdateTexture(tex, nullptr, gameBoard, width * sizeof(int));
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, nullptr, nullptr);
        SDL_RenderPresent(ren);
        updateGame();
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}