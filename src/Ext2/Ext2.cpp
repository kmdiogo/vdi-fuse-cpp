#include <cstring>
#include "../../include/Ext2/Ext2.h"

Ext2::Ext2(VDI *vdi) {
    this->vdi = vdi;
    this->blockGroupDescriptorTable = NULL;

    readSuperBlock();
    readBlockDescTable();
}

Ext2::~Ext2() {
    cout << "EXT2 Module: Destroying data structures..." << endl;
    if (blockGroupDescriptorTable != NULL) {
        for (size_t i = 0; i < superBlock.numBlockGroups; i++) {
            delete blockGroupDescriptorTable[i];
        }
        delete[] blockGroupDescriptorTable;
        delete[] blockGroupDescriptorFullContents;
    }
}

void Ext2::readSuperBlock() {
    uint8_t superBlockContents[SUPERBLOCK_SIZE];
    vdi->seek(0x400, VDI_SET);
    vdi->read(superBlockContents, SUPERBLOCK_SIZE);

    memcpy(superBlock.fullArray, superBlockContents, SUPERBLOCK_SIZE);
    memcpy(&superBlock.totalInodes, superBlockContents, 4);
    memcpy(&superBlock.totalBlocks, superBlockContents + 4, 4);
    memcpy(&superBlock.superUserBlocks, superBlockContents + 8, 4);
    memcpy(&superBlock.unallocatedBlocks, superBlockContents + 12, 4);
    memcpy(&superBlock.unallocatedInodes, superBlockContents + 16, 4);
    memcpy(&superBlock.superBlockNumber, superBlockContents + 20, 4);
    memcpy(&superBlock.log2BlockSize, superBlockContents + 24, 4);
    memcpy(&superBlock.log2FragmentSize, superBlockContents + 28, 4);
    memcpy(&superBlock.blocksPerGroup, superBlockContents + 32, 4);
    memcpy(&superBlock.fragmentsPerGroup, superBlockContents + 36, 4);
    memcpy(&superBlock.inodesPerGroup, superBlockContents + 40, 4);
    memcpy(&superBlock.lastMountTime, superBlockContents + 44, 4);
    memcpy(&superBlock.lastWriteTime, superBlockContents + 48, 4);
    memcpy(&superBlock.timesMounted, superBlockContents + 52, 2);
    memcpy(&superBlock.mountsAllowed, superBlockContents + 54, 2);
    memcpy(&superBlock.magic, superBlockContents + 56, 2);
    memcpy(&superBlock.systemState, superBlockContents + 58, 2);
    memcpy(&superBlock.errorHandler, superBlockContents + 60, 2);
    memcpy(&superBlock.minorVersion, superBlockContents + 62, 2);
    memcpy(&superBlock.lastCheck, superBlockContents + 66, 4);
    memcpy(&superBlock.interval, superBlockContents + 70, 4);
    memcpy(&superBlock.opSysId, superBlockContents + 74, 4);
    memcpy(&superBlock.majorPortion, superBlockContents + 78, 4);
    memcpy(&superBlock.userId, superBlockContents + 80, 2);
    memcpy(&superBlock.groupId, superBlockContents + 82, 2);

    superBlock.numBlockGroups = getNumBlockGroups();
    if (superBlock.numBlockGroups == 0) {
        cout << "EXT2: Error determining number of block groups." << endl;
        assert(1);
    }

    superBlock.blockSize = 1024u << superBlock.log2BlockSize;
}

uint32_t Ext2::getNumBlockGroups() {
    uint32_t numBlockGroups;
    if(superBlock.totalBlocks % superBlock.blocksPerGroup == 0)
    {
        numBlockGroups = superBlock.totalBlocks / superBlock.blocksPerGroup;
    }
    else
    {
        numBlockGroups = superBlock.totalBlocks / superBlock.blocksPerGroup + 1;
    }

    int numBlockGroups2;
    if(superBlock.totalInodes % superBlock.inodesPerGroup == 0)
    {
        numBlockGroups2 = superBlock.totalInodes / superBlock.inodesPerGroup;
    }
    else
    {
        numBlockGroups2 = superBlock.totalInodes / superBlock.inodesPerGroup + 1;
    }

    if(numBlockGroups == numBlockGroups2)
    {
        return numBlockGroups;
    }
    else return 0;
}

void Ext2::readBlockDescTable() {
    uint8_t blockDescTable[superBlock.blockSize];
    fetchBlock(blockDescTable, 2);

    blockGroupDescriptorTable = new BlockGroupDescriptor*[superBlock.numBlockGroups];
    for (size_t i = 0; i < superBlock.numBlockGroups; i++) {
        blockGroupDescriptorTable[i] = new BlockGroupDescriptor;
    }

    blockGroupDescriptorFullContents = new uint8_t[superBlock.blockSize];
    memcpy(blockGroupDescriptorFullContents, blockDescTable, superBlock.blockSize);

    for(size_t i = 0; i < superBlock.numBlockGroups; i++)
    {
        memcpy(&blockGroupDescriptorTable[i]->blockUsageBitmap, blockDescTable + i * BLOCK_DESCRIPTOR_SIZE, 4);
        memcpy(&blockGroupDescriptorTable[i]->inodeUsageBitmap, blockDescTable + i * BLOCK_DESCRIPTOR_SIZE + 4, 4);
        memcpy(&blockGroupDescriptorTable[i]->inodeTableAddress, blockDescTable + i * BLOCK_DESCRIPTOR_SIZE + 8, 4);
        memcpy(&blockGroupDescriptorTable[i]->numUnallocatedBlocks, blockDescTable + i * BLOCK_DESCRIPTOR_SIZE + 12, 2);
        memcpy(&blockGroupDescriptorTable[i]->numUnallocatediNodes, blockDescTable + i * BLOCK_DESCRIPTOR_SIZE + 14, 2);
        memcpy(&blockGroupDescriptorTable[i]->numDirectories, blockDescTable + i * BLOCK_DESCRIPTOR_SIZE + 16, 2);
    }
}

