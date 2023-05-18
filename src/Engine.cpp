//
//  Engine.cpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <numeric>
#include <cmath>

#include "Engine.hpp"

Visualizer::Engine::Engine(const COUPLE size, const int max_elements) 
: mSize(size), mMAX_NUMBER(max_elements), mUsableWidth(size.x - (size.x / 4)), mArray(max_elements) {

    // Initialize the engine
    if(!init()){
        throw std::runtime_error("Failed to initialize!");
    }

    // Fill the array with numbers from 1 to mMAX_NUMBER
    std::iota(mArray.begin(), mArray.end(), 1);

    // Fill the array with numbers from 1 to mMAX_NUMBER
    //fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::Engine(const COUPLE size, const int max_elements, const char* window_title) 
: mSize(size), mMAX_NUMBER(max_elements), mWindowTitle(window_title), mUsableWidth(size.x - (size.x / 4)), mArray(max_elements) {
    
    // Initialize the engine
    if(!init()){
        throw std::runtime_error("Failed to initialize!");
    }
    
    // Fill the array with numbers from 1 to mMAX_NUMBER
    std::iota(mArray.begin(), mArray.end(), 1);

    // Fill the array with numbers from 1 to mMAX_NUMBER
    //fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::Engine(const COUPLE size, const int max_elements, const char* window_title, int update_frequency) 
: mSize(size), mMAX_NUMBER(max_elements), mWindowTitle(window_title), mUpdateFrequency(update_frequency), mUsableWidth(size.x - (size.x / 4)), mArray(max_elements) {
    
    // Initialize the engine
    if(!init()){
        throw std::runtime_error("Failed to initialize!");
    }
    
    // Fill the array with numbers from 1 to mMAX_NUMBER
    std::iota(mArray.begin(), mArray.end(), 1);

    // Fill the array with numbers from 1 to mMAX_NUMBER
    //fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::~Engine(){
    // Deallocate memory destroying the elements used by the texture
    mTexture->free();
    // Deallocate memory destroying the elements used by the info texture
    mInfoTexture->free();

    // Close the fonts
    TTF_CloseFont(mFontSmall);
    TTF_CloseFont(mFontLarge);
    mFontSmall = NULL;
    mFontLarge = NULL;

    // Destroy the music
    Mix_FreeChunk(mSwapSound);
    mSwapSound = NULL;

    // Destroy the renderer and the window
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    // Set the pointers to NULL
    mRenderer = NULL;
    mWindow = NULL;

    // Quit SDL subsystems
    SDL_Quit();
    Mix_Quit();
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
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        printf("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Set texture filtering to linear
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
        printf("Warning: linear texture filtering not enabled!");
    }

    // Create the window
    mWindow = SDL_CreateWindow(mWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSize.x, mSize.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

    // Initialize SDL_mixer
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    // Open the font used for the text
    mFontSmall = TTF_OpenFont("../res/Roboto-Regular.ttf", mSize.y / 35);

    // Check if the font was opened
    if(mFontSmall == NULL){
        printf("Failed to load small font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Open the font used for the title text
    mFontLarge = TTF_OpenFont("../res/Roboto-Regular.ttf", mSize.y / 25);

    // Check if the font was opened
    if(mFontLarge == NULL){
        printf("Failed to load small font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Create the texture used for the text
    mTexture = new LTexture(mRenderer, mFontLarge);
    // Load the text (starting with bubble sort by default)
    mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
    // Create the texture used for the info text
    mInfoTexture = new LTexture(mRenderer, mFontSmall);
    // Load the info text
    mInfoTexture->loadFromRenderedText(gINFO_TEXT, {0xFF, 0xFF, 0xFF, 0xFF}, true);
    // Load sound effects
    mSwapSound = Mix_LoadWAV("../res/swap.wav");
    if(mSwapSound == NULL){
        printf("Failed to load swap sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }

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
        else if (e.type == SDL_WINDOWEVENT){
            // Window resized
            if(e.window.event == SDL_WINDOWEVENT_RESIZED){
                // Get the new window size
                mSize.x = e.window.data1;
                mSize.y = e.window.data2;
                // Resize the window
                SDL_SetWindowSize(mWindow, mSize.x, mSize.y);
                // Change usable area
                mUsableWidth = mSize.x - (mSize.x / 4);
                
                // Resize the texture
                mTexture->setFontSize(round(mSize.y / 25.0));
                mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});

                mInfoTexture->setFontSize(round(mSize.y / 35.0));
                mInfoTexture->loadFromRenderedText(gINFO_TEXT, {0xFF, 0xFF, 0xFF, 0xFF}, true);
            }
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
                    // If the array is not sorted and if the current sort is not already bubble sort
                    if(!mRequestSort && mCurrentSort != BUBBLE_SORT){
                        // Set the current sort to bubble sort
                        mCurrentSort = BUBBLE_SORT;
                        // Load the text for bubble sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the Q key
                case SDLK_q:
                    // If the array is not sorted and if the current sort is not already quick sort
                    if(!mRequestSort && mCurrentSort != QUICK_SORT){
                        // Set the current sort to quick sort
                        mCurrentSort = QUICK_SORT;
                        // Load the text for quick sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the C key
                case SDLK_c:
                    // If the array is not sorted and if the current sort is not already cocktail sort
                    if(!mRequestSort && mCurrentSort != COCKTAIL_SORT){
                        // Set the current sort to cocktail sort
                        mCurrentSort = COCKTAIL_SORT;
                        // Load the text for cocktail sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the E key
                case SDLK_e:
                    // If the array is not sorted and if the current sort is not already shell sort
                    if(!mRequestSort && mCurrentSort != SHELL_SORT){
                        // Set the current sort to shell sort
                        mCurrentSort = SHELL_SORT;
                        // Load the text for shell sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the H key
                case SDLK_h:
                    // If the array is not sorted and if the current sort is not already heap sort
                    if(!mRequestSort && mCurrentSort != HEAP_SORT){
                        // Set the current sort to heap sort
                        mCurrentSort = HEAP_SORT;
                        // Load the text for heap sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the M key
                case SDLK_m:
                    // If the array is not sorted and if the current sort is not already merge sort
                    if(!mRequestSort && mCurrentSort != MERGE_SORT){
                        // Set the current sort to merge sort
                        mCurrentSort = MERGE_SORT;
                        // Load the text for merge sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the L key
                case SDLK_l:
                    // If the array is not sorted and if the current sort is not already selection sort
                    if(!mRequestSort && mCurrentSort != SELECTION_SORT){
                        // Set the current sort to selection sort
                        mCurrentSort = SELECTION_SORT;
                        // Load the text for selection sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the I key
                case SDLK_i:
                    // If the array is not sorted and if the current sort is not already insertion sort
                    if(!mRequestSort && mCurrentSort != INSERTION_SORT){
                        // Set the current sort to insertion sort
                        mCurrentSort = INSERTION_SORT;
                        // Load the text for insertion sort
                        mTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], {0xFF, 0xFF, 0xFF, 0xFF});
                    }
                    break;
                // User presses the SPACEBAR key
                case SDLK_SPACE:
                    // If the array is not sorted
                    if(!mIsSorted && !mRequestSort){
                        // Stops and starts the sort
                        mRequestSort = true;
                    } else if(mRequestSort){
                        // Fast forwards the sort
                        mIsFastForward = true;
                    }
                    break;
                // User presses the S key
                case SDLK_s:
                    // If the array is sorted
                    if(mIsSorted){
                        // Shuffles the array
                        mRequestShuffle = true;
                    }
                    break;
                // User presses the X key
                case SDLK_x:
                    // Enables and disables the sound
                    mIsSoundEnabled = !mIsSoundEnabled;
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
            quickSort(0, mMAX_NUMBER - 1);
            break;
        case COCKTAIL_SORT:
            cocktailSort();
            break;
        case SHELL_SORT:
            shellSort();
            break;
        case HEAP_SORT:
            heapSort();
            break;
        case MERGE_SORT:
            mergeSort(0, mMAX_NUMBER - 1);
            break;
        case SELECTION_SORT:
            selectionSort();
            break;
        case INSERTION_SORT:
            insertionSort();
            break;
    }
    // The array is sorted
    mIsSorted = true;
    // The request is stopped
    mRequestSort = false;
    // The fast forward flag is reset
    mIsFastForward = false;
}

void Visualizer::Engine::cocktailSort(){
    bool swapped = true;
    int start = 0;
    int end = mMAX_NUMBER - 1;
 
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
            if (mArray[i] > mArray[i + 1]) {
                std::swap(mArray[i], mArray[i + 1]);
                mSwapCount++;
                if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                    // Handle events every few iterations
                    handleEvents();
                    if(!mIsRunning)
                        return;
                    if(mIsSoundEnabled)
                        Mix_PlayChannel(-1, mSwapSound, 0);
                    draw();
                }
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
            if (mArray[i] > mArray[i + 1]) {
                std::swap(mArray[i], mArray[i + 1]);
                mSwapCount++;
                if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                    // Handle events every few iterations
                    handleEvents();
                    if(!mIsRunning)
                        return;
                    if(mIsSoundEnabled)
                        Mix_PlayChannel(-1, mSwapSound, 0);
                    draw();
                }
                swapped = true;
            }
        }
 
        /*
        * increase the starting point, because
        * the last stage would have moved the next
        * smallest number to its rightful spot.
        */
        start++;
    }
}

void Visualizer::Engine::quickSort(int low, int high){
    if (low < high) {
        int pi = partition(low, high);

        // If the sort is stopped or the application is closed while inside of partition, stop the sort
        if(!mIsRunning){
            return;
        }

        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }
}

int Visualizer::Engine::partition(int low, int high){
    // Handle events time partition is called
    handleEvents();
    if(!mIsRunning)
        return -1;
    int pivot = mArray[high];
    int i = (low - 1);
 
    for (int j = low; j <= high - 1; j++) {
         if (mArray[j] < pivot) {
            i++;
            std::swap(mArray[i], mArray[j]);
            mSwapCount++;
            if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                if(mIsSoundEnabled)
                    Mix_PlayChannel(-1, mSwapSound, 0);
                draw();
            }
        }
    }
    std::swap(mArray[i + 1], mArray[high]);
    mSwapCount++;
    // If fast forward is enabled, draw the array on the screen immediately
    if(mSwapCount % mUpdateFrequency && !mIsFastForward){
        if(mIsSoundEnabled)
            Mix_PlayChannel(-1, mSwapSound, 0);
        draw();
    }
    return (i + 1);
}

void Visualizer::Engine::bubbleSort(){
    bool swapped;
    for (int i = 0; i < mMAX_NUMBER - 1; i++){
        swapped = false;
        for (int j = 0; j < mMAX_NUMBER - i - 1; j++){

            if (mArray[j] > mArray[j+1]){
                std::swap(mArray[j], mArray[j+1]);
                swapped = true;
                mSwapCount++;
                if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                    // Handle events every few iterations
                    handleEvents();
                    if(!mIsRunning)
                        return;
                    if(mIsSoundEnabled)
                        Mix_PlayChannel(-1, mSwapSound, 0);
                    draw();
                }
            }
        }

        // If no two elements were swapped in the inner loop, the array is already sorted
        if (!swapped) {
            break;
        }
    }
}

void Visualizer::Engine::shellSort(){
    // Start with a big gap, then reduce the gap
    for (int gap = mMAX_NUMBER / 2; gap > 0; gap /= 2){
        /*
        * Do a gapped insertion sort for this gap size.
        * The first gap elements a[0..gap-1] are already in gapped order
        * keep adding one more element until the entire array is
        * gap sorted 
        */
        for (int i = gap; i < mMAX_NUMBER; i += 1){
            /*
            * Add a[i] to the elements that have been gap sorted
            * save a[i] in temp and make a hole at position i
            */
            int temp = mArray[i];
  
            /*
            * shift earlier gap-sorted elements up until the correct 
            * location for a[i] is found
            */
            int j;            
            for (j = i; j >= gap && mArray[j - gap] > temp; j -= gap){
                mArray[j] = mArray[j - gap];
                mSwapCount++;
                if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                    // Handle events every few iterations
                    handleEvents();
                    if(!mIsRunning)
                        return;
                    if(mIsSoundEnabled)
                        Mix_PlayChannel(-1, mSwapSound, 0);
                    draw();
                }
            }

            //  put temp (the original a[i]) in its correct location
            mArray[j] = temp;
            mSwapCount++;
            if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                // Handle events every few iterations
                handleEvents();
                if(!mIsRunning)
                    return;
                draw();
            }
        }
    }
}

void Visualizer::Engine::heapify(int n, int i){
    // Initialize largest as root
    int largest = i;
 
    // left = 2*i + 1
    int l = 2 * i + 1;
 
    // right = 2*i + 2
    int r = 2 * i + 2;
 
    // If left child is larger than root
    if (l < n && mArray[l] > mArray[largest])
        largest = l;
 
    /*
    * If right child is larger than largest
    * so far
    */
    if (r < n && mArray[r] > mArray[largest])
        largest = r;
 
    // If largest is not root
    if (largest != i) {
        std::swap(mArray[i], mArray[largest]);
        
        mSwapCount++;
        if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
            // Handle events every few iterations
            handleEvents();
            if(!mIsRunning)
                return;
            if(mIsSoundEnabled)
                Mix_PlayChannel(-1, mSwapSound, 0);
            draw();
        }
 
        /*
        * Recursively heapify the affected
        * sub-tree
        */
        heapify(n, largest);
    }
}

void Visualizer::Engine::heapSort(){
    // Build heap (rearrange array)
    for (int i = mMAX_NUMBER / 2 - 1; i >= 0; i--)
        heapify(mMAX_NUMBER, i);
 
    /*
    * One by one extract an element
    * from heap
    */
    for (int i = mMAX_NUMBER - 1; i > 0; i--) {
 
        // Move current root to end
        std::swap(mArray[0], mArray[i]);
        
        mSwapCount++;
        if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
            // Handle events every few iterations
            handleEvents();
            if(!mIsRunning)
                return;
            if(mIsSoundEnabled)
                Mix_PlayChannel(-1, mSwapSound, 0);
            draw();
        }
 
        // call max heapify on the reduced heap
        heapify(i, 0);
    }
}

void Visualizer::Engine::mergeSort(int l, int r)
{
    if (l >= r)
        return; // Returns recursively
 
    int mid = l + (r - l) / 2;
    mergeSort(l, mid);
    mergeSort(mid + 1, r);
    merge(l, mid, r);
}

void Visualizer::Engine::merge(int left, int mid, int right){
    int subArrayOne = mid - left + 1;
    int subArrayTwo = right - mid;
 
    // Create temp arrays
    int *leftArray = new int[subArrayOne];
    int *rightArray = new int[subArrayTwo];
 
    // Copy data to temp arrays leftArray[] and rightArray[]
    for (int i = 0; i < subArrayOne; i++)
        leftArray[i] = mArray[left + i];
    for (int j = 0; j < subArrayTwo; j++)
        rightArray[j] = mArray[mid + 1 + j];
 
    int indexOfSubArrayOne = 0; // Initial index of first sub-array
    int indexOfSubArrayTwo = 0; // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array
 
    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        mSwapCount++;
        if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
            // Handle events every few iterations
            handleEvents();
            if(!mIsRunning)
                return;
            if(mIsSoundEnabled)
                Mix_PlayChannel(-1, mSwapSound, 0);
            draw();
        }
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) {
            mArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else {
            mArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    /*
    * Copy the remaining elements of
    * left[], if there are any
    */
    while (indexOfSubArrayOne < subArrayOne) {
        mArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        mSwapCount++;
        if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
            // Handle events every few iterations
            handleEvents();
            if(!mIsRunning)
                return;
            if(mIsSoundEnabled)
                Mix_PlayChannel(-1, mSwapSound, 0);
            draw();
        }

        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    /*
    * Copy the remaining elements of
    * right[], if there are any
    */
    while (indexOfSubArrayTwo < subArrayTwo) {
        mArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        mSwapCount++;
        if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
            // Handle events every few iterations
            handleEvents();
            if(!mIsRunning)
                return;
            if(mIsSoundEnabled)
                Mix_PlayChannel(-1, mSwapSound, 0);
            draw();
        }

        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

void Visualizer::Engine::selectionSort(){
    int i, j, min_idx;
    /*
    * One by one move boundary of
    * unsorted subarray
    */
    for (i = 0; i < mMAX_NUMBER - 1; i++)
    {
        /*
        * Find the minimum element in
        * unsorted array
        */
        min_idx = i;
        for (j = i + 1; j < mMAX_NUMBER; j++){
          if (mArray[j] < mArray[min_idx]){
            min_idx = j;
            mSwapCount++;
            if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                // Handle events every few iterations
                handleEvents();
                if(!mIsRunning)
                    return;
                if(mIsSoundEnabled)
                    Mix_PlayChannel(-1, mSwapSound, 0);
                draw();
            }
          }
        }
        /*
        * Swap the found minimum element
        * with the first element
        */
        if (min_idx != i){
            std::swap(mArray[min_idx], mArray[i]);
            mSwapCount++;
            if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                // Handle events every few iterations
                handleEvents();
                if(!mIsRunning)
                    return;
                if(mIsSoundEnabled)
                    Mix_PlayChannel(-1, mSwapSound, 0);
                draw();
            }
        }
    }
}

void Visualizer::Engine::insertionSort(){
    int i, key, j;
    for (i = 1; i < mMAX_NUMBER; i++){
        key = mArray[i];
        j = i - 1;
 
        /*
        * Move elements of arr[0..i-1], 
        * that are greater than key, to one
        * position ahead of their
        * current position
        */
        while (j >= 0 && mArray[j] > key){
            mArray[j + 1] = mArray[j];
            j = j - 1;

            mSwapCount++;
            if(mSwapCount % mUpdateFrequency == 0 && !mIsFastForward){
                // Handle events every few iterations
                handleEvents();
                if(!mIsRunning)
                    return;
                if(mIsSoundEnabled)
                    Mix_PlayChannel(-1, mSwapSound, 0);
                draw();
            }
        }
        mArray[j + 1] = key;
    }
}

void Visualizer::Engine::shuffle(){
    // Shuffle the array
    srand(time(NULL));

    for(int i = 0; i < mMAX_NUMBER; i++){
        int random = rand() % mMAX_NUMBER;
        std::swap(mArray[i], mArray[random]);
        // Draw the array every 10% of the way through the shuffle
        if(i % (mMAX_NUMBER / 10) == 0){
            if(mIsSoundEnabled)
                Mix_PlayChannel(-1, mSwapSound, 0);
            draw();
        }
    }

    //! Draw the array one last time (I MIGHT NEED TO ADD THIS TO THE SORTING ALGORITHMS AS WELL)
    draw();
    // The array is shuffled and not sorted
    mIsSorted = false;
    // Set the shuffle flag to false
    mRequestShuffle = false;
    // (Only for bubble sort for now), the resume index is reset
    mResumeIndex = 0;
    // Reset the swap count
    mSwapCount = 0;
}

void Visualizer::Engine::draw(){
    // Clear the screen
    SDL_RenderClear(mRenderer);

    // Render the sort name
    mTexture->render((mSize.x - mUsableWidth - mTexture->getWidth()) / 2, 20);

    // Render the info text
    mInfoTexture->render((mSize.x - mUsableWidth - mInfoTexture->getWidth()) / 2, mTexture->getHeight() + 40);

    // Render the array
    draw_rects();

    // Update the screen
    SDL_RenderPresent(mRenderer);
}

void Visualizer::Engine::draw_rects(){
    // Each element in the array is a rectangle
    SDL_FRect rect;

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
    for(int i = 0; i < mMAX_NUMBER; i++){
        // Set the color of each rectangle
        SDL_SetRenderDrawColor(mRenderer, startColorR + colorStepR * mArray[i], startColorG + colorStepG * mArray[i], startColorB + colorStepB * mArray[i], 0xFF);
        // Set the width of the rectangle to the width of the window divided by the number of elements in the array
        rect.w = (float) mUsableWidth / mMAX_NUMBER;
        // Set the height of the rectangle
        rect.h = ((float) mSize.y / mMAX_NUMBER) * mArray[i];
        // Set the x coordinate by multiplying the index by the width of the rectangle
        rect.x = i * rect.w + (mSize.x - mUsableWidth);
        // Set the y coordinate by subtracting the height of the rectangle from the height of the window
        rect.y = mSize.y - rect.h;

        // Draw the rectangle
        SDL_RenderFillRectF(mRenderer, &rect);
    }

    // Set background color
    SDL_SetRenderDrawColor(mRenderer, 0x4a, 0x18, 0xa8, 0xFF);
}