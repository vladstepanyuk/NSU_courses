//
// Created by io on 21.10.2022.
//
#include "InputFileParser.h"

namespace {
    const std::string StingSize = "#S";
}

parallelLifeConway::Field parallelLifeConway::InputFileParser::pars(std::istream &inputStream) {
    std::string str;

    int xSize, ySize;
    inputStream >> str;
    if (str != StingSize) {
        throw std::invalid_argument("wrong file format");
    }
    inputStream >> str;
    char *err;
    xSize = strtol(str.c_str(), &err, 10);
    if (*err != 0) {
        throw std::invalid_argument("wrong file format");
    }
    inputStream >> str;
    ySize = strtol(str.c_str(), &err, 10);
    if (*err != 0) {
        throw std::invalid_argument("wrong file format");
    }

    std::vector<char> field1(xSize*ySize, 0);
    Field field = Field(field1, xSize, ySize);



    for (int x, y; inputStream >> str;) {
        x = strtol(str.c_str(), &err, 10);
        if (*err != 0) {
            throw std::invalid_argument("wrong file format");
        }
        inputStream >> str;
        y = strtol(str.c_str(), &err, 10);
        if (*err != 0) {
            throw std::invalid_argument("wrong file format");
        }
        field.set(x, y, true);
    }

    return field;
}
