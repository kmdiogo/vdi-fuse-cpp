cmake_minimum_required(VERSION 3.10)
project(vdi_fuse)

set(CMAKE_CXX_STANDARD 14)

add_executable(vdi_fuse main.cpp src/VDI.cpp include/VDI.h include/utils.h src/utils.cpp include/Ext2.h src/Ext2.cpp include/Ext2/Directory.h src/Ext2/Directory.cpp include/Ext2/Inode.h include/Ext2/Inode.h)