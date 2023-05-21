//
//  Engine.cpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include <sstream>

#include "Engine.hpp"

Visualizer::Engine::Engine(const COUPLE size)
    : mWindowSize(size), mUsableWidth(size.x - (size.x / 4)), mNumbersArray(gMAX_ELEMENTS[mCurrentElementsNumber])
{

    // Initialize the engine
    if (!init())
    {
        throw std::runtime_error("Failed to initialize!");
    }

    // Fill the array with numbers from 1 to gMAX_ELEMENTS[mCurrentElementsNumber]
    std::iota(mNumbersArray.begin(), mNumbersArray.end(), 1);

    // Fill the array with numbers from 1 to gMAX_ELEMENTS[mCurrentElementsNumber]
    // fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::Engine(const COUPLE size, const char *window_title)
    : mWindowSize(size), mWindowTitle(window_title), mUsableWidth(size.x - (size.x / 4)), mNumbersArray(gMAX_ELEMENTS[mCurrentElementsNumber])
{

    // Initialize the engine
    if (!init())
    {
        throw std::runtime_error("Failed to initialize!");
    }

    // Fill the array with numbers from 1 to gMAX_ELEMENTS[mCurrentElementsNumber]
    std::iota(mNumbersArray.begin(), mNumbersArray.end(), 1);

    // Fill the array with numbers from 1 to gMAX_ELEMENTS[mCurrentElementsNumber]
    // fill_array();
    // Shuffle the array
    shuffle();
}

Visualizer::Engine::~Engine()
{
    // Deallocate memory destroying the elements used by the textures
    mSortNameTexture->free();
    mInfoPanelTexture->free();
    mSpeedTexture->free();
    mSwapsTexture->free();
    mComparisonsTexture->free();

    // Close the fonts
    TTF_CloseFont(mRobotoSmall);
    TTF_CloseFont(mRobotoLarge);

    mRobotoSmall = NULL;
    mRobotoLarge = NULL;

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

void Visualizer::Engine::run()
{
    // Main loop flag
    while (mIsRunning)
    {
        // Handle events on queue
        handleEvents();

        // If the user requested a sort, sort the array
        if (mRequestSort)
        {
            sort();
        }

        // If the user requested a shuffle, shuffle the array
        if (mRequestShuffle)
        {
            shuffle();
        }

        // Clear the screen
        draw();
    }
}

bool Visualizer::Engine::init()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        printf("Warning: linear texture filtering not enabled!");
    }

    // Create the window
    mWindow = SDL_CreateWindow(mWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWindowSize.x, mWindowSize.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    // Check if the window was created
    if (mWindow == NULL)
    {
        printf("Window could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Create the renderer
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Check if the renderer was created
    if (mRenderer == NULL)
    {
        printf("Renderer could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    int fontSizeSmall = round(mWindowSize.y / 35.0);
    int fontSizeLarge = round(mWindowSize.y / 25.0);

    // Open the font used for the text
    mRobotoSmall = TTF_OpenFont("../res/Roboto-Regular.ttf", fontSizeSmall);

    // Check if the font was opened
    if (mRobotoSmall == NULL)
    {
        printf("Failed to load small font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Open the font used for the title text
    mRobotoLarge = TTF_OpenFont("../res/Roboto-Regular.ttf", fontSizeLarge);

    // Check if the font was opened
    if (mRobotoLarge == NULL)
    {
        printf("Failed to load small font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Create the texture used for the text
    mSortNameTexture = new LTexture(mRenderer, mRobotoLarge);
    // Load the text (starting with bubble sort by default)
    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
    // Create the texture used for the info text
    mInfoPanelTexture = new LTexture(mRenderer, mRobotoSmall);
    // Load the info text
    mInfoPanelTexture->loadFromRenderedText(gINFO_TEXT, gFontColor, true, 0, {0, 20, 0, 0});

    // Create the texture used for the speed text
    mSpeedTexture = new LTexture(mRenderer, mRobotoSmall);
    // Load the speed text
    std::stringstream speed_text;
    speed_text << " Speed: " << gSPEEDS[mCurrentDrawSpeed] << "x UP/DN";
    mSpeedTexture->loadFromRenderedText(speed_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());

    // Create the texture used for the swap text
    mSwapsTexture = new LTexture(mRenderer, mRobotoSmall);

    // Create the texture used for the swap text
    mComparisonsTexture = new LTexture(mRenderer, mRobotoSmall);

    // Create the texture used for the time text
    mTimeTexture = new LTexture(mRenderer, mRobotoSmall);

    // Create the texture used for the element number text
    mElementNumberTexture = new LTexture(mRenderer, mRobotoSmall);
    // Load the element number text
    std::stringstream element_number_text;
    element_number_text << " Elements: " << gMAX_ELEMENTS[mCurrentElementsNumber] << " J/K";
    mElementNumberTexture->loadFromRenderedText(element_number_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());

    // Set background color
    SDL_SetRenderDrawColor(mRenderer, gBackgroundColor.r, gBackgroundColor.g, gBackgroundColor.b, gBackgroundColor.a);
    return true;
}

void Visualizer::Engine::handleEvents()
{
    SDL_Event e;

    // Handle events on queue
    while (SDL_PollEvent(&e))
    {
        // User requests quit
        if (e.type == SDL_QUIT)
        {
            mIsRunning = false;
        }
        else if (e.type == SDL_WINDOWEVENT)
        {
            // Window resized
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                // Get the new window size
                mWindowSize.x = e.window.data1;
                mWindowSize.y = e.window.data2;
                // Resize the window
                SDL_SetWindowSize(mWindow, mWindowSize.x, mWindowSize.y);
                // Change usable area
                mUsableWidth = mWindowSize.x - (mWindowSize.x / 4);

                int fontSizeSmall = round(mWindowSize.y / 35.0);
                int fontSizeLarge = round(mWindowSize.y / 25.0);

                // Resize the texture
                mSortNameTexture->setFontSize(fontSizeLarge);
                mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);

                mInfoPanelTexture->setFontSize(fontSizeSmall);
                mInfoPanelTexture->loadFromRenderedText(gINFO_TEXT, gFontColor, true, 0, {0, 20, 0, 0});

                mSpeedTexture->setFontSize(fontSizeSmall);
                std::stringstream speed_text;
                speed_text << " Speed: " << gSPEEDS[mCurrentDrawSpeed] << "x UP/DN";
                mSpeedTexture->loadFromRenderedText(speed_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());

                mElementNumberTexture->setFontSize(fontSizeSmall);
                std::stringstream elements_text;
                elements_text << " Elements: " << gMAX_ELEMENTS[mCurrentElementsNumber] << "J/K";
                mElementNumberTexture->loadFromRenderedText(elements_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());

                mSwapsTexture->setFontSize(fontSizeSmall);
                std::stringstream swap_text;
                swap_text << " Swaps: " << mSwapsCount;
                mSwapsTexture->loadFromRenderedText(swap_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());

                mComparisonsTexture->setFontSize(fontSizeSmall);
                std::stringstream compare_text;
                compare_text << " Compare: " << mComparisonsCount;
                mComparisonsTexture->loadFromRenderedText(compare_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());

                mTimeTexture->setFontSize(fontSizeSmall);
                std::stringstream time_text;
                time_text << " Time: " << mElapsed << "ms";
                mTimeTexture->loadFromRenderedText(time_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());
            }
        }
        // User presses a key
        else if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
            // User presses escape
            case SDLK_ESCAPE:
                mIsRunning = false;
                break;
            // User presses the B key
            case SDLK_b:
                // If the array is not sorted and if the current sort is not already bubble sort
                if (!mRequestSort && mCurrentSort != BUBBLE_SORT)
                {
                    // Set the current sort to bubble sort
                    mCurrentSort = BUBBLE_SORT;
                    // Load the text for bubble sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the Q key
            case SDLK_q:
                // If the array is not sorted and if the current sort is not already quick sort
                if (!mRequestSort && mCurrentSort != QUICK_SORT)
                {
                    // Set the current sort to quick sort
                    mCurrentSort = QUICK_SORT;
                    // Load the text for quick sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the C key
            case SDLK_c:
                // If the array is not sorted and if the current sort is not already cocktail sort
                if (!mRequestSort && mCurrentSort != COCKTAIL_SORT)
                {
                    // Set the current sort to cocktail sort
                    mCurrentSort = COCKTAIL_SORT;
                    // Load the text for cocktail sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the E key
            case SDLK_e:
                // If the array is not sorted and if the current sort is not already shell sort
                if (!mRequestSort && mCurrentSort != SHELL_SORT)
                {
                    // Set the current sort to shell sort
                    mCurrentSort = SHELL_SORT;
                    // Load the text for shell sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the H key
            case SDLK_h:
                // If the array is not sorted and if the current sort is not already heap sort
                if (!mRequestSort && mCurrentSort != HEAP_SORT)
                {
                    // Set the current sort to heap sort
                    mCurrentSort = HEAP_SORT;
                    // Load the text for heap sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the M key
            case SDLK_m:
                // If the array is not sorted and if the current sort is not already merge sort
                if (!mRequestSort && mCurrentSort != MERGE_SORT)
                {
                    // Set the current sort to merge sort
                    mCurrentSort = MERGE_SORT;
                    // Load the text for merge sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the L key
            case SDLK_l:
                // If the array is not sorted and if the current sort is not already selection sort
                if (!mRequestSort && mCurrentSort != SELECTION_SORT)
                {
                    // Set the current sort to selection sort
                    mCurrentSort = SELECTION_SORT;
                    // Load the text for selection sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the I key
            case SDLK_i:
                // If the array is not sorted and if the current sort is not already insertion sort
                if (!mRequestSort && mCurrentSort != INSERTION_SORT)
                {
                    // Set the current sort to insertion sort
                    mCurrentSort = INSERTION_SORT;
                    // Load the text for insertion sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the G key
            case SDLK_g:
                // If the array is not sorted and if the current sort is not already gnome sort
                if (!mRequestSort && mCurrentSort != GNOME_SORT)
                {
                    // Set the current sort to gnome sort
                    mCurrentSort = GNOME_SORT;
                    // Load the text for gnome sort
                    mSortNameTexture->loadFromRenderedText(gSORT_NAMES[mCurrentSort], gFontColor);
                }
                break;
            // User presses the SPACEBAR key
            case SDLK_SPACE:
                // If the array is not sorted
                if (!mIsSorted && !mRequestSort)
                {
                    // Stops and starts the sort
                    mRequestSort = true;
                }
                else if (mRequestSort)
                {
                    // Fast forwards the sort
                    mIsFastForward = true;
                }
                break;
            // User presses the S key
            case SDLK_s:
                // If the array is sorted
                if (mIsSorted)
                {
                    // Shuffles the array
                    mRequestShuffle = true;
                }
                break;
            // User presses the down arrow key
            case SDLK_DOWN:
                if (mCurrentDrawSpeed > 0)
                {
                    // Decreases the speed
                    mCurrentDrawSpeed--;
                    std::stringstream ss;
                    ss << " Speed: " << gSPEEDS[mCurrentDrawSpeed] << "x UP/DN";
                    mSpeedTexture->loadFromRenderedText(ss.str(), gFontColor, false, mInfoPanelTexture->getWidth());
                    if(mRequestSort)
                    {
                        mHasSpeedChanged = true;
                    }
                }
                break;
            // User presses the up arrow key
            case SDLK_UP:
                if (mCurrentDrawSpeed < 5)
                {
                    // Decreases the speed
                    mCurrentDrawSpeed++;
                    std::stringstream ss;
                    ss << " Speed: " << gSPEEDS[mCurrentDrawSpeed] << "x UP/DN";
                    mSpeedTexture->loadFromRenderedText(ss.str(), gFontColor, false, mInfoPanelTexture->getWidth());
                    if(mRequestSort)
                    {
                        mHasSpeedChanged = true;
                    }
                }
                break;
            // User presses the J key
            case SDLK_j:
                if (mCurrentElementsNumber > 0 && !mRequestSort)
                {
                    // Decreases the speed
                    mCurrentElementsNumber--;
                    std::stringstream en;
                    en << " Elements: " << gMAX_ELEMENTS[mCurrentElementsNumber] << " J/K";
                    mElementNumberTexture->loadFromRenderedText(en.str(), gFontColor, false, mInfoPanelTexture->getWidth());
                    mNumbersArray.resize(gMAX_ELEMENTS[mCurrentElementsNumber]);
                    std::iota(mNumbersArray.begin(), mNumbersArray.end(), 1);
                    shuffle();
                }
                break;
            // User presses the K key
            case SDLK_k:
                if (mCurrentElementsNumber < 8 && !mRequestSort)
                {
                    // Decreases the speed
                    mCurrentElementsNumber++;
                    std::stringstream en;
                    en << " Elements: " << gMAX_ELEMENTS[mCurrentElementsNumber] << " J/K";
                    mElementNumberTexture->loadFromRenderedText(en.str(), gFontColor, false, mInfoPanelTexture->getWidth());
                    mNumbersArray.resize(gMAX_ELEMENTS[mCurrentElementsNumber]);
                    std::iota(mNumbersArray.begin(), mNumbersArray.end(), 1);
                    shuffle();
                }
                break;
            }
        }
    }
}

void Visualizer::Engine::sort()
{
    // Start the timer
    mStart = std::chrono::high_resolution_clock::now();
    // Sorts the array based on the current sort selected
    switch (mCurrentSort)
    {
    case BUBBLE_SORT:
        bubbleSort();
        break;
    case QUICK_SORT:
        quickSort(0, gMAX_ELEMENTS[mCurrentElementsNumber] - 1);
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
        mergeSort(0, gMAX_ELEMENTS[mCurrentElementsNumber] - 1);
        break;
    case SELECTION_SORT:
        selectionSort();
        break;
    case INSERTION_SORT:
        insertionSort();
        break;
    case GNOME_SORT:
        gnomeSort();
        break;
    }

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();
    mElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
    // The array is sorted
    mIsSorted = true;
    // The request is stopped
    mRequestSort = false;
    // Reset the swap element
    mSwapElement = -1;
}

void Visualizer::Engine::cocktailSort()
{
    bool swapped = true;
    int start = 0;
    int end = gMAX_ELEMENTS[mCurrentElementsNumber] - 1;

    while (swapped)
    {
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
        for (int i = start; i < end; ++i)
        {
            mComparisonsCount++;
            if (mNumbersArray[i] > mNumbersArray[i + 1])
            {
                std::swap(mNumbersArray[i], mNumbersArray[i + 1]);
                mSwapsCount++;
                if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
                {
                    // Handle events every few iterations
                    handleEvents();
                    if (!mIsRunning)
                    {
                        return;
                    }
                    mSwapElement = i + 1;
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
        for (int i = end - 1; i >= start; --i)
        {
            mComparisonsCount++;
            if (mNumbersArray[i] > mNumbersArray[i + 1])
            {
                std::swap(mNumbersArray[i], mNumbersArray[i + 1]);
                mSwapsCount++;
                if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
                {
                    // Handle events every few iterations
                    handleEvents();
                    if (!mIsRunning)
                    {
                        return;
                    }
                    mSwapElement = i;
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

void Visualizer::Engine::quickSort(int low, int high)
{
    if (low < high)
    {
        int pi = partition(low, high);

        // If the sort is stopped or the application is closed while inside of partition, stop the sort
        if (!mIsRunning)
        {
            return;
        }

        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }
}

int Visualizer::Engine::partition(int low, int high)
{
    // Handle events time partition is called
    handleEvents();
    if (!mIsRunning)
    {
        return -1;
    }
    int pivot = mNumbersArray[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (mNumbersArray[j] < pivot)
        {
            i++;
            std::swap(mNumbersArray[i], mNumbersArray[j]);
            mSwapsCount++;
            if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
            {
                mSwapElement = j;
                draw();
            }
        }
    }
    std::swap(mNumbersArray[i + 1], mNumbersArray[high]);
    mSwapsCount++;
    // If fast forward is enabled, draw the array on the screen immediately
    if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] && !mIsFastForward)
    {
        mSwapElement = high;
        draw();
    }
    return (i + 1);
}

void Visualizer::Engine::bubbleSort()
{
    bool swapped;
    for (int i = 0; i < gMAX_ELEMENTS[mCurrentElementsNumber] - 1; i++)
    {
        swapped = false;
        for (int j = 0; j < gMAX_ELEMENTS[mCurrentElementsNumber] - i - 1; j++)
        {
            mComparisonsCount++;
            if (mNumbersArray[j] > mNumbersArray[j + 1])
            {
                std::swap(mNumbersArray[j], mNumbersArray[j + 1]);
                swapped = true;
                mSwapsCount++;
                if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
                {
                    // Handle events every few iterations
                    handleEvents();
                    if (!mIsRunning)
                    {
                        return;
                    }
                    mSwapElement = j + 1;
                    draw();
                }
            }
        }

        // If no two elements were swapped in the inner loop, the array is already sorted
        if (!swapped)
        {
            break;
        }
    }
}

void Visualizer::Engine::shellSort()
{
    // Start with a big gap, then reduce the gap
    for (int gap = gMAX_ELEMENTS[mCurrentElementsNumber] / 2; gap > 0; gap /= 2)
    {
        /*
         * Do a gapped insertion sort for this gap size.
         * The first gap elements a[0..gap-1] are already in gapped order
         * keep adding one more element until the entire array is
         * gap sorted
         */
        for (int i = gap; i < gMAX_ELEMENTS[mCurrentElementsNumber]; i += 1)
        {
            /*
             * Add a[i] to the elements that have been gap sorted
             * save a[i] in temp and make a hole at position i
             */
            int temp = mNumbersArray[i];

            /*
             * shift earlier gap-sorted elements up until the correct
             * location for a[i] is found
             */
            int j;
            for (j = i; j >= gap && mNumbersArray[j - gap] > temp; j -= gap)
            {
                mNumbersArray[j] = mNumbersArray[j - gap];
                mSwapsCount++;
                if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
                {
                    // Handle events every few iterations
                    handleEvents();
                    if (!mIsRunning)
                    {
                        return;
                    }
                    draw();
                }
            }

            //  put temp (the original a[i]) in its correct location
            mNumbersArray[j] = temp;
            mSwapsCount++;
            if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
            {
                // Handle events every few iterations
                handleEvents();
                if (!mIsRunning)
                {
                    return;
                }
                draw();
            }
        }
    }
}

void Visualizer::Engine::heapify(int n, int i)
{
    // Initialize largest as root
    int largest = i;

    // left = 2*i + 1
    int l = 2 * i + 1;

    // right = 2*i + 2
    int r = 2 * i + 2;

    // If left child is larger than root
    if (l < n && mNumbersArray[l] > mNumbersArray[largest])
        largest = l;

    /*
     * If right child is larger than largest
     * so far
     */
    if (r < n && mNumbersArray[r] > mNumbersArray[largest])
        largest = r;

    // If largest is not root
    if (largest != i)
    {
        std::swap(mNumbersArray[i], mNumbersArray[largest]);

        mSwapsCount++;
        if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
        {
            // Handle events every few iterations
            handleEvents();
            if (!mIsRunning)
            {
                return;
            }
            draw();
        }

        /*
         * Recursively heapify the affected
         * sub-tree
         */
        heapify(n, largest);
    }
}

void Visualizer::Engine::heapSort()
{
    // Build heap (rearrange array)
    for (int i = gMAX_ELEMENTS[mCurrentElementsNumber] / 2 - 1; i >= 0; i--)
        heapify(gMAX_ELEMENTS[mCurrentElementsNumber], i);

    /*
     * One by one extract an element
     * from heap
     */
    for (int i = gMAX_ELEMENTS[mCurrentElementsNumber] - 1; i > 0; i--)
    {

        // Move current root to end
        std::swap(mNumbersArray[0], mNumbersArray[i]);

        mSwapsCount++;
        if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
        {
            // Handle events every few iterations
            handleEvents();
            if (!mIsRunning)
            {
                return;
            }
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

void Visualizer::Engine::merge(int left, int mid, int right)
{
    int subArrayOne = mid - left + 1;
    int subArrayTwo = right - mid;

    // Create temp arrays
    int *leftArray = new int[subArrayOne];
    int *rightArray = new int[subArrayTwo];

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (int i = 0; i < subArrayOne; i++)
        leftArray[i] = mNumbersArray[left + i];
    for (int j = 0; j < subArrayTwo; j++)
        rightArray[j] = mNumbersArray[mid + 1 + j];

    int indexOfSubArrayOne = 0;    // Initial index of first sub-array
    int indexOfSubArrayTwo = 0;    // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo)
    {
        mSwapsCount++;
        if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
        {
            // Handle events every few iterations
            handleEvents();
            if (!mIsRunning)
            {
                return;
            }
            draw();
        }
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo])
        {
            mNumbersArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else
        {
            mNumbersArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    /*
     * Copy the remaining elements of
     * left[], if there are any
     */
    while (indexOfSubArrayOne < subArrayOne)
    {
        mNumbersArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        mSwapsCount++;
        if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
        {
            // Handle events every few iterations
            handleEvents();
            if (!mIsRunning)
            {
                return;
            }
            draw();
        }

        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    /*
     * Copy the remaining elements of
     * right[], if there are any
     */
    while (indexOfSubArrayTwo < subArrayTwo)
    {
        mNumbersArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        mSwapsCount++;
        if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
        {
            // Handle events every few iterations
            handleEvents();
            if (!mIsRunning)
            {
                return;
            }
            draw();
        }

        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

void Visualizer::Engine::selectionSort()
{
    int i, j, min_idx;
    /*
     * One by one move boundary of
     * unsorted subarray
     */
    for (i = 0; i < gMAX_ELEMENTS[mCurrentElementsNumber] - 1; i++)
    {
        /*
         * Find the minimum element in
         * unsorted array
         */
        min_idx = i;
        for (j = i + 1; j < gMAX_ELEMENTS[mCurrentElementsNumber]; j++)
        {

            mComparisonsCount++;
            if (mNumbersArray[j] < mNumbersArray[min_idx])
            {
                min_idx = j;
                mSwapsCount++;
                if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
                {
                    // Handle events every few iterations
                    handleEvents();
                    if (!mIsRunning)
                    {
                        return;
                    }
                    draw();
                }
            }
        }
        /*
         * Swap the found minimum element
         * with the first element
         */
        mComparisonsCount++;
        if (min_idx != i)
        {
            std::swap(mNumbersArray[min_idx], mNumbersArray[i]);
            mSwapsCount++;
            if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
            {
                // Handle events every few iterations
                handleEvents();
                if (!mIsRunning)
                {
                    return;
                }
                draw();
            }
        }
    }
}

void Visualizer::Engine::insertionSort()
{
    int i, key, j;
    for (i = 1; i < gMAX_ELEMENTS[mCurrentElementsNumber]; i++)
    {
        key = mNumbersArray[i];
        j = i - 1;

        /*
         * Move elements of arr[0..i-1],
         * that are greater than key, to one
         * position ahead of their
         * current position
         */
        while (j >= 0 && mNumbersArray[j] > key)
        {
            mComparisonsCount++;
            mNumbersArray[j + 1] = mNumbersArray[j];
            j = j - 1;

            mSwapsCount++;
            if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
            {
                // Handle events every few iterations
                handleEvents();
                if (!mIsRunning)
                {
                    return;
                }
                mSwapElement = j;
                draw();
            }
        }
        mNumbersArray[j + 1] = key;
    }
}

void Visualizer::Engine::gnomeSort(){
    int index = 0;

    while (index < gMAX_ELEMENTS[mCurrentElementsNumber]) {
        mComparisonsCount++;
        if (index == 0)
            index++;
        if (mNumbersArray[index] >= mNumbersArray[index - 1])
            index++;
        else {
            std::swap(mNumbersArray[index], mNumbersArray[index - 1]);
            index--;
            mSwapsCount++;
            if (mSwapsCount % gSPEEDS[mCurrentDrawSpeed] == 0 && !mIsFastForward)
            {
                // Handle events every few iterations
                handleEvents();
                if (!mIsRunning)
                {
                    return;
                }
                mSwapElement = index;
                draw();
            }
        }
    }
}

void Visualizer::Engine::shuffle()
{
    // Shuffle the array
    srand(time(NULL));

    for (int i = 0; i < gMAX_ELEMENTS[mCurrentElementsNumber]; i++)
    {
        int random = rand() % gMAX_ELEMENTS[mCurrentElementsNumber];
        std::swap(mNumbersArray[i], mNumbersArray[random]);
        // Draw the array every 10% of the way through the shuffle
        if (i % (gMAX_ELEMENTS[mCurrentElementsNumber] / 10) == 0)
            draw();
    }

    //! Draw the array one last time (I MIGHT NEED TO ADD THIS TO THE SORTING ALGORITHMS AS WELL)
    draw();
    // The array is shuffled and not sorted
    mIsSorted = false;
    // Set the shuffle flag to false
    mRequestShuffle = false;
    // Reset the swap count
    mSwapsCount = 0;
    // Reset the comparisons count
    mComparisonsCount = 0;
    // The fast forward flag is reset
    mIsFastForward = false;
    // Set time to 0
    mElapsed = 0.000;
    // Reset the speed change flag
    mHasSpeedChanged = false;
}

void Visualizer::Engine::draw()
{
    // Clear the screen
    SDL_RenderClear(mRenderer);

    int spacing = mWindowSize.y / 30;

    // Render the sort name
    mSortNameTexture->render((mWindowSize.x - mUsableWidth - mSortNameTexture->getWidth()) / 2, spacing);

    spacing += mSortNameTexture->getHeight() + mWindowSize.y / 15;

    // Render the info text
    mInfoPanelTexture->render((mWindowSize.x - mUsableWidth - mInfoPanelTexture->getWidth()) / 2, spacing);

    spacing += mInfoPanelTexture->getHeight();

    // Render the speed text
    mSpeedTexture->render((mWindowSize.x - mUsableWidth - mInfoPanelTexture->getWidth()) / 2, spacing);

    spacing += mSpeedTexture->getHeight();

    // Render the elements number text
    mElementNumberTexture->render((mWindowSize.x - mUsableWidth - mInfoPanelTexture->getWidth()) / 2, spacing);

    spacing += mElementNumberTexture->getHeight();

    // Update the swap text
    std::stringstream swap_text;
    swap_text << " Swaps: " << mSwapsCount;
    mSwapsTexture->loadFromRenderedText(swap_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());
    // Render the swap text
    mSwapsTexture->render((mWindowSize.x - mUsableWidth - mInfoPanelTexture->getWidth()) / 2, spacing);

    spacing += mSwapsTexture->getHeight();

    // Update the comparisons text
    std::stringstream compare_text;
    compare_text << " Compare: " << mComparisonsCount;
    mComparisonsTexture->loadFromRenderedText(compare_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());
    // Render the comparisons text
    mComparisonsTexture->render((mWindowSize.x - mUsableWidth - mInfoPanelTexture->getWidth()) / 2, spacing);

    spacing += mElementNumberTexture->getHeight();

    // Update time
    if(mRequestSort)
    {
        auto end = std::chrono::high_resolution_clock::now();
        mElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
    }

    // Update the time text
    std::stringstream time_text;
    if(!mIsFastForward && !mHasSpeedChanged)
        time_text << " Time: " << mElapsed / 1000.0 << "s";
    else if(mHasSpeedChanged)
        time_text << " Time: " << "Sped UP/DN";
    else
        time_text << " Time: " << "Skipped";

    mTimeTexture->loadFromRenderedText(time_text.str(), gFontColor, false, mInfoPanelTexture->getWidth());
    // Render the time text
    mTimeTexture->render((mWindowSize.x - mUsableWidth - mInfoPanelTexture->getWidth()) / 2, spacing);

    // Render the array
    draw_rects();

    // Update the screen
    SDL_RenderPresent(mRenderer);
}

void Visualizer::Engine::draw_rects()
{
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
    double colorStepR = (endColorR - startColorR) / (double)gMAX_ELEMENTS[mCurrentElementsNumber];
    double colorStepG = (endColorG - startColorG) / (double)gMAX_ELEMENTS[mCurrentElementsNumber];
    double colorStepB = (endColorB - startColorB) / (double)gMAX_ELEMENTS[mCurrentElementsNumber];

    // Draw the rectangles
    for (int i = 0; i < gMAX_ELEMENTS[mCurrentElementsNumber]; i++)
    {
        // Set the color of each rectangle (red if it's the element being swapped)
        if(mSwapElement == i)
            SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
        else  
            SDL_SetRenderDrawColor(mRenderer, startColorR + colorStepR * mNumbersArray[i], startColorG + colorStepG * mNumbersArray[i], startColorB + colorStepB * mNumbersArray[i], 0xFF);
        // Set the width of the rectangle to the width of the window divided by the number of elements in the array
        rect.w = (float)mUsableWidth / gMAX_ELEMENTS[mCurrentElementsNumber];
        // Set the height of the rectangle
        rect.h = ((float)mWindowSize.y / gMAX_ELEMENTS[mCurrentElementsNumber]) * mNumbersArray[i];
        // Set the x coordinate by multiplying the index by the width of the rectangle
        rect.x = i * rect.w + (mWindowSize.x - mUsableWidth);
        // Set the y coordinate by subtracting the height of the rectangle from the height of the window
        rect.y = mWindowSize.y - rect.h;

        // Draw the rectangle
        SDL_RenderFillRectF(mRenderer, &rect);
    }

    // Set background color
    SDL_SetRenderDrawColor(mRenderer, gBackgroundColor.r, gBackgroundColor.g, gBackgroundColor.b, gBackgroundColor.a);
}