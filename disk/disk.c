#include "disk.h"
#include <stdio.h>

const int BLOCK_SIZE = 512;
const int NUM_BLOCKS = 4096;

void read(FILE* disk, int blocknum, unsigned char buffer[]){
    fseek(disk, blocknum*BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, disk);
}

void write(FILE* disk, int blocknum, unsigned char data[]){
    fseek(disk, blocknum*BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, disk);
}