Inode Ext2::fetchInode(uint32_t iNodeNumber) {
    uint8_t iNodeBuffer[INODE_SIZE];
    uint32_t iNodeSize = INODE_SIZE;
    uint32_t blockGroup = (iNodeNumber-1) / superBlock.inodesPerGroup;
    uint32_t index = (iNodeNumber-1) % superBlock.inodesPerGroup;
    uint32_t containingBlock = (index*iNodeSize) / superBlock.blockSize;

    Inode inode{};
    uint8_t buf[superBlock.blockSize];
    fetchBlock(buf, containingBlock + blockGroupDescriptorTable[blockGroup]->inodeTableAddress);

    index = index % 8;

    memcpy(iNodeBuffer, buf+(index)*iNodeSize,iNodeSize);

    memcpy(&inode.typePermissions, iNodeBuffer, 2);
    memcpy(&inode.userId, iNodeBuffer + 2, 2);
    memcpy(&inode.lower32BitsSize, iNodeBuffer + 4, 4);
    memcpy(&inode.lastAccessTime, iNodeBuffer + 8, 4);
    memcpy(&inode.creationTime, iNodeBuffer + 12, 4);
    memcpy(&inode.lastModificationTime, iNodeBuffer + 16, 4);
    memcpy(&inode.deletionTime, iNodeBuffer + 20, 4);
    memcpy(&inode.groupId, iNodeBuffer + 24, 2);
    memcpy(&inode.hardLinkCount, iNodeBuffer + 26, 2);
    memcpy(&inode.diskSectorCount, iNodeBuffer + 28, 4);
    memcpy(&inode.flags, iNodeBuffer + 32, 4);
    memcpy(&inode.opSystemValue1, iNodeBuffer + 36, 4);
    memcpy(inode.pointers, iNodeBuffer + 40, 60);
    memcpy(&inode.generationNumber, iNodeBuffer + 100, 4);
    memcpy(&inode.reservedField, iNodeBuffer + 104, 4);
    memcpy(&inode.reservedField2, iNodeBuffer + 108, 4);
    memcpy(&inode.fragmentBlockAddress, iNodeBuffer + 112, 4);
    memcpy(inode.opSysValue2, iNodeBuffer + 116, 12);

    return inode;

}

void Ext2::fetchBlock(uint8_t *buffer, uint32_t blockNumber) {
    vdi->seek(blockNumber * superBlock.blockSize, VDI_SET);
    vdi->read(buffer, superBlock.blockSize);
}


Directory Ext2::fetchDirectory(uint32_t inodeNumber) {
    Inode inode = fetchInode(inodeNumber);
    uint8_t* contents = new uint8_t[inode.lower32BitsSize];
    for (size_t i = 0; i < inode.lower32BitsSize / superBlock.blockSize; i++)
    {
        //fetchBlockFromInode(&inode, i, contents + i*superBlock.blockSize);
        fetchFileBlock(inode, i, contents + i*superBlock.blockSize);
    }
    return Directory(inode, contents, inodeNumber, superBlock.blockSize);
}

uint32_t Ext2::pathToInodeNumber(const string& path) {
    if (path == "/") return 2;

    int inode = -1;
    string curPath = path;
    Directory dir = fetchDirectory(2);
    dir.open();

    while (dir.getNextEntry()) {
        //cout << dir.getName() << endl;
        string nextPath = moveUpPath(curPath);
        string front = frontDirName(curPath);
        if (front == dir.getName() && nextPath.empty()) {
            inode = dir.getInodeNumber();
            break;
        }
        if (front == dir.getName() && !nextPath.empty()) {
            curPath = nextPath;
            dir.close();
            dir = fetchDirectory(dir.getInodeNumber());
            dir.open();
        }
    }

    dir.close();
    return inode;
}

void Ext2::fetchFileBlock(Inode &inode, int inBlock, uint8_t* buf) {
    int ipb = superBlock.blockSize / 4;
    uint32_t* list;
    if (inBlock < 12) {
        list = inode.pointers;
        goto single;
    } else if (inBlock < 12 + ipb) {
        fetchBlock(buf, inode.pointers[12]);
        list = (uint32_t*)buf;
        inBlock -= 12;
        goto single;
    } else if (inBlock < 12 + ipb + ipb*ipb) {
        fetchBlock(buf, inode.pointers[13]);
        list = (uint32_t*)buf;
        inBlock -= 12 + ipb;
        goto doub;
    }

    fetchBlock(buf, inode.pointers[14]);
    inBlock -= 12 + ipb*ipb;

    // Unwinding
    int b;
    triple:
        b = inBlock / (ipb*ipb);
        inBlock %= ipb*ipb;
        fetchBlock(buf, list[b]);
    doub:
        b = inBlock / ipb;
        inBlock %= ipb;
        fetchBlock(buf, list[b]);
    single:
        fetchBlock(buf, list[inBlock]);
}

void Ext2::fetchFileContents(Inode &inode, uint8_t* buf, int offset, int size) {
    int bytesRead = 0;
    uint8_t tempBuf[superBlock.blockSize];
    while (bytesRead < size) {
        int block = offset / superBlock.blockSize;
        int trueOffset = offset % superBlock.blockSize;
        int bytesToRead = superBlock.blockSize - trueOffset;
        fetchFileBlock(inode, block, tempBuf);
        memcpy(buf + bytesRead, tempBuf + trueOffset, bytesToRead);
        bytesRead += bytesToRead;
        offset += bytesToRead;
    }
}


