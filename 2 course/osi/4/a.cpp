//
// Created by io on 21.04.2023.
//

#include "a.h"

typedef unsigned long long ull;

const int BUFFER_SIZE = 1024;
jmp_buf jmpBuf;

int global_init_var = 10;
int global_not_init_var;
const int global_const_var = 10;

namespace {
    void sig_handler(int signo) {
        std::cerr << "\ncatch abbort\n";
        longjmp(jmpBuf, 1);
    }
}


void a::f1() {
    int local_var = 10;
    static int static_var = 10;
    const int const_var = 11;

    std::filesystem::path path = "/proc/self/maps";

    std::vector<map_info> maps = std::move(get_maps(path));

    std::cout << "local_var\n";
    print_address_map_info((ull) &local_var, maps);
    std::cout << "static_var\n";
    print_address_map_info((ull) &static_var, maps);
    std::cout << "const_.var\n";
    print_address_map_info((ull) &const_var, maps);
    std::cout << "global_init_var\n";
    print_address_map_info((ull) &global_init_var, maps);
    std::cout << "global_not_init_var\n";
    print_address_map_info((ull) &global_not_init_var, maps);
    std::cout << "global_const_var\n";
    print_address_map_info((ull) &global_const_var, maps);


}

void a::f3() {
    char *ptr1 = (char *) malloc(100);
    std::cout << "ptr " << (ull) ptr1 << " before: \n";
    memcpy(ptr1, std::string("hello world").c_str(), 12);
    for (int i = 0; i < 100; ++i) {
        std::cout << ptr1[i];
    }
    std::cout << std::endl << std::endl;
    char *ptr2 = ptr1;
    free(ptr1);
    std::cout << "ptr " << (ull) ptr2 << " after: \n";
    for (int i = 0; i < 100; ++i) {
        std::cout << ptr2[i];
    }
    std::cout << std::endl << std::endl;

    char *ptr3 = (char *) malloc(100);
    std::cout << "ptr2 " << (ull) ptr3 << " before: \n";
    memcpy(ptr3, std::string("hello world").c_str(), 12);
    for (int i = 0; i < 100; ++i) {
        std::cout << ptr3[i];
    }

    char *ptr4 = ptr3 + 50;

    if (setjmp (jmpBuf) == 0) {
        std::signal(SIGABRT, &sig_handler);
        free(ptr4);
        signal(SIGABRT, SIG_DFL);
    }


    std::cout << "ptr2 " << (ull) ptr3 << " after: \n";
    for (int i = 0; i < 100; ++i) {
        putchar(ptr3[i]);
    }
}

void a::f4() {
    if (const char *env_p = std::getenv("home"))
        std::cout << "Your $home is: " << env_p << '\n';

    if (setenv("home", "12", 1)) {
        std::cout << "err";
    }
    if (const char *env_p = std::getenv("home"))
        std::cout << "Your $home is: " << env_p << '\n';
}

ull ugly_f() {
    int i[13] = {1, 2, 1, 1, 1, 1, 2};

    return (ull) (&i);
}

void a::f2() {
    int *i = (int *) ugly_f();

    for (int j = 0; j < 13; ++j) {
        std::cout << i[j] << ' ';
    }
}

std::vector<map_info> a::get_maps(std::filesystem::path &path) {
    std::vector<map_info> maps{};
    std::ifstream inf(path.string(), std::ios_base::binary);
    if (!inf.good()) throw std::invalid_argument("can't open file");
    std::string buffer(BUFFER_SIZE, 0);
    while (inf.good()) {
        std::getline(inf, buffer);
        int firstSpace = buffer.find(' ');
        if (firstSpace == -1) continue;


        int first_ = buffer.find('-');
        std::string firstNumber = buffer.substr(0, first_);
        std::string secondNumber = buffer.substr(first_ + 1, firstSpace - first_);
        std::string perms_str = buffer.substr(firstSpace + 1, buffer.find(' ', firstSpace + 1));
        ull firstNum = strtoull(firstNumber.c_str(), nullptr, 16);
        ull secondNum = strtoull(secondNumber.c_str(), nullptr, 16);


        std::vector<bool> perms(4, false);

        for (int i = 0; i < 4; ++i)
            perms[i] = perms_str[i] != '-';

        maps.emplace_back(perms, std::pair(firstNum, secondNum), buffer.substr(buffer.rfind(' ') + 1));

    }
    return maps;
}


void a::print_address_map_info(ull address, const std::vector<map_info> &maps) {
    int st = 0, end = maps.size() - 1, i = (end + st) / 2;
    while (end > st) {
        if (maps[i].getStartEndAddress().first > address) end = i - 1;
        else if (maps[i].getStartEndAddress().first == address) break;
        else st = i + 1;
        i = (end + st) / 2;
    }
    if (maps[i].getStartEndAddress().first > address) {
        i -= 1;
    }

    std::cout << "address " << address << " in " << maps[i].getStartEndAddress().first << '-'
              << maps[i].getStartEndAddress().second << " perms: ";

    const std::vector<bool> &perms = maps[i].getPerms();

    std::cout << (perms[0] ? 'r' : '-');
    std::cout << (perms[1] ? 'w' : '-');
    std::cout << (perms[2] ? 'x' : '-');
    std::cout << (perms[3] ? 'p' : '-');

    std::cout << "\tpathname: " << maps[i].getPathname() << std::endl << std::endl;
}


map_info::map_info(const std::vector<bool> &perms, const std::pair<ull, ull> &startEndAddress,
                   std::string pathname) : perms(perms),
                                           start_end_address(
                                                   startEndAddress),
                                           pathname(std::move(
                                                   pathname)) {}

const std::vector<bool> &map_info::getPerms() const {
    return perms;
}

const std::pair<ull, ull> &map_info::getStartEndAddress() const {
    return start_end_address;
}

const std::string &map_info::getPathname() const {
    return pathname;
}
