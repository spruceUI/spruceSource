#include "global.h"

#include <SDL.h>

namespace global
{

	SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color text_color = {235, 219, 178, 255};
    SDL_Color minor_text_color = {124, 111, 100, 255};
    map<string, string> aliases;
    
    string replaceAliases(const string & str) {
        // replace aliases with corresponding value
        string s = str;
        std::size_t first = s.find("$");
        if (first != string::npos) {
            for (const auto& [key, value] : global::aliases)
            {
                while (true) {
                    std::size_t start = first;
                    auto pos = s.find(key, start);
                    if (pos == string::npos) break;
                    s.replace(pos, key.length(), value);
                    start = pos + key.length();
                    if (start >= s.length()) break;
                }
            }
        }
        return s;
    }

} // namespace constants
