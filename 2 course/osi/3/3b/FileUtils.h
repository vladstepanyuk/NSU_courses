//
// Created by io on 31.03.2023.
//

#ifndef INC_3B_FILEUTILS_H
#define INC_3B_FILEUTILS_H

#include <filesystem>
#include <iostream>
#include <fstream>



class FileUtils {
    static std::vector<std::filesystem::perms> options;
public:
    static void mk_dir(std::filesystem::path &path);

    static void ls(std::filesystem::path &path);

    static void rm(std::filesystem::path &path);

    static void touch(std::filesystem::path &path);

    static void cat(std::filesystem::path &path);

    static void create_symlink(std::filesystem::path &target, std::filesystem::path &link);

    static void cat_symlink(std::filesystem::path &link);

    static void create_hard_link(std::filesystem::path &target, std::filesystem::path &link);

    static void show_status(std::filesystem::path &path);

    static void set_permissions(std::filesystem::path &path, std::vector<bool> &perms);

};


#endif //INC_3B_FILEUTILS_H
