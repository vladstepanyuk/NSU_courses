//
// Created by io on 05.05.2023.
//

#ifndef INC_4_C_H
#define INC_4_C_H
#include <unistd.h>
#include <sys/mman.h>
#include <stdexcept>


class MallocRegionInfo{
private:
    bool free;
    bool correct;
    void *nextInfo;
    void *prevInfo;
public:

    bool isCorrect() const;

    void setCorrect(bool correct);

    static const size_t sizeOfInfo;

    bool isFree() const;

    void setIsFree(bool isFree);

    void *getNextInfo() const;

    void *getPrevInfo() const;

    void setPrevInfo(void *prevInfo);

    void setNextInfo(void *nextInfo);

    void printInfoToAddress(void *address);

    void readInfoFromAddress(void *address);
};



class c {
private:
    static void* startAddr;
    static size_t length;
    static void* ptr;

    static bool flag;

public:
    static void fillRegion(void *startAddress, void *endAddress);
    static void mapNewSection();
    static void *malloc(size_t size);
    static void free(void *ptr);
};


#endif //INC_4_C_H
