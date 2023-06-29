//
// Created by io on 05.05.2023.
//

#include <cmath>
#include <iostream>
#include "c.h"


namespace {
    const int MIN_MALLOC_SIZE = 1024;
    const int MIN_MALLOC_COUNT = 40;
}

void *c::startAddr = nullptr;
void *c::ptr = nullptr;
bool c::flag = false;
size_t c::length = 0;

const size_t MallocRegionInfo::sizeOfInfo = 2 * sizeof(bool) + 2 * sizeof(void *);

typedef unsigned long long ull;

void c::mapNewSection() {
    int flag = 0;
    if (startAddr != nullptr) {
        flag = MAP_FIXED;
    }


    void *addr = mmap(startAddr, length + MIN_MALLOC_SIZE * MIN_MALLOC_COUNT +
                                 (MallocRegionInfo::sizeOfInfo) * (MIN_MALLOC_COUNT), PROT_READ | PROT_WRITE | flag,
                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (addr == MAP_FAILED)
        throw std::runtime_error("can't map new region");

    if (startAddr == nullptr)
        startAddr = addr;

    if (startAddr == addr) {
        fillRegion((void *) ((ull) startAddr + length),
                   (void *) ((ull) startAddr + length + MIN_MALLOC_SIZE * MIN_MALLOC_COUNT +
                             (MallocRegionInfo::sizeOfInfo) * (MIN_MALLOC_COUNT + 1)));
    }


    length += MIN_MALLOC_SIZE * MIN_MALLOC_COUNT + (MallocRegionInfo::sizeOfInfo) * (MIN_MALLOC_COUNT);


}

void c::fillRegion(void *startAddress, void *endAddress) {
    void *sA = startAddress;

    MallocRegionInfo info;
    void *prev = nullptr;


    while (endAddress > sA) {
        info.setIsFree(true);
        info.setPrevInfo(prev);
        info.setNextInfo((void *) ((ull) sA + MallocRegionInfo::sizeOfInfo + MIN_MALLOC_SIZE));
        info.printInfoToAddress(sA);
        prev = sA;
        sA = info.getNextInfo();
    }

}

void *c::malloc(size_t size) {
    if (!ptr) {
        ptr = startAddr;
    }

    size_t actualSize = static_cast<size_t>(std::ceil((1.0 * size) / MIN_MALLOC_SIZE)) * MIN_MALLOC_SIZE;

    if (length - ((ull) ptr - (ull) startAddr) < actualSize) {
        if (flag) {
            flag = false;
            throw std::runtime_error("can't allocate memory");
        }
        flag = true;
        ptr = nullptr;
        return malloc(size);
    }


    MallocRegionInfo info;
    void *tmpPtr = ptr;
    for (int i = 0; i < actualSize / MIN_MALLOC_SIZE; ++i) {
        info.readInfoFromAddress(tmpPtr);
        if (!info.isFree()) {

            tmpPtr = info.getPrevInfo();
            void *tmpPtr1 = info.getNextInfo();

            while (tmpPtr != nullptr && tmpPtr >= ptr) {
                free(tmpPtr);
                info.readInfoFromAddress(tmpPtr);
                tmpPtr = info.getPrevInfo();
            }

            ptr = tmpPtr1;
            return malloc(size);
        }
        info.setIsFree(false);
        info.printInfoToAddress(tmpPtr);
        tmpPtr = info.getNextInfo();
    }


    info.readInfoFromAddress(ptr);
    info.setNextInfo((void *) ((ull) ptr + MallocRegionInfo::sizeOfInfo * actualSize / MIN_MALLOC_SIZE + actualSize));
    info.printInfoToAddress(ptr);
    tmpPtr = ptr;
    ptr = info.getNextInfo();

    info.readInfoFromAddress(ptr);
    info.setPrevInfo(tmpPtr);
    info.printInfoToAddress(ptr);

    return (void *) ((ull) tmpPtr + MallocRegionInfo::sizeOfInfo);

}

void c::free(void *ptr) {
    MallocRegionInfo info;
    void *infoPtr = (void *) ((ull) ptr - MallocRegionInfo::sizeOfInfo);
    if (infoPtr < startAddr) {
        throw std::runtime_error("invalid free() pointer");
    }
    info.readInfoFromAddress(infoPtr);
    void *tmpPtr = info.getNextInfo();
    if (tmpPtr <= infoPtr || tmpPtr >= (void *) ((ull) startAddr + length)) {
        throw std::runtime_error("invalid free() pointer");
    }
    info.readInfoFromAddress(tmpPtr);
    if (info.getPrevInfo() != infoPtr) {
        throw std::runtime_error("invalid free() pointer");
    }

    fillRegion(infoPtr, tmpPtr);
}


bool MallocRegionInfo::isFree() const {
    return free;
}

void MallocRegionInfo::setIsFree(bool isFree) {
    MallocRegionInfo::free = isFree;
}

void *MallocRegionInfo::getNextInfo() const {
    return nextInfo;
}

void MallocRegionInfo::setNextInfo(void *nextInfo) {
    MallocRegionInfo::nextInfo = nextInfo;
}

void MallocRegionInfo::printInfoToAddress(void *address) {
    *(bool *) (address) = free;
    *(bool *) ((ull) address + sizeof(bool)) = correct;
    *(void **) ((ull) (address) + 2 * sizeof(bool)) = nextInfo;
    *(void **) ((ull) (address) + 2 * sizeof(bool) + sizeof(void *)) = prevInfo;

}

void *MallocRegionInfo::getPrevInfo() const {
    return prevInfo;
}

void MallocRegionInfo::setPrevInfo(void *prevInfo) {
    MallocRegionInfo::prevInfo = prevInfo;
}

void MallocRegionInfo::readInfoFromAddress(void *address) {
    free = *(bool *) (address);
    correct = *(bool *) ((ull) address + sizeof(bool));
    nextInfo = *(void **) ((ull) (address) + 2 * sizeof(bool));
    prevInfo = *(void **) ((ull) (address) + 2 * sizeof(bool) + sizeof(void *));
}

bool MallocRegionInfo::isCorrect() const {
    return correct;
}

void MallocRegionInfo::setCorrect(bool correct) {
    MallocRegionInfo::correct = correct;
}
