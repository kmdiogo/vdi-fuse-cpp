#include "../include/vdif_ext2.h"


void ext2Init(VDIFData* private_data) {
    printf("Initializing Ext2 Module...\n");
    private_data->fsData = new Ext2Data(&private_data->vdi);
}

void ext2Destroy(VDIFData* private_data) {
    delete (Ext2Data*)private_data->fsData;
}

void ext2ReadDir(VDIFData* private_data, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    uint32_t dirInodeNum = GET_EXT(private_data).pathToInodeNumber(path);
    Directory dir = GET_EXT(private_data).fetchDirectory(dirInodeNum);
    dir.open();
    while (dir.getNextEntry()) {
        filler(buf, dir.getName().c_str(), NULL, 0);
    }
    dir.close();
}

int ext2GetAttr(VDIFData* private_data, const char *path, struct stat *stbuf) {
    uint32_t inodeNum = GET_EXT(private_data).pathToInodeNumber(path);
    if (inodeNum == -1) return -ENOENT;

    Inode inode = GET_EXT(private_data).fetchInode(inodeNum);
    stbuf->st_size = inode.lower32BitsSize;
    stbuf->st_nlink = inode.hardLinkCount;

    if ((inode.typePermissions & 0xF000u) == 0x4000u) {
        stbuf->st_mode = S_IFDIR | 0755;
    } else {
        stbuf->st_mode = S_IFREG | 0777;
    }
    return 0;
}

void ext2AttachFunctions(VDIFData* private_data) {
    private_data->fsops = (FSOperations){
            .init = ext2Init,
            .destroy = ext2Destroy,
            .readdir = ext2ReadDir,
            .getattr = ext2GetAttr

    };
}
