#include <algorithm>
#include "Field.h"


namespace {
    const int xDiff[3] = {-1, 0, 1};
    const int yDiff[3] = {-1, 0, 1};
}

const std::vector<char> parallelLifeConway::StandardBRules = {3};
const std::vector<char> parallelLifeConway::StandardSRules = {2, 3};


parallelLifeConway::Field::Field() = default;


parallelLifeConway::Field::Field(const Field &field) = default;


void parallelLifeConway::Field::tickInternal(const parallelLifeConway::Field &oldField) {
    for (int y = 2; y < ySize - 2; ++y) {
        for (int x = 0; x < xSize; ++x) {
            set(x, y, oldField.checkCondition(x, y));
        }
    }
}

bool parallelLifeConway::Field::checkCondition(int x, int y) const {

    int counter = 0;
    for (int k = 0; k < 3; k++)
        for (int l = 0; l < 3; l++) {
            int x1 = (xSize + x + xDiff[k]) % xSize;
            int y1 = (ySize + y + yDiff[l]) % ySize;
            if (get(x1, y1) && (x1 != x || y1 != y)) counter++;
        }

    if (!get(x, y)) {
        return std::any_of(StandardBRules.begin(), StandardBRules.end(), [counter](int i) { return i  == counter;});
    } else {
        return std::any_of(StandardSRules.begin(), StandardSRules.end(), [counter](int i) { return i  == counter;});

    }
}

parallelLifeConway::Field::Field(const std::vector<char> &field, int xSize, int ySize) : xSize(xSize), ySize(ySize),
                                                                                         field(field) {}

bool parallelLifeConway::Field::get(int x, int y) const {
    return field[y*xSize + x];
}

void parallelLifeConway::Field::set(int x, int y, bool value) {
    field[y*xSize + x] = static_cast<char>(value);
}

void parallelLifeConway::Field::tickExternal(const parallelLifeConway::Field &oldField) {
    for (int x = 0; x < xSize; ++x) {
        set(x, 1, oldField.checkCondition(x, 1));
    }
    for (int x = 0; x < xSize; ++x) {
        set(x, ySize - 2, oldField.checkCondition(x, ySize - 2));
    }
}

int parallelLifeConway::Field::getXSize() const {
    return xSize;
}

int parallelLifeConway::Field::getYSize() const {
    return ySize;
}

std::vector<char> &parallelLifeConway::Field::getField() {
    return field;
}

bool parallelLifeConway::Field::operator==(const parallelLifeConway::Field &rhs) const {
    bool flag = true;

    for (int y = 1; y < ySize - 1; ++y) {
        for (int x = 0; x < xSize; ++x) {
            if (get(x, y) != rhs.get(x, y))
                return false;
        }
    }

    return xSize == rhs.xSize &&
           ySize == rhs.ySize;
}

bool parallelLifeConway::Field::operator!=(const parallelLifeConway::Field &rhs) const {
    return !(rhs == *this);
}


parallelLifeConway::Field::~Field() = default;


