//
// Created by Kenny on 2/19/2020.
//

#ifndef VDI_FUSE_UTILS_H
#define VDI_FUSE_UTILS_H

#include <cstdint>
#include <iostream>

using namespace std;

void printBytes(const uint8_t* buffer, size_t size, const char* description);
string moveUpPath(string path);
string frontDirName(string path);

#endif //VDI_FUSE_UTILS_H
