#pragma once
#include "Field.h"
#include <string>
#include <fstream>
#include <stdexcept>
#include <cctype>

namespace parallelLifeConway {
    class InputFileParser {
    public:
        static Field pars(std::istream &inputStream);
    };
}
