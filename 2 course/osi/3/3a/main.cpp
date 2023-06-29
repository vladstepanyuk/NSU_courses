#include <iostream>
#include <sys/stat.h>
#include "FileUtils.h"
#include "StringUtils.h"
#include <sys/stat.h>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    if (argc != 2) return 1;
    std::string path(argv[1]);
    fs::path dirPath = path;
    if (!fs::is_directory(dirPath)) return 1;
    FileUtils::invertDirectory(path);

    return 0;
}
