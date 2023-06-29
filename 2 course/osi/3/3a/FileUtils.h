

#ifndef INC_3_FILEUTILS_H
#define INC_3_FILEUTILS_H
#include <sys/stat.h>
#include <string>
#include <exception>
#include <filesystem>
#include "StringUtils.h"


class FileUtils {
public:

    static void invertDirectory(const std::filesystem::path &path);

    static void invertFile(const std::filesystem::path &filePath, const std::filesystem::path &directory);


    static void makeDir(const std::filesystem::path &path);

};


#endif //INC_3_FILEUTILS_H
