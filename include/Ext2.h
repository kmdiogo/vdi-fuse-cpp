//
// Created by Kenny on 2/17/2020.
//

#ifndef VDIFUSE_EXT2_STRUCTS_H
#define VDIFUSE_EXT2_STRUCTS_H

#define SUPERBLOCK_SIZE 1024
#define INODE_SIZE 128
#define BLOCK_DESCRIPTOR_SIZE 32

#include <cstdint>
#include "../include/VDI.h"
#include "./Ext2/Directory.h"
#include "../include/Ext2/Inode.h"

class Directory;

struct SuperBlock
{
    uint32_t totalInodes;
    uint32_t totalBlocks;
    uint32_t superUserBlocks;
    uint32_t unallocatedBlocks;
    uint32_t unallocatedInodes;
    uint32_t superBlockNumber;
    uint32_t log2BlockSize;
    uint32_t log2FragmentSize;
    uint32_t blocksPerGroup;
    uint32_t fragmentsPerGroup;
    uint32_t inodesPerGroup;
    uint32_t lastMountTime;
    uint32_t lastWriteTime;
    //number of times mounted since last consistency check
    uint16_t timesMounted;
    // number of mounts allowed before consistency check
    uint16_t mountsAllowed;
    // should be 0xEF53
    uint16_t magic;
    // 1 means clean, 2 means system has errors
    uint16_t systemState;
    // what to do when there is an error
    // 1 - ignore error, 2 - remount file system as read only, 3 - kernel panic
    uint16_t errorHandler;
    // combine with major portion to get full version
    uint16_t minorVersion;
    // POSIX time of last consistency check
    uint32_t lastCheck;
    // interval between forced consistency checks
    uint32_t interval;
    // 0 - linux, 1 - GNU HURD, 2 - MASIX, 3 - FREEBSD, 4 - other "lites"
    uint32_t opSysId;
    uint32_t majorPortion;
    // user id that can use reserved blocks
    uint16_t userId;
    // group id that can use reserved blocks
    uint16_t groupId;

    uint32_t numBlockGroups;
    // left shift 1024 by log2PageSize to get page size
    uint32_t blockSize;

    uint8_t fullArray[SUPERBLOCK_SIZE];

};


struct BlockGroupDescriptor
{
    uint32_t blockUsageBitmap;
    uint32_t inodeUsageBitmap;
    uint32_t inodeTableAddress;
    uint16_t numUnallocatedBlocks;
    uint16_t numUnallocatediNodes;
    uint16_t numDirectories;
    uint8_t* blockGroupDescriptorFullContents;

    // some unused bytes here

};

/*struct Ext2 {
    BlockGroupDescriptor ** blockGroupDescriptorTable;
    SuperBlock* superBlock;
    uint8_t* blockGroupDescriptorFullContents;
};*/

class Ext2 {
public:
    Ext2(VDI* vdi);
    ~Ext2();
    Inode fetchInode(uint32_t iNodeNumber);
    Directory fetchDirectory(uint32_t inodeNumber);


    void fetchBlockFromInode(Inode *inode, int blockNum, uint8_t *blockBuf);
    uint32_t getBlockSize() { return superBlock.blockSize; }
    void traverse(Directory* dir);

private:
    void fetchBlock(uint8_t* buffer, uint32_t blockNumber);

    void fetchSingle(Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
    void fetchDouble(Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
    void fetchTriple(Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb);

    void readSuperBlock();
    void readBlockDescTable();
    uint32_t getNumBlockGroups();

    SuperBlock superBlock;
    uint8_t* blockGroupDescriptorFullContents;
    BlockGroupDescriptor** blockGroupDescriptorTable;
    VDI* vdi;


};

#endif //VDIFUSE_EXT2_STRUCTS_H
