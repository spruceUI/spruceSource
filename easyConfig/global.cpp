#include "global.h"

#include <SDL.h>

namespace global
{
    void replaceText(string & str, const string & key, const string & value, std::size_t start);

	SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color text_color = {235, 219, 178, 255};
    SDL_Color minor_text_color = {124, 111, 100, 255};
    map<string, string> aliases;
    
    void replaceText(string & str, const string & key, const string & value, std::size_t start=0) {
        while (true) {
            auto pos = str.find(key, start);
            if (pos == string::npos) break;
            str.replace(pos, key.length(), value);
            start = pos + value.length();
            if (start >= str.length()) break;
        }
    }

    string replaceAliases(const string & str) {
        // replace aliases with corresponding value
        string s = str;
        std::size_t first = s.find("$");
        if (first == string::npos) return s;

        for (const auto& [key, value] : global::aliases)
        {
            replaceText(s, key, value, first);
        }
        return s;
    }

    string replaceAliases(const string & str, unsigned int index, const string & value) {
        // first replace user aliases
        string s = replaceAliases(str);

        // try to replace _INDEX_
        replaceText(s, "_INDEX_", std::to_string(index));

        // try to replace _VALUE_
        replaceText(s, "_VALUE_", value);

        return s;
    }

} // namespace constants
