

#ifndef INC_4_A_H
#define INC_4_A_H
#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <csetjmp>
#include "Utils.h"


typedef unsigned long long ull;

class map_info {
    std::vector<bool> perms;
    std::pair<ull, ull> start_end_address;
    std::string pathname;
public:
    map_info(const std::vector<bool> &perms, const std::pair<ull, ull> &startEndAddress, std::string pathname);

    const std::vector<bool> &getPerms() const;

    const std::pair<ull, ull> &getStartEndAddress() const;

    const std::string &getPathname() const;
};

class a {
private:


    static std::vector<map_info> get_maps(std::filesystem::path &path);

    static void print_address_map_info(ull address, const std::vector<map_info> &maps);

public:
    static void f1();
    static void f2();
    static void f3();
    static void f4();

};


#endif //INC_4_A_H
