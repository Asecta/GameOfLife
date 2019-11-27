#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>
#include <chrono>
#include "game/GameOfLife.h"

using namespace std;
using namespace std::chrono;

const bool HEADLESS = false;
const int GUI_SCALE = 3;

void populate(IGameOfLife &instance) {
    // Open file input stream
    ifstream worldFile;
    worldFile.open("../initialstate.txt");

    // Game state memory encoded as (x1 y1 x2 y2..) No data validation as for testing

    vector<int> vectorArray;

    int in;
    while (worldFile >> in) { // Wow that was easy
        vectorArray.push_back(in);
    }

    // Iterate over coord pairs and initialize them on the game instance
    for (int i = 0; i < vectorArray.size(); i += 2) {
        int cellX = vectorArray[i];
        int cellY = vectorArray[i + 1];
        instance.setCell(cellX, cellY, true);
    }

    worldFile.close();
}


void runGUI() {
    //Instantiate a GOL version for demonstration
    ParallelGameOfLife instance = ParallelGameOfLife(256, 256, 8);
    // the following is pretty much SDL2 boilerplate
    SDL_Window *window = SDL_CreateWindow("Game of life", 100, 100, instance.getWidth() * GUI_SCALE, instance.getHeight() * GUI_SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Using RGB332 format for displaying 8 bits colours. Saves time on any conversion
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING,
                                             instance.getWidth(), instance.getHeight());
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    int pitch = instance.getWidth() * sizeof(uint_fast8_t);

    // Populate the initial game state
    populate(instance);

    SDL_Event event;
    bool running = true;
    while (running) {
        // Poll the even loop, otherwise windows times out the application
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Update the game
        instance.nextState();

        // Render the latest game state
        SDL_UpdateTexture(texture, nullptr, instance.getCurrentState().data(), pitch);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // Destruct SDL
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void runHeadless() {
    int generationsPerSimulation = 1000;
    int simulationStartSize = 128; // The width and height to begin with (total size = this^2)
    int simulationSizeStep = 128; // The amount to increase the size by per simulation

    int simulationCount = 16; // The number of simulations to test
    int threadPoolStartSize = 1; // The number of threads to start with
    int threadPoolStep = 1; // The number of threads to increase by per generation

    cout << "Running game of life in headless mode..." << endl << flush;
    cout << "Steps per simulation: " << generationsPerSimulation << endl << flush;
    cout << "Number of simulations: " << simulationCount << endl << flush;
    cout << "Simulation space size step: " << simulationSizeStep << endl << flush;

    high_resolution_clock::time_point totalStartTime = high_resolution_clock::now();

    // Open data output stream
    ofstream dataFile;
    dataFile.open("../data.csv");

    // write CSV headers to file
    for (int i = 0; i < simulationCount; i++) {
        int size = simulationStartSize + simulationSizeStep * i;
        dataFile << "," << size << "x" << size;
    }

    // Start simulating
    for (int poolSize = 1; poolSize <= 24; poolSize++) {
        cout << "Simulating on " << poolSize << " thread(s)" << endl << flush;
        dataFile << endl << flush << poolSize;

        // Iterate over each pool size
        for (int simulationNo = 0; simulationNo < simulationCount; simulationNo++) {
            int width = simulationStartSize + simulationSizeStep * simulationNo;
            int height = simulationStartSize + simulationSizeStep * simulationNo;
            ParallelGameOfLife instance = ParallelGameOfLife(width, height, poolSize);

            // Populate with initial data
            populate(instance);

            cout << "[Simulation] " << width << "x" << height << ", " << poolSize << " threads.. " << flush;

            // Log time before simulating
            high_resolution_clock::time_point startTime = high_resolution_clock::now();

            for (int step = 0; step < generationsPerSimulation; step++) {
                instance.nextState();
            }

            // Calculate time difference between start and current time
            high_resolution_clock::time_point endTime = high_resolution_clock::now();
            double time = duration_cast<duration<double>>(endTime - startTime).count();

            double stepPerSeconds = generationsPerSimulation / time;
            cout << "Done " << time << " s " << "(" << stepPerSeconds << "steps p/s)\n" << flush;

            // Write results to file
            dataFile << "," << time;
        }
    }

    high_resolution_clock::time_point totalEndTime = high_resolution_clock::now();
    double time = duration_cast<duration<double>>(totalEndTime - totalStartTime).count();

    cout << "Finished in " << time << " seconds" << endl << flush;

    dataFile.close();
}

int main(int argc, char *argv[]) {
    // Switch between GUI and Headless mode, headless mode is for generating data
    if (HEADLESS) {
        runHeadless();
    } else {
        runGUI();
    }
    return EXIT_SUCCESS;
}