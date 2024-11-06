#include "texture_base.h"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "global.h"

using std::cerr, std::endl;

TextureBase::TextureBase(SDL_Surface *surface, TextureAlignment alignment)
{
    init(surface, alignment);
}

void TextureBase::init(SDL_Surface *surface, TextureAlignment alignment, double scale) 
{
    alignment_ = alignment;

    // init width and height
    w_ = static_cast<int>(surface->w * scale);
    h_ = static_cast<int>(surface->h * scale);

    // create texture
    createTexture(surface);

    // compute render rect
    updateTargetRect(alignment);

    // set flag
    isInitialized_ = true;
}

void TextureBase::createTexture(SDL_Surface *surface) {
   texture_ = SDLTextureUniquePtr {
        SDL_CreateTextureFromSurface(
            global::renderer,
            surface)
    };
    if (texture_ == nullptr)
        cerr << ("Texture creation failed") << endl;

    SDL_SetTextureBlendMode(texture_.get(), SDL_BLENDMODE_BLEND);
}


void TextureBase::updateTargetRect(TextureAlignment alignment) {
    rect_ = {0, 0, 0, 0};
    rect_.w = w_;
    rect_.h = h_;
    switch (alignment) {
        case TextureAlignment::topCenter:
            rect_.x = -(w_ - h_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2;
        break;
        case TextureAlignment::topLeft:
            rect_.x = -(w_ - h_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2 + 
                (global::SCREEN_HEIGHT - w_) / 2;
        break;
        case TextureAlignment::topRight:
            rect_.x = -(w_ - h_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2 - 
                (global::SCREEN_HEIGHT - w_) / 2;
        break;
        case TextureAlignment::bottomCenter:
            rect_.x = (global::SCREEN_WIDTH - w_) + (w_ - h_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2;
        break;
        case TextureAlignment::bottomLeft:
            rect_.x = (global::SCREEN_WIDTH - w_) + (w_ - h_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2 +
            	(global::SCREEN_HEIGHT - w_) / 2;
        break;
        case TextureAlignment::bottomRight:
            rect_.x = (global::SCREEN_WIDTH - w_) + (w_ - h_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2 -
                (global::SCREEN_HEIGHT - w_) / 2;
        break;
        case TextureAlignment::center:
            rect_.x = (global::SCREEN_WIDTH - w_) / 2;
            rect_.y = (global::SCREEN_HEIGHT - h_) / 2;
        break;
    }
}

void TextureBase::FitScreenSize(int marginX, int marginY) {

    if (w_ > global::SCREEN_HEIGHT - marginX * 2) 
        w_ = global::SCREEN_HEIGHT - marginX * 2;
    if (h_ > global::SCREEN_WIDTH - marginY * 2) 
        h_ = global::SCREEN_WIDTH - marginY * 2;

    updateTargetRect(alignment_);
}

void TextureBase::render() const {
    SDL_RenderCopyEx(global::renderer, 
        texture_.get(), 
        nullptr, 
        &rect_,
        270, nullptr, SDL_FLIP_NONE
    );
}

void TextureBase::render(int offsetX, int offsetY) const {
    auto rect = rect_;
    rect.x += offsetY;
    rect.y -= offsetX;

    SDL_RenderCopyEx(global::renderer, 
        texture_.get(), 
        nullptr, 
        &rect,
        270, nullptr, SDL_FLIP_NONE
    );
}

void TextureBase::scrollLeft(int offset) {
    rect_.y += offset;
}

