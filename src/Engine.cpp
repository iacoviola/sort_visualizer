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

#include "Engine.hpp"
#include "couple.hpp"

Visualizer::Engine::Engine(Couple size, const int max_elements) 
: mSize(size), mMAX_NUMBER(max_elements) {
    
    if(!init()){
        printf("Failed to initialize!\n");
        throw std::runtime_error("Failed to initialize!");
    }
    fill_array();
    shuffle();
}

Visualizer::Engine::Engine(Couple size, const int max_elements, const char* window_title) 
: mSize(size), mMAX_NUMBER(max_elements), mWindowTitle(window_title) {
    
    if(!init()){
        printf("Failed to initialize!\n");
        throw std::runtime_error("Failed to initialize!");
    }
    fill_array();
    shuffle();
}

Visualizer::Engine::Engine(Couple size, const int max_elements, const char* window_title, int update_frequency) 
: mSize(size), mMAX_NUMBER(max_elements), mWindowTitle(window_title), mUpdateFrequency(update_frequency) {
    
    if(!init()){
        printf("Failed to initialize!\n");
        throw std::runtime_error("Failed to initialize!");
    }
    fill_array();
    shuffle();
}

Visualizer::Engine::~Engine(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    mRenderer = NULL;
    mWindow = NULL;

    SDL_Quit();
}

bool Visualizer::Engine::init(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
        printf("Warning: linear texture filtering not enabled!");
    }

    mWindow = SDL_CreateWindow(mWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSize.x, mSize.y, SDL_WINDOW_SHOWN);

    if(mWindow == NULL){
        printf("Window could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(mRenderer == NULL){
        printf("Renderer could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
    return true;
}

void Visualizer::Engine::run(){
    while(mIsRunning){
        handleEvents();

        if(mRequestSort){
            bubbleSort();
        }

        if(mRequestShuffle){
            shuffle();
        }

        draw();
    }
}

void Visualizer::Engine::handleEvents(){
    SDL_Event e;

    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT){
            mIsRunning = false;
        } else if (e.type == SDL_KEYDOWN){
            switch(e.key.keysym.sym){
                case SDLK_ESCAPE:
                    mIsRunning = false;
                    break;
                case SDLK_SPACE:
                    if(!mIsSorted){
                        mRequestSort = !mRequestSort;
                    }
                    break;
                case SDLK_s:
                    if(mIsSorted || mResumeIndex != 0){
                        mRequestShuffle = true;
                    }
                    break;
            }
        }
    }
}

void Visualizer::Engine::bubbleSort(){
    for (int i = mResumeIndex; i < mArray.size() - 1; i++){
        for (int j = 0; j < mArray.size() - i - 1; j++){
            handleEvents();
            if(!mIsRunning || !mRequestSort || mRequestShuffle){
                mResumeIndex = i;
                return;
            }

            if (mArray[j] > mArray[j+1]){
                std::swap(mArray[j], mArray[j+1]);
                if(j % mUpdateFrequency == 0)
                    draw();
            }
        }
    }

    mIsSorted = true;
    mRequestSort = false;
    mResumeIndex = 0;
}

void Visualizer::Engine::draw(){
    SDL_RenderClear(mRenderer);

    draw_rects();

    SDL_RenderPresent(mRenderer);
}

void Visualizer::Engine::shuffle(){
    srand(time(NULL));
    for(int i = 0; i < mArray.size(); i++){
        int random = rand() % mArray.size();
        std::swap(mArray[i], mArray[random]);
    }

    draw();
    mIsSorted = false;
    mRequestShuffle = false;
    mResumeIndex = 0;
}

void Visualizer::Engine::draw_rects(){
    SDL_Rect rect;

    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for(int i = 0; i < mArray.size(); i++){
        rect.w = mSize.x / mMAX_NUMBER;
        rect.h = ((double) mSize.y / mMAX_NUMBER) * mArray[i];
        rect.x = i * rect.w;
        rect.y = mSize.y - rect.h;

        SDL_RenderFillRect(mRenderer, &rect);
    }

    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
}

void Visualizer::Engine::fill_array(){
    for(int i = 1; i <= mMAX_NUMBER; i++){
        mArray.push_back(i);
    }
}