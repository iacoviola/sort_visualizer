//
//  couple.hpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#ifndef Utilities_hpp
#define Utilities_hpp

#include <string>

/*!
 * @brief A struct that represents a couple of integers used to represent the size of the window
 */
typedef struct COUPLE
{
    int x;
    int y;
} COUPLE;

/*!
 * @brief Enum that represents the sorting algorithms available
 */
enum SORT_IDENTIFIER
{
    BUBBLE_SORT,
    QUICK_SORT,
    COCKTAIL_SORT,
    SHELL_SORT,
    HEAP_SORT,
    MERGE_SORT,
    SELECTION_SORT,
    INSERTION_SORT,
    GNOME_SORT
};

// Strings used to represent the sorting algorithms
const std::string gSORT_NAMES[] = {
    "Bubble Sort",
    "Quick Sort",
    "Cocktail Sort",
    "Shell Sort",
    "Heap Sort",
    "Merge Sort",
    "Selection Sort",
    "Insertion Sort",
    "Gnome Sort"
};

// String used for the info text
const std::string gINFO_TEXT = " B - bubble sort \n E - shell sort \n Q - quick sort \n H - heap sort \n C - cocktail sort \n M - merge sort \n L - selection sort \n I - insertion sort \n G - gnome sort \n ------- \n Spacebar - start/ff \n S - shuffle \n ESC - quit \n ------- \n";
// Save the different speeds
const int gSPEEDS[] = {1, 5, 10, 20, 50, 100};

const SDL_Color gBackgroundColor = {0x4a, 0x18, 0xa8, 0xFF}; /*! The background color of the window */

const SDL_Color gFontColor = {0xFF, 0xFF, 0xFF, 0xFF}; /*! The font color of the text */

#endif /* Utilities_hpp */