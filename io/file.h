#include <stdlib.h>
#include <stdio.h>


#include "../disk/disk.h"
#include <string.h>

//initialize the LLFS vdisk
void initLLFS();

//initialize the superblock
void initSuperBlock();
//update root values between memory and disk
void update_root();

//obtain root value from disk
void init_root();

void initFreesVector();

void updateFreesVector(int blocknum, int newval);

void updateBlock(int blocknum, char* data);
//128 total inodes
//each inode is 32 bytes
//so 4096 bytes needed for inodes
//16 inodes per block and 8 blocks total
void initInodes();

/*128 total inodes each inode is 32 bytes so 4096 bytes needed for inodes
16 inodes per block and 8 blocks total CREATE INODE FOR THE ROOT DIRECTORY
*/
void setInodes();

//Creating a file or directory – given some directory location, name and type
//type 1 - file
//type 0 - directory
void createFile(char* location, char* name);
void createDirectory(char* location, char* name);

//Deleting a file – from a given directory
void deleteFile(char* directory, const char* name);

//Writing to a file – append to an existing file
void writeFile(char* directory, const char* name, char* data);

//Reading from a file – load a file from disk into memory
void readFile(char* directory, const char* filename);

void saveProgress();

void setup();


void init();





















