#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <string>
#include <vector>

using std::string;

namespace File_utils
{
    // File utilities

    bool fileExists(const string &p_path);

    string getLowercaseFileExtension(const string &name);

    string getFileName(const string &p_path);

    string getShortFileName(const string &p_path);

    string getPath(const string &p_path);

    string getCWP();
}

#endif
