//
// Created by Kenny on 2/21/2020.
//

#ifndef VDI_FUSE_INODE_H
#define VDI_FUSE_INODE_H

struct Inode
{
    uint16_t typePermissions;
    uint16_t userId;
    uint32_t lower32BitsSize;
    uint32_t lastAccessTime;
    uint32_t creationTime;
    uint32_t lastModificationTime;
    uint32_t deletionTime;
    uint16_t groupId;
    uint16_t hardLinkCount;
    uint32_t diskSectorCount;
    uint32_t flags;
    uint32_t opSystemValue1;
    uint32_t pointers[15];
    uint32_t generationNumber;
    // for ext2 version 0, reserved field, for ext2 version >= 1, extended attribute block
    uint32_t reservedField;
    // for ext2 version 0, reserved field, for ext2 version >= 1, upper 32 bits of file size
    uint32_t reservedField2;
    uint32_t fragmentBlockAddress;
    uint8_t opSysValue2[12];
};

#endif //VDI_FUSE_INODE_H
