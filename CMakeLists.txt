cmake_minimum_required(VERSION 3.10)
project(vdi_fuse)

set(CMAKE_CXX_STANDARD 14)
add_definitions("-D_FILE_OFFSET_BITS=64")

add_executable(vdi_fuse main.cpp src/VDI.cpp include/VDI.h include/utils.h src/utils.cpp include/Ext2/Ext2.h src/Ext2/Ext2.cpp include/Ext2/Directory.h src/Ext2/Directory.cpp include/Ext2/Inode.h include/Ext2/Inode.h include/vdif_structs.h include/vdif_ext2.h src/vdif_ext2.cpp)