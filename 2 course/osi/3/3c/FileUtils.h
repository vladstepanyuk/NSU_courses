#ifndef INC_3B_FILEUTILS_H
#define INC_3B_FILEUTILS_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <bitset>
#include <stdio.h>


typedef unsigned long long ull;

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

    static std::vector<std::pair<ull, ull>> get_maps(std::filesystem::path &path);
    
    static void print_page_map_info(std::filesystem::path &path, std::vector<std::pair<ull, ull>> &maps);

    static void seekg(std::ifstream &inf, ull offset);

    static void print_frame_info(std::string &buf);

};


#endif //INC_3B_FILEUTILS_H
