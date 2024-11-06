#include "image_item.h"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "global.h"
#include "SDL_rotozoom.h"
#include "fileutils.h"

using namespace std;

ImageItem::ImageItem(int index, std::string filename, bool rotation)
    : index_(index), filename_(std::move(filename)), rotation_(rotation)
{
    init();
}

void ImageItem::init()
{
    loading_ok_ = false;

    description_ = File_utils::getShortFileName(filename_);
}

void ImageItem::loadImage()
{
    if (loading_ok_)
        return;

    if (rotation_) {
        image_ = SDLSurfaceUniquePtr{
            loadImageToFit(
                filename_,
                global::SCREEN_HEIGHT,
                global::SCREEN_WIDTH)};
    } else {
        image_ = SDLSurfaceUniquePtr{
            loadImageToFit(
                filename_,
                global::SCREEN_WIDTH,
                global::SCREEN_HEIGHT)};
    }

    if (image_ == nullptr)
        cerr << ("Image loading failed: ") << filename_ << endl;
    loading_ok_ = (image_ != nullptr);
}

void ImageItem::createTexture()
{
    texture_ = SDLTextureUniquePtr{
        SDL_CreateTextureFromSurface(global::renderer, image_.get())};

    if (texture_ == nullptr)
        cerr << ("Texture creation failed") << endl;
}

void ImageItem::render()
{
    if (!loading_ok_)
        return;

    render(0, 0);
}

void ImageItem::render(int x, int y)
{
    if (!loading_ok_)
        return;

    if (texture_ == nullptr)
    {
        createTexture();
    }

    // Rectangle to hold the offsets
    SDL_Rect dstrect;
    if (rotation_) {
        // Set offsets
        dstrect.x = (global::SCREEN_WIDTH - image_->w) / 2 - 1 + x;
        dstrect.y = (global::SCREEN_HEIGHT - image_->h) / 2 + y;
        dstrect.w = image_->w;
        dstrect.h = image_->h;
        // Blit the surface
        SDL_RenderCopyEx(global::renderer, texture_.get(), nullptr,
                        &dstrect, 270, nullptr, SDL_FLIP_NONE);
    } else {
        // Set offsets
        dstrect.x = (global::SCREEN_WIDTH - image_->w) / 2 - 1 + x;
        dstrect.y = (global::SCREEN_HEIGHT - image_->h) / 2 + y;
        dstrect.w = image_->w;
        dstrect.h = image_->h;
        // Blit the surface
        SDL_RenderCopyEx(global::renderer, texture_.get(), nullptr,
                        &dstrect, 0, nullptr, SDL_FLIP_NONE);
    }
}

void ImageItem::renderOffset(double offset_x, double offset_y)
{
    if (!loading_ok_)
        return;

    int pos_x = static_cast<int>(offset_x * global::SCREEN_WIDTH);
    int pos_y = static_cast<int>(offset_y * global::SCREEN_HEIGHT);
    render(pos_x, pos_y);
}

SDLSurfaceUniquePtr ImageItem::loadImageToFit(
    const std::string &p_filename, int fit_w, int fit_h)
{
    // Load image
    SDL_Surface *l_img = IMG_Load(p_filename.c_str());
    if (IMG_GetError() != nullptr && *IMG_GetError() != '\0')
    {
        if (!strcmp(IMG_GetError(), "Unsupported image format") == 0)
        {
            cerr << "loadImageToFit: " << IMG_GetError() << endl;
        }
        SDL_ClearError();
        return nullptr;
    }

    const double aspect_ratio = static_cast<double>(l_img->w) / l_img->h;
    int target_w, target_h;
    if (fit_w * l_img->h <= fit_h * l_img->w)
    {
        target_w = fit_w; // std::min(l_img->w, fit_w);
        target_h = target_w / aspect_ratio;
    }
    else
    {
        target_h = fit_h; // std::min(l_img->h, fit_h);
        target_w = target_h * aspect_ratio;
    }
    SDLSurfaceUniquePtr l_img2{zoomSurface(l_img,
                                           static_cast<double>(target_w) / l_img->w,
                                           static_cast<double>(target_h) / l_img->h, SMOOTHING_ON)};
    SDL_FreeSurface(l_img);

    return l_img2;
}
