#include "GameOfLife.h"

// Simple GOL interface

IGameOfLife::IGameOfLife(int width, int height) {
    this->width = width;
    this->height = height;
    this->boardSize = width * height;
}

int IGameOfLife::getBoardSize() {
    return boardSize;
}

int IGameOfLife::getWidth() {
    return width;
}

int IGameOfLife::getHeight() {
    return height;
}
