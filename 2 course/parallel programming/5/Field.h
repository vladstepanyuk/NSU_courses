#pragma once
#include <vector>
#include <string>
#include <iostream>

namespace parallelLifeConway{
    extern const std::vector<char> StandardBRules;
    extern const std::vector<char> StandardSRules;

    class Field final {
    public:
        Field();

        Field(const std::vector<char> &field, int xSize, int ySize);

        Field(const Field &field);

        ~Field();


        void tickInternal(const parallelLifeConway::Field &oldField);

        void tickExternal(const parallelLifeConway::Field &oldField);


        bool get(int x, int y) const;

        void set(int x, int y, bool value);

        int getXSize() const;

        int getYSize() const;

        std::vector<char> &getField();

        bool operator==(const Field &rhs) const;

        bool operator!=(const Field &rhs) const;

    private:

        bool checkCondition(int x, int y) const;


        int xSize;
        int ySize;
        std::vector<char> field;
    };
}


