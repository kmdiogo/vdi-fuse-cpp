//
// Created by Kenny on 2/23/2020.
//

#ifndef VDI_FUSE_VDIF_STRUCTS_H
#define VDI_FUSE_VDIF_STRUCTS_H

#include "fuse.h"
#include "VDI.h"

typedef struct VDIFData VDIFData;

struct FSOperations {
    void (*init) (VDIFData* private_data);
    void (*destroy) (VDIFData* private_data);
    void (*readdir) (VDIFData* private_data, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
    int (*getattr)(VDIFData* private_data, const char *path, struct stat *stbuf);
    int (*read)(VDIFData* private_data, const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
};

struct VDIFData {
    char* vdiFilePath;
    char* vdiFs;
    VDI vdi;

    void* fsData;
    FSOperations fsops;
};

#endif //VDI_FUSE_VDIF_STRUCTS_H
