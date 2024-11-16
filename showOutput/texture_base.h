#ifndef TEXTURE_BASE_H
#define TEXTURE_BASE_H

#include "sdl_unique_ptr.h"

enum class TextureAlignment { topCenter, topLeft, topRight, bottomCenter, bottomLeft, bottomRight, center };

class TextureBase
{
public:
    explicit TextureBase() {};
    explicit TextureBase(SDL_Surface *surface, TextureAlignment alignment);
    virtual ~TextureBase() = default;

    // disallow copying and assignment
    TextureBase(const TextureBase &) = delete;
    TextureBase &operator=(const TextureBase &) = delete;

    void init(SDL_Surface *surface, TextureAlignment alignment, double scale=1.0);
    void updateTargetRect(TextureAlignment alignment);
    void FitScreenSize(int marginX=0, int marginY=0);
    void render() const;
    void render(int offsetX, int offsetY) const;
    void scrollLeft(int offset);
    bool isInitialized() const { return isInitialized_; }

    int getWidth() const { return w_; }
    int getHeight() const { return h_; }
    SDL_Texture * getTexture() const { return texture_.get(); }

private:
    void createTexture(SDL_Surface *surface);

    bool isInitialized_ = false; 
    int w_, h_;
    SDL_Rect rect_;
    SDLTextureUniquePtr texture_ = nullptr;
    TextureAlignment alignment_;
};

#endif // TEXTURE_BASE_H
