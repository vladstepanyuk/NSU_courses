//
// Created by io on 30.03.2023.
//

#include "StringUtils.h"

std::string StringUtils::invertDirectoryName(std::basic_string<char> path) {
    int i = 0;
    std::filesystem::path p = path;
    std::string name = p.filename().string();
    std::string newPath;
    if (name == ".") {
        newPath = path.substr(0, path.size() - 2);
        return invertDirectoryName(newPath);
    }


    newPath = path.substr(0, path.size() - name.size());
    newPath += invertString(name);


    return newPath;
}

std::string StringUtils::invertString(std::basic_string<char> string) {
    std::string newString(string.size(), 0);
    for (int j = string.size() - 1, i = 0; j >= 0; --j, i++)
        newString[i] = string[j];

    return newString;
}
