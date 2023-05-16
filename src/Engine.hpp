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

#include "couple.hpp"

namespace Visualizer {
    class Engine{
        public:
            Engine(Couple size, const int max_elements);
            Engine(Couple size, const int max_elements, const char* window_title);
            Engine(Couple size, const int max_elements, const char* window_title, int update_frequency);
            ~Engine();
            
            void run();

        private:
            const Couple mSize;
            const int mMAX_NUMBER;

            bool mIsRunning = true;
            bool mRequestSort = false;
            bool mRequestShuffle = false;
            bool mIsSorted = false;
            std::string mWindowTitle = "SDL Sort Visualizer";
            int mUpdateFrequency = 10;
            int mResumeIndex = 0;

            SDL_Renderer* mRenderer = NULL;
            SDL_Window* mWindow = NULL;

            std::vector<int> mArray;

            bool init();
            void handleEvents();
            void bubbleSort();
            void draw();
            void shuffle();
            void draw_rects();
            void fill_array();
    };
}

#endif /* Engine_hpp */