#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <string>
#include <vector>

namespace File_utils
{
    // File utilities

    bool fileExists(const std::string &p_path);

    std::string getLowercaseFileExtension(const std::string &name);

    std::string getFileName(const std::string &p_path);

    std::string getShortFileName(const std::string &p_path);

    std::string getPath(const std::string &p_path);

    std::string getCWP();
}

#endif
