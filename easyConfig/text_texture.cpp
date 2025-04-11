#include "text_texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

TextTexture::TextTexture(const string & text, TTF_Font *font, SDL_Color color, 
    TextureAlignment alignment)
    : text_(text)
{
    // create surface
    SDL_Surface *surface = TTF_RenderUTF8_Blended(
        font,
        text.c_str(),
        color
    );
    
    init(surface, alignment, 0.5);

    // free surface
	SDL_FreeSurface(surface);
} 

TextTexture::TextTexture(const string & text, TTF_Font *font, SDL_Color color, 
    TextureAlignment alignment, unsigned int wrapLength)
    : text_(text)
{
    // create surface
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        font,
        text.c_str(),
        color,
        wrapLength
    );

    init(surface, alignment, 0.5);

    // free surface
	SDL_FreeSurface(surface);
} 

