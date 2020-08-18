#include <stdlib.h>
#include <stdio.h>

void read(FILE* disk, int blocknum, unsigned char buffer[]);
void write(FILE* disk, int blocknum, unsigned char data[]);