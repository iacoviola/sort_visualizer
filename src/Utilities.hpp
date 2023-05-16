//
//  couple.hpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#ifndef couple_hpp
#define couple_hpp
/*!
* @brief A struct that represents a couple of integers used to represent the size of the window
*/
struct COUPLE {
    int x;
    int y;
};

/*!
* @brief Enum that represents the sorting algorithms available
*/
enum SORT_IDENTIFIER {
    BUBBLE_SORT,
    QUICK_SORT,
    COCKTAIL_SORT,
    SHELL_SORT,
    HEAP_SORT
};

#endif /* couple_hpp */