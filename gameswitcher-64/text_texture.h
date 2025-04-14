#ifndef TEXT_TEXTURE_H
#define TEXT_TEXTURE_H

#include <string>

#include "sdl_unique_ptr.h"

#include <SDL_ttf.h>

enum class TextTextureAlignment { topCenter, topLeft, topRight, bottomCenter, bottomLeft, bottomRight };

class TextTexture
{
public:
    explicit TextTexture(std::string text, TTF_Font *font, SDL_Color color, 
        TextTextureAlignment alignment);
    explicit TextTexture(std::string text, TTF_Font *font, SDL_Color color, 
        TextTextureAlignment alignment, unsigned int wrapLength);
    virtual ~TextTexture() = default;

    // disallow copying and assignment
    TextTexture(const TextTexture &) = delete;
    TextTexture &operator=(const TextTexture &) = delete;

    void updateTargetRect(TextTextureAlignment alignment);
    void render() const;
    void render(int offsetX) const;
    void scrollLeft(int offset);

    int getWidth() const { return w_; }
    int getHeight() const { return h_; }
    std::string getText() const { return text_; }
    SDL_Texture * getTexture() const { return texture_.get(); }

private:
    void createTexture(SDL_Surface *surface);

    std::string text_;
    int w_, h_;
    SDL_Rect rect_;
    SDLTextureUniquePtr texture_ = nullptr;
};

#endif // TEXT_TEXTURE_H