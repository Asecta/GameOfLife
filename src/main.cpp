#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>
#include <chrono>
#include <ctime>
#include "game/GameOfLife.h"

using namespace std;
using namespace std::chrono;

const bool HEADLESS = false;
const int GUI_SCALE = 3;

void populate(IGameOfLife &instance) {
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
        instance.setCell(cellX, cellY, true);
    }

    initialStateFile.close();
}


void runGUI() {
    ParallelGameOfLife instance = ParallelGameOfLife(256, 256, 8);
    // the following is pretty much SDL2 boilerplate
    SDL_Window *window = SDL_CreateWindow("Game of life", 100, 100, instance.getWidth() * GUI_SCALE, instance.getHeight() * GUI_SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, instance.getWidth(), instance.getHeight());
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    populate(instance);

    bool quit = false;
    SDL_Event event;

    int pitch = instance.getWidth() * sizeof(int);

    while (!quit) {
        // Poll the even loop, otherwise windows times out the application
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Update the game
        instance.nextState();

        // Draw our game.
        SDL_UpdateTexture(texture, nullptr, instance.getCurrentState().data(), pitch);
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

    int simulationCount = 8;
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

    for (int threadCount = 1; threadCount <= threadTestCount * threadStep; threadCount += threadStep) {
        cout << "Testing on " << threadCount << " thread(s)" << endl << flush;
        dataFile << endl << flush << threadCount;

        for (int simulationNo = 0; simulationNo < simulationCount; simulationNo++) {
            int width = simulationStartSize + simulationSpaceStep * simulationNo;
            int height = simulationStartSize + simulationSpaceStep * simulationNo;
            ParallelGameOfLife instance = ParallelGameOfLife(width, height, threadCount);

            populate(instance);

            cout << "Running simulation of size " << width << "x" << height << " on " << threadCount << " threads... " << std::flush;

            high_resolution_clock::time_point startTime = high_resolution_clock::now();

            for (int step = 0; step < stepsPerSimulation; step++) {
                instance.nextState();
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