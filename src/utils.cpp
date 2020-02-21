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