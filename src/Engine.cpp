//
//  Engine.cpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>

#include "Engine.hpp"
#include "Utilities.hpp"

Visualizer::Engine::Engine(COUPLE size, const int max_elements) 
: mSize(size), mMAX_NUMBER(max_elements) {

    // Initialize the engine
    if(!init()){
        throw std::runtime_error("Failed to initialize!");
    }
    // Fill the array with numbers from 1 to mMAX_NUMBER
    fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::Engine(COUPLE size, const int max_elements, const char* window_title) 
: mSize(size), mMAX_NUMBER(max_elements), mWindowTitle(window_title) {
    
    // Initialize the engine
    if(!init()){
        throw std::runtime_error("Failed to initialize!");
    }
    // Fill the array with numbers from 1 to mMAX_NUMBER
    fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::Engine(COUPLE size, const int max_elements, const char* window_title, int update_frequency) 
: mSize(size), mMAX_NUMBER(max_elements), mWindowTitle(window_title), mUpdateFrequency(update_frequency) {
    
    // Initialize the engine
    if(!init()){
        throw std::runtime_error("Failed to initialize!");
    }
    // Fill the array with numbers from 1 to mMAX_NUMBER
    fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::~Engine(){
    // Deallocate memory destroying the elements used by the texture
    mTexture->free();
    
    // Destroy the renderer and the window
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    // Set the pointers to NULL
    mRenderer = NULL;
    mWindow = NULL;

    // Quit SDL subsystems
    SDL_Quit();
    TTF_Quit();
}

void Visualizer::Engine::run(){
    // Main loop flag
    while(mIsRunning){
        // Handle events on queue
        handleEvents();

        // If the user requested a sort, sort the array
        if(mRequestSort){
            sort();
        }

        // If the user requested a shuffle, shuffle the array
        if(mRequestShuffle){
            shuffle();
        }
        
        // Clear the screen
        draw();
    }
}

bool Visualizer::Engine::init(){
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Set texture filtering to linear
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
        printf("Warning: linear texture filtering not enabled!");
    }

    // Create the window
    mWindow = SDL_CreateWindow(mWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSize.x, mSize.y, SDL_WINDOW_SHOWN);

    // Check if the window was created
    if(mWindow == NULL){
        printf("Window could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Create the renderer
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Check if the renderer was created
    if(mRenderer == NULL){
        printf("Renderer could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if(TTF_Init() == -1){
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Open the font used for the text
    TTF_Font* font = TTF_OpenFont("../res/Roboto-Regular.ttf", 28);

    // Check if the font was opened
    if(font == NULL){
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Create the texture used for the text
    mTexture = new LTexture(mRenderer, font);
    // Load the text (starting with bubble sort by default)
    mTexture->loadFromRenderedText("BUBBLE SORT", {0xFF, 0xFF, 0xFF, 0xFF});
    // Set background color
    SDL_SetRenderDrawColor(mRenderer, 0x4a, 0x18, 0xa8, 0xFF);
    return true;
}

void Visualizer::Engine::handleEvents(){
    SDL_Event e;

    // Handle events on queue
    while(SDL_PollEvent(&e)){
        // User requests quit
        if(e.type == SDL_QUIT){
            mIsRunning = false;
        }
        // User presses a key 
        else if (e.type == SDL_KEYDOWN){
            switch(e.key.keysym.sym){
                // User presses escape
                case SDLK_ESCAPE:
                    mIsRunning = false;
                    break;
                // User presses the B key
                case SDLK_b:
                    // If the array is not sorted or stopped midexecution and if the current sort is not already bubble sort
                    if(!mIsSorted && !mIsSortStopped && mCurrentSort != BUBBLE_SORT){
                        // Set the current sort to bubble sort
                        mCurrentSort = BUBBLE_SORT;
                        // Load the text for bubble sort
                        mTexture->loadFromRenderedText("BUBBLE SORT", {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the Q key
                case SDLK_q:
                    // If the array is not sorted or stopped midexecution and if the current sort is not already quick sort
                    if(!mIsSorted && !mIsSortStopped && mCurrentSort != QUICK_SORT){
                        // Set the current sort to quick sort
                        mCurrentSort = QUICK_SORT;
                        // Load the text for quick sort
                        mTexture->loadFromRenderedText("QUICK SORT", {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the C key
                case SDLK_c:
                    // If the array is not sorted or stopped midexecution and if the current sort is not already cocktail sort
                    if(!mIsSorted && !mIsSortStopped && mCurrentSort != COCKTAIL_SORT){
                        // Set the current sort to cocktail sort
                        mCurrentSort = COCKTAIL_SORT;
                        // Load the text for cocktail sort
                        mTexture->loadFromRenderedText("COCKTAIL SORT", {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the SPACEBAR key
                case SDLK_SPACE:
                    // If the array is not sorted
                    if(!mIsSorted){
                        // Stops and starts the sort
                        mRequestSort = !mRequestSort;
                    }
                    break;
                // User presses the F key
                case SDLK_f:
                    // If the array is being sorted
                    if(mRequestSort){
                        // Fast forwards the sort
                        mIsFastForward = true;
                    }
                    break;
                // User presses the S key
                case SDLK_s:
                    // If the array is sorted or stopped midexecution
                    if(mIsSorted || mIsSortStopped){
                        // Shuffles the array
                        mRequestShuffle = true;
                    }
                    break;
            }
        }
    }
}

void Visualizer::Engine::sort(){
    // Sorts the array based on the current sort selected
    switch(mCurrentSort){
        case BUBBLE_SORT:
            bubbleSort();
            break;
        case QUICK_SORT:
            // Shuffles the array because quick sort cannot be stopped and resumed so it must be restarted
            shuffle();
            quickSort(0, mArray.size() - 1);
            break;
        case COCKTAIL_SORT:
            // Shuffles the array because cocktail sort cannot be stopped and resumed (for now) so it must be restarted
            shuffle();
            cocktailSort();
            break;
    }

    // Once the array is sorted
    if(!mIsSortStopped){
        // The array is sorted
        mIsSorted = true;
        // The request is stopped
        mRequestSort = false;
        // The fast forward flag is reset
        mIsFastForward = false;
        // (Only for bubble sort for now), the resume index is reset
        mResumeIndex = 0;
    }
}

void Visualizer::Engine::cocktailSort(){
    bool swapped = true;
    int start = 0;
    int end = mArray.size() - 1;
 
    while (swapped) {
        /* 
        * Reset the swapped flag on entering
        * the loop, because it might be true from
        * a previous iteration.
        */
        swapped = false;
 
        /*
        * Loop from left to right same as
        * the bubble sort
        */
        for (int i = start; i < end; ++i) {
            
            // Handle events everytime the loop repeats
            handleEvents();
            // If the sort is stopped or the application is closed, stop the sort
            if(!mIsRunning || !mRequestSort){
                mIsSortStopped = true;
                return;
            }

            if (mArray[i] > mArray[i + 1]) {
                std::swap(mArray[i], mArray[i + 1]);
                /*
                * If the current index is a multiple of the update frequency, draw the array on the screen
                * if fast forward is enabled, draw the array on the screen immediately
                */
                if(i % mUpdateFrequency == 0 && !mIsFastForward)
                    draw();
                swapped = true;
            }
        }
        // if nothing moved, then array is sorted.
        if (!swapped)
            break;
 
        /* 
        * Otherwise, reset the swapped flag so that it
        * can be used in the next stage
        */
        swapped = false;
 
        /*
        * Move the end point back by one, because
        * item at the end is in its rightful spot
        */
        end--;
 
        /*
        * From right to left, doing the
        * same comparison as in the previous stage
        */
        for (int i = end - 1; i >= start; --i) {

            handleEvents();
            if(!mIsRunning || !mRequestSort){
                mIsSortStopped = true;
                return;
            }

            if (mArray[i] > mArray[i + 1]) {
                std::swap(mArray[i], mArray[i + 1]);
                /*
                * If the current index is a multiple of the update frequency, draw the array on the screen
                * if fast forward is enabled, draw the array on the screen immediately
                */
                if(i % mUpdateFrequency == 0 && !mIsFastForward)
                    draw();
                swapped = true;
            }
        }
 
        /*
        * increase the starting point, because
        * the last stage would have moved the next
        * smallest number to its rightful spot.
        */
        start++;

        // Once the array is sorted set the sort stopped flag to false
        mIsSortStopped = false;
    }
}

void Visualizer::Engine::quickSort(int low, int high){
    if (low < high) {
        int pi = partition(low, high);

        // If the sort is stopped or the application is closed while inside of partition, stop the sort
        if(mIsSortStopped){
            return;
        }

        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }

    // Once the array is sorted set the sort stopped flag to false only for the first call of quick sort
    if(low == 0 && high == mArray.size() - 1 && !mIsSortStopped){
        mIsSortStopped = false;
    }
}

int Visualizer::Engine::partition(int low, int high){

    // Handle events partition is called
    handleEvents();
    if(!mIsRunning || !mRequestSort){
        mIsSortStopped = true;
        return -1;
    }

    int pivot = mArray[high];
    int i = (low - 1);
 
    for (int j = low; j <= high - 1; j++) {
         if (mArray[j] < pivot) {
            i++;
            std::swap(mArray[i], mArray[j]);
            /*
            * If the current index is a multiple of the update frequency, draw the array on the screen
            * if fast forward is enabled, draw the array on the screen immediately
            */
            if(j % mUpdateFrequency == 0 && !mIsFastForward)
                draw();
        }
    }
    std::swap(mArray[i + 1], mArray[high]);
    // If fast forward is enabled, draw the array on the screen immediately
    if(!mIsFastForward)
        draw();
    return (i + 1);
}

void Visualizer::Engine::bubbleSort(){
    for (int i = mResumeIndex; i < mArray.size() - 1; i++){
        for (int j = 0; j < mArray.size() - i - 1; j++){
            // Handle events everytime the inner loop repeats
            handleEvents();
            if(!mIsRunning || !mRequestSort){
                mIsSortStopped = true;
                mResumeIndex = i;
                return;
            }

            if (mArray[j] > mArray[j+1]){
                std::swap(mArray[j], mArray[j+1]);
                /*
                * If the current index is a multiple of the update frequency, draw the array on the screen
                * if fast forward is enabled, draw the array on the screen immediately
                */
                if(j % mUpdateFrequency == 0 && !mIsFastForward)
                    draw();
            }
        }
    }

    // Once the array is sorted set the sort stopped flag to false
    mIsSortStopped = false;
}

void Visualizer::Engine::shuffle(){
    // Shuffle the array
    srand(time(NULL));

    for(int i = 0; i < mArray.size(); i++){
        int random = rand() % mArray.size();
        std::swap(mArray[i], mArray[random]);
        // Draw the array every 10% of the way through the shuffle
        if(i % (mMAX_NUMBER / 10) == 0)
            draw();
    }

    //! Draw the array one last time (I MIGHT NEED TO ADD THIS TO THE SORTING ALGORITHMS AS WELL)
    draw();
    // The array is shuffled and not sorted
    mIsSorted = false;
    // Set the shuffle flag to false
    mRequestShuffle = false;
    // (Only for bubble sort for now), the resume index is reset
    mResumeIndex = 0;
}

void Visualizer::Engine::draw(){

    // Clear the screen
    SDL_RenderClear(mRenderer);

    // Render the sort name
    mTexture->render(20, 20);

    // Render the array
    draw_rects();

    // Update the screen
    SDL_RenderPresent(mRenderer);
}

void Visualizer::Engine::draw_rects(){
    // Each element in the array is a rectangle
    SDL_Rect rect;

    // Set the starting color and ending color
    int startColorR = 0xf3;
    int startColorG = 0xee;
    int startColorB = 0xfc;

    int endColorR = 0x69;
    int endColorG = 0x2b;
    int endColorB = 0xe0;

    // Calculate the color step
    double colorStepR = (endColorR - startColorR) / (double) mMAX_NUMBER;
    double colorStepG = (endColorG - startColorG) / (double) mMAX_NUMBER;
    double colorStepB = (endColorB - startColorB) / (double) mMAX_NUMBER;

    // Draw the rectangles
    for(int i = 0; i < mArray.size(); i++){
        // Set the color of each rectangle
        SDL_SetRenderDrawColor(mRenderer, startColorR + colorStepR * i, startColorG + colorStepG * i, startColorB + colorStepB * i, 0xFF);
        // Set the width of the rectangle to the width of the window divided by the number of elements in the array
        rect.w = mSize.x / mMAX_NUMBER;
        // Set the height of the rectangle
        rect.h = ((double) mSize.y / mMAX_NUMBER) * mArray[i];
        // Set the x coordinate by multiplying the index by the width of the rectangle
        rect.x = i * rect.w;
        // Set the y coordinate by subtracting the height of the rectangle from the height of the window
        rect.y = mSize.y - rect.h;

        // Draw the rectangle
        SDL_RenderFillRect(mRenderer, &rect);
    }

    // Set background color
    SDL_SetRenderDrawColor(mRenderer, 0x4a, 0x18, 0xa8, 0xFF);
}

void Visualizer::Engine::fill_array(){
    // Fill the array with numbers from 1 to mMAX_NUMBER
    for(int i = 1; i <= mMAX_NUMBER; i++){
        mArray.push_back(i);
    }
}