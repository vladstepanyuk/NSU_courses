//
// Created by io on 31.03.2023.
//

#include <vector>
#include "FileUtils.h"

namespace fs = std::filesystem;

namespace {
    int BUFFER_SIZE = 1024;
}

std::vector<std::filesystem::perms> FileUtils::options = std::vector<std::filesystem::perms>();


void FileUtils::mk_dir(std::filesystem::path &path) {
    if (fs::exists(path)) {
        throw std::invalid_argument(path.string() + "already exist");
    } else {
        fs::create_directories(path);
    }
}

void FileUtils::ls(std::filesystem::path &path) {
    if (is_directory(path)) {
        for (auto &file: fs::directory_iterator(fs::directory_entry(path))) {
            std::cout << file.path().filename() << std::endl;
        }
    } else {
        throw std::invalid_argument(path.string() + " is not a directory ");
    }
}

void FileUtils::rm(std::filesystem::path &path) {
    if (!fs::exists(path)) return;
    fs::remove_all(path);
}

void FileUtils::touch(std::filesystem::path &path) {
    std::ofstream of(path.string());
}

void FileUtils::cat(std::filesystem::path &path) {
    std::ifstream inf(path.string(), std::ios_base::binary);
    if (!inf.good()) throw std::invalid_argument("can't open file");
    std::string buffer(BUFFER_SIZE, 0);
    while (inf.good()) {
        inf.read(&buffer[0], BUFFER_SIZE);
        if (inf.gcount() != BUFFER_SIZE) std::cout << buffer.substr(0, inf.gcount());
        else std::cout << buffer;
    }
}

void FileUtils::create_symlink(std::filesystem::path &target, std::filesystem::path &link) {
    if (fs::is_directory(target)) {
        fs::create_directory_symlink(target, link);
    } else {
        fs::create_symlink(target, link);
    }
}

void FileUtils::cat_symlink(std::filesystem::path &link) {
    std::cout << link.filename();
    fs::exists(link) ? fs::is_symlink(link)
                       ? std::cout << " -> " << fs::read_symlink(link) << '\n'
                       : std::cout << " exists but it is not a symlink\n"
                     : std::cout << " does not exist\n";
}

void FileUtils::create_hard_link(std::filesystem::path &target, std::filesystem::path &link) {
    if (fs::exists(target)) fs::create_hard_link(target, link);
    else std::cout << " does not exist" << std::endl;
}

void FileUtils::show_status(std::filesystem::path &path) {
    std::filesystem::perms p = fs::status(path).permissions();
    std::cout << "permissions:" << std::endl;
    using std::filesystem::perms;
    auto show = [=](char op, perms perm) {
        std::cout << (perms::none == (perm & p) ? '-' : op);
    };
    show('r', perms::owner_read);
    show('w', perms::owner_write);
    show('x', perms::owner_exec);
    show('r', perms::group_read);
    show('w', perms::group_write);
    show('x', perms::group_exec);
    show('r', perms::others_read);
    show('w', perms::others_write);
    show('x', perms::others_exec);
    std::cout << std::endl;
    std::cout << "hard links count: " << fs::hard_link_count(path) << std::endl;
}

void FileUtils::set_permissions(std::filesystem::path &path, std::vector<bool> &perms) {
    if (options.size() != 9) {
        options = std::vector<std::filesystem::perms>(9);
        FileUtils::options[0] = fs::perms::owner_read;
        FileUtils::options[1] = fs::perms::owner_write;
        FileUtils::options[2] = fs::perms::owner_exec;
        FileUtils::options[3] = fs::perms::group_read;
        FileUtils::options[4] = fs::perms::group_write;
        FileUtils::options[5] = fs::perms::group_exec;
        FileUtils::options[6] = fs::perms::others_read;
        FileUtils::options[7] = fs::perms::others_write;
        FileUtils::options[8] = fs::perms::others_exec;
    }


    for (int i = 0; i < 9; ++i) {
        if (perms[i]) fs::permissions(path, options[i], fs::perm_options::add);
        else fs::permissions(path, options[i], fs::perm_options::remove);
    }

}

std::vector<std::pair<ull, ull>> FileUtils::get_maps(std::filesystem::path &path) {
    std::vector<std::pair<unsigned long long, unsigned long long>> maps{};
    std::stringstream iss;
    std::ifstream inf(path.string(), std::ios_base::binary);
    if (!inf.good()) throw std::invalid_argument("can't open file");
    std::string buffer(BUFFER_SIZE, 0);
    while (inf.good()) {
        std::getline(inf, buffer);
        int firstSpace = buffer.find(' ');
        if (firstSpace == -1) continue;
        maps.emplace_back(0, 0);


        int first_ = buffer.find('-');
        std::string firstNumber = buffer.substr(0, first_);
        std::string secondNumber = buffer.substr(first_ + 1, firstSpace - first_);
        maps.back().first = strtoull(firstNumber.c_str(), nullptr, 16);
        maps.back().second = strtoull(secondNumber.c_str(), nullptr, 16);

    }
    return maps;
}

void FileUtils::print_page_map_info(std::filesystem::path &path, std::vector<std::pair<ull, ull>> &maps) {
    FILE *file = fopen(path.c_str(), "rb+");
    std::string buffer(8, 0);
    for (auto pair: maps) {

        ull firstNumber = pair.first / 4096;
        ull secondNumber = pair.second / 4096;
        fseek (file, firstNumber*8, SEEK_SET);
        for (ull i = 0; i < secondNumber - firstNumber && file != NULL; ++i) {
            std::cout <<"address: "<< firstNumber + i << std::endl;
            int res = fread(&buffer[0], 1, 8, file);
            if (res != 8) std::cout << "ERROR" << std::endl;
            print_frame_info(buffer);
        }
    }
}

void FileUtils::seekg(std::ifstream &inf, ull offset) {
    inf.seekg(0, std::ios_base::beg);
    for (int i = 0; i < offset / INT32_MAX; ++i)
        inf.seekg(INT32_MAX, std::ios_base::cur);
    inf.seekg(static_cast<long>(offset) % INT32_MAX, std::ios_base::cur);
}

void FileUtils::print_frame_info(std::string &buf) {
    std::vector<std::bitset<8>> bitsets(buf.size());
    for (int i = 0; i < buf.size(); ++i) {
        bitsets[i] = buf[i];
    }

//    std::cout << "bool: " << bitsets[7][0] << std::endl;
    std::cout << "is present: " << bitsets[7][7] << std::endl;
    std::cout << "is swapped: " << bitsets[7][6] << std::endl;
    if (bitsets[7][7]) {
        ull PFN = 0;
        for (int i = 0; i < 6; ++i) {
            PFN |= bitsets[i].to_ullong();
            PFN <<= 8;
        }
        PFN >>= 1;
        PFN |= (bitsets[6] >> 1).to_ullong();
        std::cout << "Page frame number: " << PFN << std::endl;
    } else if (bitsets[7][6]) {
        ull swapType = (0 | (bitsets[0] >> 3).to_ullong());
        std::cout << "swap type: " << swapType << std::endl;
        ull swapOffset = (0 | ((bitsets[0] & std::bitset<8>(0b00000111)).to_ullong() << 8));
        for (int i = 1; i < 6; ++i) {
            swapOffset |= bitsets[i].to_ullong();
            swapOffset <<= 8;
        }
        swapOffset >>= 1;
        swapOffset |= (bitsets[6] >> 1).to_ullong();
        std::cout << "Page offset: "<< swapOffset << std::endl;
    }
}


