#include "texture_base.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "global.h"

using std::cerr, std::endl;

TextureBase::TextureBase(SDL_Surface *surface, TextureAlignment alignment) {
  init(surface, alignment);
}

void TextureBase::init(SDL_Surface *surface, TextureAlignment alignment,
                       double scale) {
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
  texture_ = SDLTextureUniquePtr{
      SDL_CreateTextureFromSurface(global::renderer, surface)};
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
    rect_.x = (global::screen_width - w_) / 2;
    rect_.y = 0;
    break;
  case TextureAlignment::topLeft:
    rect_.x = 0;
    rect_.y = 0;
    break;
  case TextureAlignment::topRight:
    rect_.x = global::screen_width - w_;
    rect_.y = 0;
    break;
  case TextureAlignment::bottomCenter:
    rect_.x = (global::screen_width - w_) / 2;
    rect_.y = global::screen_height - h_;
    break;
  case TextureAlignment::bottomLeft:
    rect_.x = 0;
    rect_.y = global::screen_height - h_;
    break;
  case TextureAlignment::bottomRight:
    rect_.x = global::screen_width - w_;
    rect_.y = global::screen_height - h_;
    break;
  case TextureAlignment::center:
    rect_.x = (global::screen_width - w_) / 2;
    rect_.y = (global::screen_height - h_) / 2;
    break;
  }
}

void TextureBase::FitScreenSize(int marginX, int marginY) {

  if (w_ > global::screen_width - marginX * 2)
    w_ = global::screen_width - marginX * 2;
  if (h_ > global::screen_height - marginY * 2)
    h_ = global::screen_height - marginY * 2;

  updateTargetRect(alignment_);
}

void TextureBase::render() const {
  SDL_RenderCopyEx(global::renderer, texture_.get(), nullptr, &rect_,
                   global::ROTATION, nullptr, SDL_FLIP_NONE);
}

void TextureBase::render(int offsetX, int offsetY) const {
  auto rect = rect_;
  rect.x += offsetX;
  rect.y += offsetY;

  SDL_RenderCopyEx(global::renderer, texture_.get(), nullptr, &rect,
                   global::ROTATION, nullptr, SDL_FLIP_NONE);
}

void TextureBase::scrollLeft(int offset) { rect_.x += offset; }
