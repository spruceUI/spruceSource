#include "fileutils.h"

#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iterator>
#include <memory>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef _POSIX_SPAWN
#include <poll.h>
#include <spawn.h>
#else
#include <signal.h>
#endif

namespace
{
    void AsciiToLower(char *c)
    {
        if (*c >= 'A' && *c <= 'Z')
            *c -= ('Z' - 'z');
    }

    void AsciiToLower(string *s)
    {
        for (char &c : *s)
            AsciiToLower(&c);
    }
} // namespace

bool File_utils::fileExists(const string &p_path)
{
    struct stat l_stat;
    return stat(p_path.c_str(), &l_stat) == 0;
}

string File_utils::getLowercaseFileExtension(const string &name)
{
    const auto dot_pos = name.rfind('.');
    if (dot_pos == string::npos)
        return "";
    string ext = name.substr(dot_pos + 1);
    AsciiToLower(&ext);
    return ext;
}

string File_utils::getFileName(const string &p_path)
{
    size_t l_pos = p_path.rfind('/');
    return p_path.substr(l_pos + 1);
}

string File_utils::getShortFileName(const string &p_path)
{
    auto name = getFileName(p_path);
    const auto dot_pos = name.rfind('.');
    if (dot_pos == string::npos)
        return name;
    string shortFilename = name.substr(0, dot_pos);
    return shortFilename;
}

string File_utils::getPath(const string &p_path)
{
    size_t l_pos = p_path.rfind('/');
    return p_path.substr(0, l_pos);
}

string File_utils::getCWP()
{
    const size_t size = 1024;
    char buffer[size];

    if (getcwd(buffer, size) != nullptr)
    {
        string path = buffer;
        return path;
    }
    else
    {
        return "";
    }
}
