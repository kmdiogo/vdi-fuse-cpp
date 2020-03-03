//
// Created by Kenny on 2/23/2020.
//

#ifndef VDI_FUSE_VDIF_EXT2_H
#define VDI_FUSE_VDIF_EXT2_H

#define GET_EXT(vdif_data) (((Ext2Data*)vdif_data->fsData)->ext)
#define GET_TABLE(vdif_data) (((Ext2Data*)vdif_data->fsData)->dirInodeTable)

#include "Ext2/Ext2.h"
#include "vdif_structs.h"
#include <map>
#include <stack>

class Ext2Data {
public:
    Ext2Data(VDI* vdi) : ext(vdi) {}
    Ext2 ext;
};

void ext2AttachFunctions(VDIFData* private_data);
void ext2Init(VDIFData* private_data);
void ext2Destroy(VDIFData* private_data);
void ext2ReadDir(VDIFData* private_data, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int ext2GetAttr(VDIFData* private_data, const char *path, struct stat *stbuf);
int ext2Read(VDIFData* private_data, const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

#endif //VDI_FUSE_VDIF_EXT2_H
