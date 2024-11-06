#include "image_texture.h"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using std::cerr, std::endl;

ImageTexture::ImageTexture(const string & filename, TextureAlignment alignment)
    : filename_(filename)
{
    // create surface
	SDL_Surface *surface = IMG_Load(filename.c_str());
    if (IMG_GetError() != nullptr && strcmp(IMG_GetError(), "") != 0) {
        cerr << "cannot load image: " << filename << endl;
        cerr << IMG_GetError() << endl;
        SDL_ClearError();
    }

    init(surface, alignment);

    // free surface
	SDL_FreeSurface(surface);
} 

