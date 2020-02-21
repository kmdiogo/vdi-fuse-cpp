//
// Created by Kenny on 2/20/2020.
//

#ifndef VDI_FUSE_DIRECTORY_H
#define VDI_FUSE_DIRECTORY_H

#define REWIND_NO_DOTS 24
#define REWIND_DOTS 0

#include <iostream>
#include <cstdint>
#include <cstring>
#include "./Inode.h"
#include "../utils.h"

using namespace std;

class Directory {
public:
    Directory(Inode* inode, uint8_t* contents, uint32_t inodeNumber, uint32_t blockSize);
    ~Directory();
    bool open();
    void close();
    void rewind(uint32_t location);
    bool getNextEntry();

    char* getName() { return name; }
    uint32_t getInodeNumber() { return inodeNumber; }
    uint32_t getType() { return type; };
    uint8_t* getContents() { return contents; }


private:
    Inode* inode;
    long long cursor;
    uint8_t* contents;
    char* name;
    uint32_t inodeNumber;
    uint32_t type;
    uint32_t blockSize;

    bool isOpened;
};


/*struct Directory
{
    Inode* inode;
    long long cursor;
    uint8_t* contents;
    char* name;
    uint32_t inodeNumber;
    uint32_t type;
};*/

#endif //VDI_FUSE_DIRECTORY_H
