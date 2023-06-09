//
//  main.cpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdexcept>

#include "Engine.hpp"

int main(int argc, char *argv[])
{
    try
    {
        Visualizer::Engine engine({800, 600}, "SDL Sort Visualizer");
        engine.run();
    }
    catch (std::runtime_error &e)
    {
        printf("%s\n", e.what());
        return -1;
    }
    return 0;
}
