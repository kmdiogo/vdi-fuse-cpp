/*
gcc VDIFuse.c `pkg-config fuse --cflags --libs` -o VDIFuse

~/Desktop/VDIFuse/a.out -d -s -f ~/Desktop/VDIMount
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <iostream>

#include "include/VDI.h"
#include "include/vdif_structs.h"

// VDIF Filesystem modules
#include "include/vdif_ext2.h"

#define VDIF_DATA ((VDIFData *) fuse_get_context()->private_data)

static const char *filepath = "/file";
static const char *filename = "file";
static const char *filecontent = "I'm the content of the only file available there\n";

static int vdif_getattr(const char *path, struct stat *stbuf) {
    //printf("getattr %s\n", path);
    memset(stbuf, 0, sizeof(struct stat));

    return VDIF_DATA->fsops.getattr(VDIF_DATA, path, stbuf);

    /*if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (strcmp(path, filepath) == 0) {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(filecontent);
        return 0;
    }*/



    return -ENOENT;
}

static int vdif_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {
    //printf("readdir %s %d\n", path, offset);
    (void) offset;
    (void) fi;

    /*filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    filler(buf, filename, NULL, 0);*/

    VDIF_DATA->fsops.readdir(VDIF_DATA, path, buf, filler, offset, fi);

    return 0;
}

static int vdif_open(const char *path, struct fuse_file_info *fi) {
    //printf("open %s\n", path);
    return 0;
}

static int vdif_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    printf("Name: %s  Size: %zu  Offset: %ld\n", path, size, offset);
    int s = VDIF_DATA->fsops.read(VDIF_DATA, path, buf, size, offset, fi);
    printf("Returned size: %d\n", s);
    return s;
    //printf("read %s %zu %ld\n", path, size, offset);
    /*if (strcmp(path, filepath) == 0) {
        size_t len = strlen(filecontent);
        if (offset >= len) {
            return 0;
        }

        if (offset + size > len) {
            memcpy(buf, filecontent + offset, len - offset);
            return len - offset;
        }

        memcpy(buf, filecontent + offset, size);
        return size;
    }*/

    return -ENOENT;
}

static void* vdif_init(struct fuse_conn_info *conn) {
    // Note from Joseph J. Pfeiffer, Jr., Ph.D. <pfeiffer@cs.nmsu.edu>  :
    // Undocumented but extraordinarily useful fact:  the fuse_context is
    // set up before this function is called, and
    // fuse_get_context()->private_data returns the user_data passed to
    // fuse_main().  Really seems like either it should be a third
    // parameter coming in here, or else the fact should be documented
    // (and this might as well return void, as it did in older versions of
    // FUSE).
    printf("---- VDIF INIT ---\n");

    // Assign appropriate file system action function pointers
    // Extend this when adding more filesystem support
    if (strcmp(VDIF_DATA->vdiFs, "ext2") == 0) {
        ext2AttachFunctions(VDIF_DATA);
    }

    VDIF_DATA->fsops.init(VDIF_DATA);
    printf("VDIF Initialization complete.\n");

    // NOTE: VDI.open is done in main so that the .vdi file will get checked before starting FUSE
    return VDIF_DATA;
}

static void vdif_destroy(void* private_data) {
    printf("---- VDIF DESTROY ----\n");

    // Cleanup private data
    VDIF_DATA->vdi.close();
    VDIF_DATA->fsops.destroy(VDIF_DATA);
    printf("VDIF Destruction Complete.\n");
}

fuse_operations fuse_ops;
void fillFuseOperations() {
    fuse_ops.getattr = vdif_getattr;
    fuse_ops.open = vdif_open;
    fuse_ops.read = vdif_read;
    fuse_ops.readdir = vdif_readdir;
    fuse_ops.init = vdif_init;
    fuse_ops.destroy = vdif_destroy;
}

int main(int argc, char *argv[])
{
    fillFuseOperations();
    // Command line argument parsing
    const int SUPPORTED_FS_SIZE = 1;
    const char* supportedFs[SUPPORTED_FS_SIZE] = {"ext2"};
    char* vdiFs = argv[argc-1];
    bool isFsInvalid = true;
    for (size_t i = 0; i < SUPPORTED_FS_SIZE; i++) {
        if (strcmp(vdiFs, supportedFs[i]) == 0) {
            isFsInvalid = false;
        }
    }
    if (isFsInvalid) {
        printf("Sorry, '%s' filesystems are currently not supported by vdi-fuse\n", vdiFs);
        return 0;
    }

    printf("Welcome to VDI Fuse!\n");

    VDIFData vdif_data;
    vdif_data.vdiFilePath = argv[argc-2];
    vdif_data.vdiFs = argv[argc-1];
    vdif_data.vdi = VDI();
    if (!vdif_data.vdi.open(vdif_data.vdiFilePath)) {
        printf("Could not open vdi file %s", vdif_data.vdiFilePath);
        return -1;
    }
    return fuse_main(argc-2, argv, &fuse_ops, &vdif_data);
}
