#include <iostream>
#include <vector>
#include <unistd.h>
#include "FileUtils.h"
#include <string>

namespace fs = std::filesystem;


int main(int argc, char **argv) {
    try {
        if (argc < 2) return 0;


        fs::path pagemap = "/proc/" + std::string(argv[1]) + "/pagemap";
        fs::path maps = "/proc/" + std::string(argv[1]) + "/maps";
        std::vector<std::pair<unsigned long long, unsigned long long>> stEndAddresses = FileUtils::get_maps(maps);

        for (auto pair:stEndAddresses) {
            std::cout << pair.first << ' ' << pair.second << std::endl;
        }

        FileUtils::print_page_map_info(pagemap, stEndAddresses);

        printf("pid: %d\n", getpid());
        char c;
        std::cin.getline(&c, 1);


    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}
