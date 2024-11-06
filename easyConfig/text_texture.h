#ifndef TEXT_TEXTURE_H
#define TEXT_TEXTURE_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "texture_base.h"

using std::string;

class TextTexture: public TextureBase
{
public:
    explicit TextTexture(const string & text, TTF_Font *font, SDL_Color color, 
        TextureAlignment alignment = TextureAlignment::topLeft);
    explicit TextTexture(const string & text, TTF_Font *font, SDL_Color color, 
        TextureAlignment alignment, unsigned int wrapLength);
    virtual ~TextTexture() = default;

    // disallow copying and assignment
    TextTexture(const TextTexture &) = delete;
    TextTexture &operator=(const TextTexture &) = delete;

    const string & getText() const { return text_; }

private:

    const string text_;
};

#endif // TEXT_TEXTURE_H