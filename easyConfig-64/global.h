#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <map>
#include <string>

using std::map;
using std::string;

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

namespace global {
extern int screen_width;
extern int screen_height;
const int ROTATION = 0;

extern SDL_Renderer *renderer;
extern TTF_Font *font;
extern SDL_Color text_color;
extern SDL_Color minor_text_color;
extern map<string, string> aliases;

string replaceAliases(const string &s);
string replaceAliases(const string &s, unsigned int index, const string &value);

} // namespace global

#endif // GLOBAL_H_
