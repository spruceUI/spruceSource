#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <SDL.h>

namespace global
{
    static int SCREEN_WIDTH = 640;
    static int SCREEN_HEIGHT = 480;

    static int ROTATION = 0;

    extern SDL_Renderer *renderer;

} // namespace constants

#endif // GLOBAL_H_
