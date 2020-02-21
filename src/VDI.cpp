//
// Created by Kenny on 2/19/2020.
//

#include "../include/VDI.h"

VDI::VDI() {

}

VDI::~VDI() {
    close();
}

bool VDI::open(const string& filename) {
    cout << "Opening VDI File: " << filename << endl;
    file = fopen(filename.c_str(), "rb+");
    if (file == NULL) {
        cout << "Could not open VDI File: " << endl;
        perror("fopen");
        return false;
    }

    fread(header.preHeader, 1, 72, file);
    fread(&header.headerSize, 4, 1, file);
    fread(&header.imageType, 4, 1, file);
    fread(&header.imageFlags, 4, 1, file);
    fread(header.imageDescription, 1, 32, file);
    fseek(file, 0x154, SEEK_SET);
    fread(&header.offsetPages, 4, 1, file);
    fread(&header.offsetData, 4, 1, file);
    fread(&header.diskGeometry.cylinders, 4, 1, file);
    fread(&header.diskGeometry.heads, 4, 1, file);
    fread(&header.diskGeometry.sectors, 4, 1, file);
    fread(&header.diskGeometry.sectorSize, 4, 1, file);
    fread(&header.unused, 4, 1, file);
    fread(&header.diskSize, 8, 1, file);
    fread(&header.pageSize, 4, 1, file);
    fread(&header.pageExtraData, 4, 1, file);
    fread(&header.pagesInHDD, 4, 1, file);
    fread(&header.pagesAllocated, 4, 1, file);
    fread(header.UUID, 1, 16, file);
    fread(header.UUIDLastSnap, 1, 16, file);
    fread(header.UUIDLink, 1, 16, file);
    fread(header.UUIDParent, 1, 16, file);
    fread(header.notNeeded, 1, 56, file);

    fseek(file, header.offsetData + 454, SEEK_SET);
    uint32_t volumeStart;
    fread(&volumeStart, 4, 1, file);
    volumeStart = volumeStart*512;
    header.offsetData += volumeStart;

    return true;
}

void VDI::close() {
    if (file != NULL) {
        fclose(file);
    }
}

void VDI::seek(long long offset, int anchor) {
    if(anchor == VDI_SET)
    {
        cursor = header.offsetData + offset;
    }
    if(anchor == VDI_CUR)
    {
        cursor += offset;
    }
    if(anchor == VDI_END)
    {
        // cursor should be negative
        cursor = offset + header.diskSize;
    }
}

// call vdiSeek before vdiRead at all times
void VDI::read(uint8_t *buffer, size_t nbytes) {
    long long page = cursor / header.pageSize;
    long long offset = cursor % header.pageSize;

    long long position = page * header.pageSize + offset;
    fseek(file, position, SEEK_SET);
    fread(buffer, 1, nbytes, file);
}

/*
void vdiWrite(VDIFile* vdi, uint8_t* buffer, size_t nbytes)
{
    long long page = vdi->cursor/vdi->header->pageSize;
    long long offset = vdi->cursor%vdi->header->pageSize;

    long long position = page*vdi->header->pageSize+offset;
    fseek(vdi->f, position, SEEK_SET);
    fwrite(buffer, 1, nbytes, vdi->f);
}

void writeBlock(VDIFile* vdi, uint8_t* buffer, uint32_t blockNumber)
{
    vdiSeek(vdi,  blockNumber*vdi->superBlock->blockSize, VDI_SET);
    vdiWrite(vdi, buffer, vdi->superBlock->blockSize);
}*/






