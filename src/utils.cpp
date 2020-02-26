//
// Created by Kenny on 2/19/2020.
//
#include "../include/utils.h"

void printBytes(const uint8_t* buffer, size_t size, const char* description)
{
    printf("\n%s\n", description);
    for(size_t i = 0; i<size; i++)
    {
        printf("%X ", buffer[i]);
    }
    printf("\n");
}

string moveUpPath(string path) {
    // "/hello/world.txt -> "/world.txt"
    // /hello/ -> ""
    int start = path.length();
    int end = path[path.length()-1] == '/' ? path.length()-1 : path.length();
    for (int i = 1; i < end; i++) {
        if (path[i] == '/') {
            start = i;
            break;
        }
    }
    return path.substr(start, path.length()-1);
}

string frontDirName(string path) {
    string name;
    for (int i = 1; i < path.length(); i++) {
        if (path[i] == '/') {
            break;
        } else {
            name += path[i];
        }
    }
    return name;
}