#ifndef INC_3_STRINGUTILS_H
#define INC_3_STRINGUTILS_H

#include <filesystem>
#include <string>

class StringUtils {
public:
    static std::string invertDirectoryName(std::basic_string<char> path);

    static std::string invertString(std::basic_string<char> string);

};


#endif //INC_3_STRINGUTILS_H
