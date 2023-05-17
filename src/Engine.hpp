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

namespace Visualizer {
    class Engine{
        public:
            /*!
            * @brief Engine constructor
            * @param size The size of the window
            * @param max_elements The maximum number of elements in the array
            */
            Engine(COUPLE size, const int max_elements);

            /*!
            * @brief Engine constructor
            * @param size The size of the window
            * @param max_elements The maximum number of elements in the array
            * @param window_title The title of the window
            */
            Engine(COUPLE size, const int max_elements, const char* window_title);

            /*!
            * @brief Engine constructor
            * @param size The size of the window
            * @param max_elements The maximum number of elements in the array
            * @param window_title The title of the window
            * @param update_frequency The frequency with which the window is updated when sorting
            */
            Engine(COUPLE size, const int max_elements, const char* window_title, int update_frequency);

            /*!
            * @brief Engine destructor
            */
            ~Engine();
            
            /*!
            * @brief Starts the engine
            */
            void run();

        private:
            COUPLE mSize; /*! The size of the window */
            const int mMAX_NUMBER; /*! The maximum number of elements in the array */

            bool mIsRunning = true; /*! Whether the engine is running or not (esc or closing the app make this false)*/
            bool mRequestSort = false; /*! Whether the user requested a sort or not (spacebar was pressed)*/
            bool mRequestShuffle = false; /*! Whether the user requested a shuffle or not (s was pressed)*/
            bool mIsSorted = false; /*! Whether the array is sorted or not (end of sort)*/
            bool mIsFastForward = false; /*! Whether the sort is fast forwarded or not (f was pressed while sorting)*/

            std::string mWindowTitle = "SDL Sort Visualizer"; /*! The title of the window */
            int mSwapCount = 0; /*! The number of swaps performed during the sort */
            int mUpdateFrequency = 10; /*! The frequency with which the window is updated when sorting */
            int mResumeIndex = 0; /*! The index from which the sort should resume (used when the sort is stopped, only used for bubble sort for now) */
            SORT_IDENTIFIER mCurrentSort = BUBBLE_SORT; /*! The currently selected sort algorithm */

            SDL_Renderer* mRenderer = NULL; /*! The main renderer */
            SDL_Window* mWindow = NULL; /*! The main window */

            LTexture* mTexture; /*! The texture used to draw the text */
            LTexture* mInfoTexture; /*! The texture used to draw the info text */

            TTF_Font* mFontSmall; /*! The font used to draw the text */
            TTF_Font* mFontLarge; /*! The font used to draw the text */

            int mUsableWidth; /*! The usable width of the window */

            std::vector<int> mArray; /*! The array to be sorted */

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
            
            /*!
            * @brief Fills the array with numbers from 1 to mMAX_NUMBER
            */
            void fill_array();
    };
}

#endif /* Engine_hpp */