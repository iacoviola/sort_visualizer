//
//  LTexture.hpp
//  SDL_Keying
//
//  Created by Emiliano Iacopini on 3/12/23.
//

#ifndef LTexture_hpp
#define LTexture_hpp

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//Texture wrapper class
class LTexture
{
    public:
        //Initializes variables
        LTexture(SDL_Renderer* renderer);
        //Initializes variables
        LTexture(SDL_Renderer* renderer, TTF_Font* font = NULL);

        //Deallocates memory
        ~LTexture();

        //Loads image at specified path
        bool loadFromFile(std::string path);

        //Creates image from font string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor, bool wrappable = false);

        //Deallocates texture
        void free();

        //Renders texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	
        //set color modulation
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        //set blending
        void setBlendMode(SDL_BlendMode blending);

        //set alpha modulation
        void setAlpha(Uint8 alpha);

        //Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        //The actual hardware texture
        SDL_Texture* mTexture;
        SDL_Renderer* renderer;
        TTF_Font* font;

        //Image dimensions
        int mWidth;
        int mHeight;
};

#endif /* LTexture_hpp */
