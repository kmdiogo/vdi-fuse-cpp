//
// Created by Kenny on 2/20/2020.
//

#include "../../include/Ext2/Directory.h"

Directory::Directory(Inode inode, uint8_t* contents, uint32_t inodeNumber, uint32_t blockSize) {
    this->inode = inode;
    this->inodeNumber = inodeNumber;
    this->blockSize = blockSize;
    this->contents = contents;
    this->cursor = 0;
}

Directory::~Directory() {

}

bool Directory::open() {
    // Is not directory
    if((inode.typePermissions & 0xF000u) != 0x4000u) {
        return false;
    }
    rewind(REWIND_NO_DOTS);
    return true;
}

void Directory::close() {
    delete[] contents;
}

void Directory::rewind(uint32_t location) {
    cursor = location;
}

bool Directory::getNextEntry() {
    if(cursor >= inode.lower32BitsSize) return false;

    uint32_t nextDirInode = 0;
    uint32_t nextDirEntrySize = 0;
    uint32_t nextDirNameLength = 0;
    uint32_t nextDirType = 0;
    char* nextDirName;

    memcpy(&nextDirInode, contents + cursor, 4);
    if(nextDirInode == 0) return false;

    memcpy(&nextDirEntrySize, contents + cursor + 4, 2);
    memcpy(&nextDirNameLength, contents + cursor + 6, 1);
    memcpy(&nextDirType, contents + cursor + 7, 1);

    nextDirName = new char[nextDirNameLength+1];
    memcpy(nextDirName, contents + cursor + 8, nextDirNameLength);
    nextDirName[nextDirNameLength] = 0;
    name = nextDirName;
    delete[] nextDirName;

    cursor += nextDirEntrySize;
    type = nextDirType;
    inodeNumber = nextDirInode;
    return true;
}



