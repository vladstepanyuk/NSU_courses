//
// Created by io on 30.03.2023.
//

#include <iostream>
#include <fstream>
#include "FileUtils.h"

namespace {
    long BUFFER_SIZE = 1024;
}

namespace fs = std::filesystem;

void FileUtils::makeDir(const fs::path &path) {
    if (fs::exists(path)) {
        for (const auto&file: fs::directory_iterator(fs::directory_entry(path))) {
            fs::remove_all(file.path());
        }
    } else {
        fs::create_directories(path);
    }
}

void FileUtils::invertDirectory(const std::filesystem::path &path) {
    std::string newPath = StringUtils::invertDirectoryName(path.string());

    fs::path newDirPath = newPath;
    FileUtils::makeDir(newDirPath);
    for (const auto&file: fs::directory_iterator(fs::directory_entry(path))) {
        if (file.is_regular_file()) invertFile(file.path(), newDirPath);
    }

}

void FileUtils::invertFile(const std::filesystem::path &filePath, const std::filesystem::path &directory) {
    std::string newFileName = StringUtils::invertDirectoryName(filePath.filename());
    std::ofstream outFile(directory.string() + '/' + newFileName, std::ios_base::binary);
    std::ifstream inFile(filePath.string(), std::ios_base::binary);
    unsigned long FileSize = fs::file_size(filePath);
    std::string  buffer(BUFFER_SIZE, 0);
    for (unsigned long i = 1; i <= FileSize/BUFFER_SIZE; i += 1) {
        inFile.seekg(-static_cast<long>(i*BUFFER_SIZE), std::ios_base::end);
        inFile.read(&buffer[0], BUFFER_SIZE);
        std::string reverseStr = StringUtils::invertString(buffer);
        outFile.write(&reverseStr[0], BUFFER_SIZE);
    }
    buffer.resize(FileSize % BUFFER_SIZE);
    inFile.seekg(std::ios_base::beg);
    inFile.read(&buffer[0], FileSize % BUFFER_SIZE);
    std::string reverseStr = StringUtils::invertString(buffer);
    outFile.write(&reverseStr[0], FileSize % BUFFER_SIZE);

}
