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
    if (fs::exists(path)) throw std::invalid_argument(path.string() + "already exists");
    std::ofstream of(path.string());
    if (of.fail()) throw std::invalid_argument("can't create file");
}

void FileUtils::cat(std::filesystem::path &path) {
    std::ifstream inf(path.string(), std::ios_base::binary);
    if (!inf.good()) throw std::invalid_argument("can't open file");
    std::string buffer(BUFFER_SIZE, 0);
    while (inf.good()) {
        inf.read(&buffer[0], BUFFER_SIZE);
        if (inf.gcount() != BUFFER_SIZE) std::cout<<buffer.substr(0, inf.gcount());
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


