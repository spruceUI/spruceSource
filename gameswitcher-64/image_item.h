#ifndef IMAGE_ITEM_H_
#define IMAGE_ITEM_H_

#include <string>

#include "sdl_unique_ptr.h"

class ImageItem
{
public:
    explicit ImageItem(int index, std::string filename, bool rotation=true);
    virtual ~ImageItem() = default;

    // disallow copying and assignment
    ImageItem(const ImageItem &) = delete;
    ImageItem &operator=(const ImageItem &) = delete;

    void loadImage();

    void createTexture();

    // render itself at center screen
    void render();

    // render itself at specified position
    void render(int x, int y);

    // render itself at with offset in proportion to screen size,
    // image is centered in screen when offset is zero
    void renderOffset(double offset_x, double offset_y);

    bool loading_ok_;
    int getIndex() const { return index_; }
    std::string getFilename() const { return filename_; }
    void setDescription(std::string description) { description_ = description; }
    std::string getDescription() const { return description_; }
    SDL_Texture * getTexture() const { return texture_.get(); }
private:
    void init();

    // Load an image to fit the given viewport size.
    SDLSurfaceUniquePtr loadImageToFit(
        const std::string &p_filename, int fit_w, int fit_h);

    const int index_;
    const std::string filename_;
    std::string description_;
    SDLSurfaceUniquePtr image_ = nullptr;
    SDLTextureUniquePtr texture_ = nullptr;
    const bool rotation_;
};

#endif // IMAGE_ITEM_H_
