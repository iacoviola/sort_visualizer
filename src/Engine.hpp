//
//  Engine.hpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include "Utilities.hpp"
#include "LTexture.hpp"

namespace Visualizer
{
    class Engine
    {
    public:
        /*!
         * @brief Engine constructor
         * @param size The size of the window
         * @param max_elements The maximum number of elements in the array
         */
        Engine(const COUPLE size, const int max_elements);

        /*!
         * @brief Engine constructor
         * @param size The size of the window
         * @param max_elements The maximum number of elements in the array
         * @param window_title The title of the window
         */
        Engine(const COUPLE size, const int max_elements, const char *window_title);

        /*!
         * @brief Engine destructor
         */
        ~Engine();

        /*!
         * @brief Starts the engine
         */
        void run();

    private:
        COUPLE mWindowSize;            /*! The size of the window */
        const int mMAX_ELEMENTS; /*! The maximum number of elements in the array */

        bool mIsRunning = true;       /*! Whether the engine is running or not (esc or closing the app make this false)*/
        bool mRequestSort = false;    /*! Whether the user requested a sort or not (spacebar was pressed)*/
        bool mRequestShuffle = false; /*! Whether the user requested a shuffle or not (s was pressed)*/
        bool mIsSorted = false;       /*! Whether the array is sorted or not (end of sort)*/
        bool mIsFastForward = false;  /*! Whether the sort is fast forwarded or not (f was pressed while sorting)*/

        std::string mWindowTitle = "SDL Sort Visualizer"; /*! The title of the window */

        int mSwapsCount = 0;       /*! The number of swaps performed during the sort */
        int mComparisonsCount = 0; /*! The number of comparisons performed during the sort */

        int mCurrentDrawSpeed = 3; /*! The currently selected drawing speed */

        SORT_IDENTIFIER mCurrentSort = BUBBLE_SORT; /*! The currently selected sort algorithm */

        SDL_Renderer *mRenderer = NULL; /*! The main renderer */
        SDL_Window *mWindow = NULL;     /*! The main window */

        LTexture *mSortNameTexture;    /*! The texture used to draw the text */
        LTexture *mInfoPanelTexture;   /*! The texture used to draw the info text */
        LTexture *mSpeedTexture;       /*! The texture used to draw the speed text */
        LTexture *mSwapsTexture;       /*! The texture used to draw the swap count text */
        LTexture *mComparisonsTexture; /*! The texture used to draw the comparison count text */

        TTF_Font *mRobotoSmall; /*! The font used to draw the text */
        TTF_Font *mRobotoLarge; /*! The font used to draw the text */

        int mFontSizeSmall; /*! The size of the small font */
        int mFontSizeLarge; /*! The size of the large font */

        int mUsableWidth; /*! The usable width of the window */

        std::vector<int> mNumbersArray; /*! The array to be sorted */

        /*!
         * @brief Initializes the engine
         * @return true if the engine was initialized successfully, false otherwise
         */
        bool init();

        /*!
         * @brief Handles user input
         */
        void handleEvents();

        /*!
         * @brief Sorts the array using the currently selected algorithm
         */
        void sort();

        /*!
         * @brief Sorts the array using cocktail sort
         */
        void cocktailSort();

        /*!
         * @brief Sorts the array using quick sort
         * @param low The lower bound of the array
         * @param high The upper bound of the array
         */
        void quickSort(int low, int high);

        /*!
         * @brief Utility function used by quick sort
         * @param low The lower bound of the array
         * @param high The upper bound of the array
         */
        int partition(int low, int high);

        /*!
         * @brief Sorts the array using bubble sort
         */
        void bubbleSort();

        /*!
         * @brief Sorts the array using shell sort
         */
        void shellSort();

        /*!
         * @brief Utility function used by heap sort
         * @param n The size of the heap
         * @param i The index of the array root
         */
        void heapify(int n, int i);

        /*!
         * @brief Sorts the array using heap sort
         */
        void heapSort();

        /*!
         * @brief Utility function used by merge sort
         * @param l The lower bound of the array
         * @param r The upper bound of the array
         * @param m The middle index of the array
         */
        void merge(int l, int m, int r);

        /*!
         * @brief Sorts the array using merge sort
         * @param l The lower bound of the array
         * @param r The upper bound of the array
         */
        void mergeSort(int l, int r);

        /*
         * @brief Sorts the array using selection sort
         */
        void selectionSort();

        /*!
         * @brief Sorts the array using insertion sort
         */
        void insertionSort();

        /*!
         * @brief Shuffles the array using the Fisher-Yates algorithm
         */
        void shuffle();

        /*!
         * @brief Draws the elements to the screen
         */
        void draw();

        /*!
         * @brief Draws the array elements to the screen as rectangles
         */
        void draw_rects();
    };
}

#endif /* Engine_hpp */