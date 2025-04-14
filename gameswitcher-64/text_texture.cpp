#include "text_texture.h"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "global.h"

TextTexture::TextTexture(std::string text, TTF_Font *font, SDL_Color color, 
    TextTextureAlignment alignment)
    : text_(text)
{
    // create surface
    SDL_Surface *surface = TTF_RenderUTF8_Blended(
        font,
        text.c_str(),
        color
    );
    
    // init width and height
    w_ = surface->w;
    h_ = surface->h;

    // create texture
    createTexture(surface);

    // free surface
	SDL_FreeSurface(surface);

    // compute render rect
    updateTargetRect(alignment);
} 

TextTexture::TextTexture(std::string text, TTF_Font *font, SDL_Color color, 
    TextTextureAlignment alignment, unsigned int wrapLength)
    : text_(text)
{
    // create surface
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        font,
        text.c_str(),
        color,
        wrapLength
    );

    // init width and height
    w_ = surface->w;
    h_ = surface->h;

    // create texture
    createTexture(surface);

    // free surface
	SDL_FreeSurface(surface);

    // compute render rect
    updateTargetRect(alignment);
} 

void TextTexture::createTexture(SDL_Surface *surface) {
   texture_ = SDLTextureUniquePtr {
        SDL_CreateTextureFromSurface(
            global::renderer,
            surface)
    };
    if (texture_ == nullptr)
        std::cerr << ("Texture creation failed") << std::endl;
}

void TextTexture::updateTargetRect(TextTextureAlignment alignment) {
    rect_ = {0, 0, 0, 0};
    rect_.w = w_;
    rect_.h = h_;

    switch (alignment) {
        case TextTextureAlignment::topCenter:
            rect_.x = (global::SCREEN_WIDTH - w_) / 2;
            rect_.y = 0;
        break;
        case TextTextureAlignment::topLeft:
            rect_.x = 0;
            rect_.y = 0;
        break;
        case TextTextureAlignment::topRight:
            rect_.x = global::SCREEN_WIDTH - w_;
            rect_.y = 0;          
        break;
        case TextTextureAlignment::bottomCenter:
            rect_.x = (global::SCREEN_WIDTH - w_) / 2;
            rect_.y = global::SCREEN_HEIGHT - h_;
        break;
        case TextTextureAlignment::bottomLeft:
            rect_.x = 0;
            rect_.y = global::SCREEN_HEIGHT - h_;
        break;
        case TextTextureAlignment::bottomRight:
            rect_.x = global::SCREEN_WIDTH - w_;
            rect_.y = global::SCREEN_HEIGHT - h_;
        break;
    }
}


void TextTexture::render() const {
    SDL_RenderCopyEx(global::renderer, 
        texture_.get(), 
        nullptr, 
        &rect_,
        global::ROTATION, nullptr, SDL_FLIP_NONE
    );
}

void TextTexture::render(int offsetX) const {
    auto rect = rect_;
    rect.x -= offsetX;

    SDL_RenderCopyEx(global::renderer, 
        texture_.get(), 
        nullptr, 
        &rect,
        global::ROTATION, nullptr, SDL_FLIP_NONE
    );
}

void TextTexture::scrollLeft(int offset) {
    rect_.x += offset;
}
