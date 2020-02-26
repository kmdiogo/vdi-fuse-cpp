//
// Created by Kenny on 2/16/2020.
//

#ifndef VDIFUSE_VDI_H
#define VDIFUSE_VDI_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>

#include "utils.h"

#define VDI_SET 0
#define VDI_CUR 1
#define VDI_END 2

using namespace std;

struct DiskGeometry
{
    uint32_t cylinders;
    uint32_t heads;
    uint32_t sectors;
    uint32_t sectorSize;
};


struct Header
{
    //block = pages
    uint8_t preHeader[72];
    uint32_t headerSize;
    uint32_t imageType;
    uint32_t imageFlags;
    uint8_t imageDescription[32];
    DiskGeometry diskGeometry;
    uint32_t offsetPages;
    uint32_t offsetData;
    uint32_t unused;
    long long diskSize;
    uint32_t pageSize;
    uint32_t pageExtraData;
    uint32_t pagesInHDD;
    uint32_t pagesAllocated;
    uint8_t UUID[16];
    uint8_t UUIDLastSnap[16];
    uint8_t UUIDLink[16];
    uint8_t UUIDParent[16];
    uint8_t notNeeded[56];

};


class VDI {
public:
    VDI() = default;
    bool open(const string& filename);
    void close();
    void seek(long long offset, int anchor);
    void read(uint8_t* buffer, size_t nbytes);

private:
    Header header;
    int fileDescriptor;
    FILE * file;
    long long cursor;

};

/*void vdiWrite(VDIFile* vdi, uint8_t* buffer, size_t nbytes);
void writeBlock(VDIFile* vdi, uint8_t* buffer, uint32_t blockNumber);*/

#endif //VDIFUSE_VDI_H
