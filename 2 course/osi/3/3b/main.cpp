#include <iostream>
#include <linux/fs.h>
#include <vector>
#include "FileUtils.h"

namespace fs = std::filesystem;


int main(int argc, char **argv) {
    try {
        std::filesystem::path path = argv[0];
        if (path.filename() == "myMkDir") {
            if (argc != 2) return 0;
            fs::path dirPath = argv[1];
            FileUtils::mk_dir(dirPath);
        } else if (path.filename() == "myLs") {
            if (argc != 2) return 0;
            fs::path dirPath = argv[1];
            FileUtils::ls(dirPath);
        } else if (path.filename() == "myRm") {
            if (argc != 2) return 0;
            fs::path filePath = argv[1];
            FileUtils::rm(filePath);
        } else if (path.filename() == "myTouch") {
            if (argc != 2) return 0;
            fs::path filePath = argv[1];
            FileUtils::touch(filePath);
        } else if (path.filename() == "myCat") {
            if (argc != 2) return 0;
            fs::path filePath = argv[1];
            FileUtils::cat(filePath);
        } else if (path.filename() == "createSymlink") {
            if (argc != 3) return 0;
            fs::path filePath = argv[1];
            fs::path linkPath = argv[2];
            FileUtils::create_symlink(filePath, linkPath);
        } else if (path.filename() == "createHardlink") {
            if (argc != 3) return 0;
            fs::path filePath = argv[1];
            fs::path linkPath = argv[2];
            FileUtils::create_hard_link(filePath, linkPath);
        } else if (path.filename() == "readSymLink") {
            if (argc != 2) return 0;
            fs::path filePath = argv[1];
            FileUtils::cat_symlink(filePath);
        } else if (path.filename() == "showStatus") {
            if (argc != 2) return 0;
            fs::path filePath = argv[1];
            FileUtils::show_status(filePath);
        } else if (path.filename() == "setPerms") {
            if (argc != 3) return 0;
            fs::path filePath = argv[1];
            std::string str = argv[2];
            std::vector<bool> flags(9, false);
            for (int i = 0; i < 9; ++i) {
                if (str[i] == '1') flags[i] = true;
            }
            std::cout << filePath << std::endl;
            for (auto boolean: flags) {
                std::cout << boolean << ' ';
            }
            std::cout << std::endl;
            FileUtils::set_permissions(filePath, flags);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
