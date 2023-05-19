//
//  LTexture.cpp
//  SDL_Sort_Visualizer
//
//  Created by Emiliano Iacopini on 5/16/23.
//

#include "LTexture.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

LTexture::LTexture(SDL_Renderer *renderer)
    : renderer(renderer)
{
    if (font == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::LTexture(SDL_Renderer *renderer, TTF_Font *font)
    : renderer(renderer), font(font)
{
    if (font == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, bool wrappable)
{
    // Remove preexisting texture
    free();

    SDL_Surface *textSurface = NULL;

    // Render text surface
    if (wrappable)
    {
        textSurface = TTF_RenderText_LCD_Wrapped(font, textureText.c_str(), textColor, {0x69, 0x2b, 0xe0, 0xFF}, 0);
    }
    else
    {
        textSurface = TTF_RenderText_LCD(font, textureText.c_str(), textColor, {0x69, 0x2b, 0xe0, 0xFF});
    }

    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (mTexture == NULL)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Get image dimensions
    mWidth = textSurface->w;
    mHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    return true;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, int width)
{
    // Remove preexisting texture
    free();

    SDL_Surface *textSurface = NULL;
    SDL_Surface *newSurface = NULL;

    // Render text surface
    textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);

    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Render background surface
    newSurface = SDL_CreateRGBSurface(0, width, textSurface->h, 32, 0, 0, 0, 0);

    if (newSurface == NULL)
    {
        printf("Unable to create background surface! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Fill background surface with color
    SDL_FillRect(newSurface, NULL, SDL_MapRGB(newSurface->format, 0x69, 0x2b, 0xe0));

    // Blit text surface to background surface
    SDL_Rect textRect = {0, 0, 0, 0};
    SDL_BlitSurface(textSurface, NULL, newSurface, &textRect);

    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(renderer, newSurface);
    if (mTexture == NULL)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Get image dimensions
    mWidth = newSurface->w;
    mHeight = newSurface->h;

    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(newSurface);

    return true;
}

void LTexture::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    // Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

void LTexture::setFontSize(int size)
{
    TTF_SetFontSize(font, size);
}
