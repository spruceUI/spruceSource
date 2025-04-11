#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include <string>

#include <SDL2/SDL.h>

#include "texture_base.h"

using std::string;

class ImageTexture: public TextureBase
{
public:
    explicit ImageTexture(
        const string & filename, 
        TextureAlignment alignment = TextureAlignment::topLeft
    );
    virtual ~ImageTexture() = default;

    // disallow copying and assignment
    ImageTexture(const ImageTexture &) = delete;
    ImageTexture &operator=(const ImageTexture &) = delete;

    const string & getFilename() const { return filename_; }

private:

    const string filename_;
};

#endif // IMAGE_TEXTURE_H