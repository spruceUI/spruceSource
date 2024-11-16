#ifndef SDL_UNIQUE_PRT_H_
#define SDL_UNIQUE_PRT_H_

#include <memory>
#include <type_traits>

#include <SDL.h>

/**
 * @brief Deletes the SDL surface using `SDL_FreeSurface`.
 */
struct SDLSurfaceDeleter {
    void operator()(SDL_Surface *surface) const { SDL_FreeSurface(surface); }
};

/**
 * @brief std::unique_ptr specializations for SDL surface.
 */
using SDLSurfaceUniquePtr = std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>;

/**
 * @brief Deletes the SDL surface using `SDL_FreeSurface`.
 */
struct SDLTextureDeleter {
    void operator()(SDL_Texture *texture) const { SDL_DestroyTexture(texture); }
};

/**
 * @brief std::unique_ptr specializations for SDL texture.
 */
using SDLTextureUniquePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;


/**
 * @brief Deletes the object using `SDL_free`.
 */
template <typename T> struct SDLFreeDeleter {
    static_assert(!std::is_same<T, SDL_Surface>::value,
        "SDL_Surface should use SDLSurfaceUniquePtr instead.");

    void operator()(T *obj) const { SDL_free(obj); }
};

/**
 * @brief A unique pointer to T that is deleted with SDL_free.
 */
template <typename T>
using SDLUniquePtr = std::unique_ptr<T, SDLFreeDeleter<T>>;

#endif // SDL_UNIQUE_PRT_H_
