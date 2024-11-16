#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <string>
#include <map>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace global
{
    const int SCREEN_WIDTH = 480;
    const int SCREEN_HEIGHT = 640;

    extern SDL_Renderer *renderer;
    extern TTF_Font *font;
    extern SDL_Color text_color;
    extern SDL_Color minor_text_color;

} // namespace constants

#endif // GLOBAL_H_
