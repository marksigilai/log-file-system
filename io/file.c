#include "../disk/disk.h"
#include <string.h>
#include "file.h"

const static uint32_t MAGIC_NUMBER = 42;
const static uint32_t NUM_INODES = 128;
const  uint32_t POINTERS_PER_INODE = 10;
const int BLOCKSIZE = 512;
const int NUMBLOCKS = 4096;
const int AVAILABLE = 0;
const int UNAVAILABLE = 1;
const int DIRECTORY = 0;
const int REGULAR = 1;

//free list for non allocated blocks?
//second inode is root
struct inode {
    //8
    uint8_t availability;
    //8
    uint8_t inode_number; // Size of file
    //8
    uint8_t flag;//flag only needs to be 1 or 0 so 8 bits is enough?
    //direct[0] will be directory block
    //20
    uint16_t direct[POINTERS_PER_INODE]; // Direct pointers to memory blocks(4096) so u16
    //PROBLEM WITH POINTERS CANT BE SAVED
    struct directory *directory;// pointer to directory if directory
    //PROBLEM
    char filename[16];
};
struct directory_entry {
    uint8_t refinode;//1 byte
    char filename[30];//24 bytes so total of 32
};

struct directory {
    struct directory_entry dir_entries[16];
};



//global map of all inodes in memory
struct inode inodes[NUM_INODES];
struct inode* root = &inodes[0];
//Finding an available block using the frees vector
int findBlock();
//Checking the integrity of the inodes and the blocks
void set_block(unsigned char buffer[], int block_num);
void unset_block(unsigned char buffer[], int block_num);
void print_buffer(unsigned char buffer[], int size);
void init_buffer(unsigned char buffer[], int size);
void create(char* location, char* name, int type);

struct inode* traverse(char *directory);

void init(){
    FILE* disk = fopen("../disk/vdisk", "r");
//    unsigned char buffer[BLOCKSIZE];
//    init_buffer(buffer, BLOCKSIZE);
//    read(disk, 0, buffer);
//    uint32_t magic_number = 0;
//    memcpy(&magic_number, &buffer, sizeof(MAGIC_NUMBER));

    if(disk){
        //if disk was already created, set up inodes with old values
        printf("Vdisk was already initialized, setting up old values...\n");
        fclose(disk);
        //initInodes();
        setup();
    }
    else{
        //initialize everything fresh
        printf("Vdisk was not initialized...\n");
        initLLFS();
        initSuperBlock();
        initInodes();
        initFreesVector();
        init_root();
    }
    fclose(disk);
}

//Disk
void initLLFS(){
    FILE* disk = fopen("../disk/vdisk", "wb+");
    char* init = calloc(BLOCKSIZE*NUMBLOCKS, 1);
    //print_buffer(init, BLOCKSIZE*NUMBLOCKS)
    fwrite(init, 1, BLOCKSIZE*NUMBLOCKS, disk);
    free(init);
    fclose(disk);
}
//Block 0
void initSuperBlock(){
    FILE* disk = fopen("../disk/vdisk", "rb+");
    //char* buffer = (char *)malloc(BLOCKSIZE);
    unsigned char buffer[BLOCKSIZE];
    init_buffer(buffer, BLOCKSIZE);

    memcpy(buffer, &MAGIC_NUMBER, sizeof(MAGIC_NUMBER));
    memcpy(buffer + sizeof(NUMBLOCKS)*1, &NUMBLOCKS, sizeof(NUMBLOCKS));
    memcpy(buffer + sizeof(NUMBLOCKS)*2, &NUM_INODES, sizeof(NUMBLOCKS));
    //memcpy(buffer + sizeof(root)*3, &root, sizeof(root));

    //print_buffer(buffer, BLOCKSIZE);
    write(disk, 0, buffer);
    //fwrite(buffer, BLOCKSIZE, 1, disk);
    fclose(disk);
    //printf("Done initializing superblock");
}
//Block 1
void initFreesVector(){
    FILE* disk = fopen("../disk/vdisk", "rb+");
    unsigned char buffer[BLOCKSIZE];
    init_buffer(buffer, BLOCKSIZE);
    for(int i = 0; i < 10; i++){
        set_block(buffer, i);
    }
    write(disk, 1, buffer);
    fclose(disk);
}

//120 total inodes
//each inode is 48 bytes
//so 5760 bytes needed for inodes
//10 inodes per block and 12 blocks total
void initInodes(){
    FILE* disk = fopen("../disk/vdisk", "rb+");
    unsigned char buffer[BLOCKSIZE];
    init_buffer(buffer, BLOCKSIZE);
    int j, i, counter=0;
    //12 blocks and 10 in each(block 2-14)
    for(j =0; j < 12; j++) {
        for (i = 0; i < 10; i++) {
            struct inode* roo;
            roo = &inodes[counter];
            inodes[counter].inode_number = (uint8_t)counter;
            inodes[counter].availability = AVAILABLE;
            for(int k = 0; k < POINTERS_PER_INODE; k++){
                inodes[counter].direct[k] = (uint16_t)0;
            }

            strcpy(inodes[counter].filename, "");
            memcpy(buffer + sizeof(inodes[counter])*i, &inodes[counter], sizeof(inodes[counter]));
            counter++;
        }
        //printf("j is %d and i is %d\n",j, i);
        //print_buffer(buffer, BLOCKSIZE);
        write(disk, (j+2), buffer);
        init_buffer(buffer, BLOCKSIZE);
    }
    fclose(disk);
}
//obtain inodes in disk to memory
void setup(){
    FILE* disk = fopen("../disk/vdisk", "r");
    unsigned char buffer[BLOCKSIZE];
    init_buffer(buffer, BLOCKSIZE);
    int j, i, counter=0;
    struct inode* roo;
    //12 blocks and 10 in each(block 2-14)
    for(j =0; j < 12; j++) {
        read(disk, (j+2), buffer);
        for (i = 0; i < 10; i++) {
            char buf[48];
            init_buffer((unsigned char*)buf, 48);
            memcpy(buf, buffer + sizeof(buf)*i, sizeof(buf));

            memcpy( &inodes[counter].availability, buf, 1);

            memcpy( &inodes[counter].inode_number, buf + 1, sizeof((uint8_t)inodes[counter].inode_number));

            memcpy( &inodes[counter].flag, buf + 1 + 1, sizeof(inodes[counter].flag));

            memcpy( &inodes[counter].direct, buf + 1 + 1 + 1, sizeof(inodes[counter].direct));

            //memcpy( &inodes[counter].directory, buf + 1 + 1 + 1 + sizeof(inodes[counter].direct), sizeof(inodes[counter].directory));

            memcpy( &inodes[counter].filename, buf + 1 + 1 + 1 + sizeof(inodes[counter].direct) + sizeof(inodes[counter].directory), sizeof(inodes[counter].filename));

            //memcpy(buf, buffer + sizeof(inodes[counter])*i, 47);
            //roo = (struct inode*)buf;
            //memcpy( &inodes[counter], buffer + sizeof(inodes[counter])*i,sizeof(inodes[counter]));
            roo = &inodes[counter];

            counter++;
        }
        init_buffer(buffer, BLOCKSIZE);
    }

    //get all the directory entries from their address in memory
    for(int k = 0; k < 120; k++){
        roo = &inodes[0];
        unsigned char direntries[512];
        unsigned char direntry[32];
        init_buffer((unsigned char*)direntries, 512);
        if(inodes[k].flag == DIRECTORY && inodes[k].availability == UNAVAILABLE){
            read(disk, inodes[k].direct[0], direntries);
            inodes[k].directory = (struct directory*)malloc(sizeof(struct directory));
            for(int l = 0; l < 16; l++){
                memcpy( direntry, direntries + 32*l, 32);
                //memcpy( &inodes[k].directory->dir_entries[l], (struct directory_entry*)direntry, sizeof(struct directory_entry));
                memcpy( &inodes[k].directory->dir_entries[l].refinode, direntry, sizeof(inodes[k].directory->dir_entries[l].refinode));
                memcpy( &inodes[k].directory->dir_entries[l].filename, (char*)(direntry + sizeof(inodes[k].directory->dir_entries[l].refinode)),
                        sizeof(inodes[k].directory->dir_entries[l].filename));
                roo = &inodes[k];
                init_buffer((unsigned char*)direntry, 32);
            }

        }
    }

    fclose(disk);
}
//save inodes to memory and directories to their address
void saveProgress(){
    FILE* disk = fopen("../disk/vdisk", "rb+");
    unsigned char buffer[BLOCKSIZE];
    struct inode* roo;
    int j, i, counter=0;
    //save all the inodes to memory
    //12 blocks and 10 in each(block 2-14)
    for(j =0; j < 12; j++) {
        char buf[48];
        init_buffer((unsigned char*)buf, 48);
        init_buffer(buffer, BLOCKSIZE);
        for (i = 0; i < 10; i++) {
            memcpy(buf, &inodes[counter].availability, 1);

            memcpy(buf + 1, &inodes[counter].inode_number, sizeof((uint8_t)inodes[counter].inode_number));

            memcpy(buf + 1 + 1, &inodes[counter].flag, sizeof(inodes[counter].flag));

            memcpy(buf + 1 + 1 + 1, &inodes[counter].direct, sizeof(inodes[counter].direct));

            //memcpy(buf + 1 + 1 + 1 + sizeof(inodes[counter].direct), &inodes[counter].directory ,sizeof(inodes[counter].directory));

            memcpy(buf + 1 + 1 + 1 + sizeof(inodes[counter].direct) + sizeof(inodes[counter].directory), &inodes[counter].filename, sizeof(inodes[counter].filename));

            memcpy(buffer + sizeof(buf)*i, buf, sizeof(buf));
            roo = &inodes[counter];
            counter++;
        }
        write(disk, (j+2), buffer);
        init_buffer(buffer, BLOCKSIZE);
    }

    fclose(disk);
    disk = fopen("../disk/vdisk", "rb+");
    //save all the directory entries to their address
    for(int k = 0; k < 120; k++){
        roo = &inodes[k];
        unsigned char direntries[512];
        unsigned char direntry[32];
        init_buffer((unsigned char*)direntries, 512);
        if(inodes[k].flag == DIRECTORY && inodes[k].availability == UNAVAILABLE){
            for(int l = 0; l < 16; l++){
                init_buffer((unsigned char*)direntry, 32);
                roo = &inodes[k];
                memcpy( direntry, &inodes[k].directory->dir_entries[l].refinode, sizeof(inodes[k].directory->dir_entries[l].refinode));
                memcpy( direntry + sizeof(inodes[k].directory->dir_entries[l].refinode), &inodes[k].directory->dir_entries[l].filename,
                        sizeof(inodes[k].directory->dir_entries[l].filename));
                //memcpy( direntry, &inodes[k].directory->dir_entries[l], sizeof(struct directory_entry));
                memcpy( direntries + 32*l, direntry, 32);
                //print_buffer(direntries, 100);
            }
            write(disk, inodes[k].direct[0], direntries);
        }
    }

    fclose(disk);
}

//obtain root value from disk
void init_root(){
    root = &inodes[0];
    struct inode* roo;
    roo = &inodes[0];
    root->availability = UNAVAILABLE;
    root->flag = DIRECTORY;
    strcpy(root->filename , "root");
    FILE* disk = fopen("../disk/vdisk", "rb+");
    //int block = findBlock();
    root->direct[0] = (uint16_t)findBlock();
    root->directory = (struct directory*)malloc(sizeof(struct directory));
    updateFreesVector(root->direct[0], UNAVAILABLE);
    unsigned char buffer[BLOCKSIZE];
    read(disk, 0, buffer);
    memcpy(buffer + sizeof(root)*3, &root, sizeof(root));
    write(disk, 0, buffer);
    //store the directory in disk mem
    init_buffer(buffer, BLOCKSIZE);
    memcpy(buffer, root->directory, sizeof(*root->directory));
    write(disk, root->direct[0], buffer);
    fclose(disk);
}

void updateFreesVector(int blocknum, int newval){

    FILE* disk = fopen("../disk/vdisk", "rb+");
    unsigned char buffer[BLOCKSIZE];
    read(disk, 1, buffer);
    if(newval == AVAILABLE){
        unset_block(buffer, blocknum);
    }else{
        set_block(buffer, blocknum);
    }

    write(disk, 1, buffer);
    fclose(disk);
}

void updateBlock(int blocknum, char* data){
    FILE* disk = fopen("../disk/vdisk", "rb+");
    unsigned char buffer[BLOCKSIZE];
    init_buffer(buffer, BLOCKSIZE);
    memcpy(buffer, data, strlen(data));
    //print_buffer(buffer, BLOCKSIZE);
    write(disk, blocknum, buffer);
    //printf("sdfgfdsg\n\n\n\n");
    //read(disk, blocknum, buffer);
    //print_buffer(buffer, BLOCKSIZE);
    fclose(disk);
}

//stores directory in disk and it's address in the inode
void createDirectory(char* location, char* name){
    create(location, name, DIRECTORY);
    saveProgress();
}

void createFile(char* location, char* name){
    create(location, name, REGULAR);
    saveProgress();
}

void create(char* location, char* name, int type){
    //allocate and initialize inode map inode and filename
    //find a free inode and allocate it
    //printf("%s\n", inodes[0].filename);
    //find an available inode
    int i;
    for(i =0; i < NUM_INODES; i++){
        struct inode roo;
        roo = inodes[i];
        if(inodes[i].availability == AVAILABLE){
            inodes[i].availability = UNAVAILABLE;
            inodes[i].flag = (uint8_t)type;
            strcpy(inodes[i].filename, name);
            break;
        }
    }
    //printf("Here\n");

    struct inode* dir = traverse(location);
    if(!dir){
        inodes[i].availability = AVAILABLE;
        return;
    }
    printf("-> creating (%s) in directory (%s)\n", name, dir->filename);
    //find and set the location's inode to point to the new file's inode
    for(int j = 0; j < 16; j++) {
    	if(strcmp(dir->directory->dir_entries[j].filename, name) == 0){
    		printf("-> The file (%s) already exists in the directory (%s)\n", name, dir->filename);
    		inodes[i].availability = AVAILABLE;
    		return;
    	}
        if (strcmp(dir->directory->dir_entries[j].filename, "") == 0) {
            //dir->directory->dir_entries[j] = &dir_entry1;
            dir->directory->dir_entries[j].refinode = inodes[i].inode_number;
            strcpy(dir->directory->dir_entries[j].filename, name);
            break;
        }
    }
    //allocate 1 memory - make the inode point to the memory address allocated
    if(type == REGULAR){
        //writing to a regular file will allocate a block and populate
        return;
    }else if(type == DIRECTORY){
        inodes[i].direct[0] = (uint16_t)findBlock();
        updateFreesVector(inodes[i].direct[0], UNAVAILABLE);
        inodes[i].directory = (struct directory*)malloc(sizeof(struct directory));
        for(int k = 0; k < 16; k++){
            strcpy(inodes[i].directory->dir_entries[k].filename, "");
            inodes[i].directory->dir_entries[k].refinode = (uint16_t)0;
        }
        return;
    }

    printf("-> something in create() failed");
    //make info persistent

}

//returns the inode of the last directory name in the list
//traverses the tree from root
struct inode* traverse(char *directory){
    char dirs[100];
    memset(dirs, '\0', 30);
    strcpy(dirs, directory);
    char str[30];
    memset(str, '\0', 30);
    int k = 0;
    int l = 0;
    //string tokenizer for the directory
    struct inode *current = root;
    while(strncmp(&dirs[k], "\0", 1) != 0){
        while(strncmp(&dirs[k], "/", 1) != 0 && strncmp(&dirs[k], "\0", 1) != 0){
            strncpy(&str[l], &dirs[k], 1);
            k++;
            l++;
        }
        //strncpy(&str[l+1], "\n", 1);
        //l=0;
        k++;
        if(strcmp(str, "") != 0){
            //printf("This is rot %s \n", current->directory->dir_entries[0]->refinode->filename);
            int i = 0;
            for(int j = 0; j < 16; j++){
                if(strncmp(current->directory->dir_entries[j].filename, str, (size_t)l) == 0){
                    current = &inodes[current->directory->dir_entries[j].refinode];
                    i++;
                    j=0;
                    //break;
                }
            }
        }
        l=0;
        memset(str, '\0', 30);
    }
    if(strlen(directory) > 2 ){
        if(strcmp(current->filename, "root") == 0){
            printf("-> The directory was not found\n");
            return 0;
        }
    }
    return current;
}

//Deleting a file – from a given directory
void deleteFile(char* directory, const char* name){
    //make the inode for the directory dereference?
    //make the blocks free on the free list?
    struct inode* victim = NULL;

    //traverse to the last directory where the file is located
    //returns it's inode
    struct inode* dir = traverse(directory);
    if(!dir){
        return;
    }
    //free parent directory's ref
    for (int j = 0; j < 16; j++) {
        if (strcmp(dir->directory->dir_entries[j].filename, name) == 0){
            //printf("-> Found the file to delete, removing...\n");
            victim = &inodes[dir->directory->dir_entries[j].refinode];
            //free(dir->directory->dir_entries[j]);
            strcpy(dir->directory->dir_entries[j].filename, "");
            dir->directory->dir_entries[j].refinode = (uint8_t)NULL;
        }
    }

    //if victim is a file deallocate its blocks and inode
    if(victim && victim->flag == REGULAR){
        victim->availability = AVAILABLE;
        for (int j = 0; j < POINTERS_PER_INODE; j++) {
            if (victim->direct[j] != (uint16_t) NULL) {
                //FREE THE BLOCK OF MEMORY REFERENCED use unset
                printf("-> Deleting file %s and Freeing memory block...%d...\n", victim->filename,victim->direct[j]);
                updateFreesVector(victim->direct[j], AVAILABLE);
                //victim->direct[j] = (uint16_t)NULL;
            }
        }
    }
    //if victim is a directory check if its empty
    else if(victim && victim->flag == DIRECTORY){
        for (int j = 0; j < 16; j++) {
            if (dir->directory->dir_entries[j].refinode != (uint8_t)NULL) {
                printf("-> The directory is not empty we cannot delete, exiting...\n");
                return;
            }
        }
        //empty so free inode and deallocate directory block
        victim->availability = AVAILABLE;
        updateFreesVector(victim->direct[0], AVAILABLE);
    }
    saveProgress();

}

//Writing to a file – append to an existing file
void writeFile(char* directory, const char* name, char* data){

    //write to a free block and save its block number
    struct inode* dir = traverse(directory);
    if(!dir){
        return;
    }
    //printf("-----------------------------------%ld\n", sizeof(struct inode));
    for(int j = 0; j < 16; j++){
        if(strcmp(dir->directory->dir_entries[j].filename, name) == 0){
            //if file provided is a directory, error
            if(inodes[dir->directory->dir_entries[j].refinode].flag == DIRECTORY){
                printf("-> The file (%s) is a directory, cannot write,  exiting...\n", name);
                return;
            }
            //allocate a new block to the file
            for(int i = 0; i < 16; i++){
                if(inodes[dir->directory->dir_entries[j].refinode].direct[i] == 0){
                    uint16_t allocated_block = (uint16_t)findBlock();
                    inodes[dir->directory->dir_entries[j].refinode].direct[i] = allocated_block;
                    updateFreesVector(allocated_block, UNAVAILABLE);
                    updateBlock(allocated_block, data);
                    return;
                }
            }
        }
    }
    saveProgress();

}

//Reading from a file – load a file from disk into memory
void readFile(char* directory, const char* filename){
    unsigned char buffer[BLOCKSIZE];
    FILE* disk = fopen("../disk/vdisk", "rb+");
    struct inode* dir = traverse(directory);
    if(!dir){
        return;
    }
    for(int i = 0; i < 16; i++){
        if(strcmp(inodes[dir->directory->dir_entries[i].refinode].filename, filename) == 0){
            if(inodes[dir->directory->dir_entries[i].refinode].flag == DIRECTORY){
                printf("-> Error: Cannot read a directory\n");
                return;
            }
            printf("-> Printing the file (%s) \n",filename);
            for(int j = 0; j < POINTERS_PER_INODE; j++){
                if(inodes[dir->directory->dir_entries[i].refinode].direct[j]){
                    int blocknum =  inodes[dir->directory->dir_entries[i].refinode].direct[j];
                    read(disk, blocknum, buffer);
                    printf("%s\n", (char*) buffer);
                }
            }
            return;
        }
    }
    printf("-> Could not find file (%s) in the directory (%s) \n", filename, directory);

}

//Finding an available block using the frees vector
int findBlock(){
    FILE* disk = fopen("../disk/vdisk", "rb+");
    unsigned char buffer[BLOCKSIZE];
    read(disk, 1, buffer);
    //print_buffer(buffer, BLOCKSIZE);
    //bitwise and for every byte in the block
    for(int i = 0; i < 512; i++){
        if(~buffer[i] & 1){
            return 8*i + 0;
        }
        if(~buffer[i] & 2){
            return 8*i + 1;
        }
        if(~buffer[i] & 4){
            return 8*i + 2;
        }
        if(~buffer[i] & 8){
            return 8*i + 3;
        }
        if(~buffer[i] & 16){
            return 8*i + 4;
        }
        if(~buffer[i] & 32){
            return 8*i + 5;
        }
        if(~buffer[i] & 64){
            return 8*i + 6;
        }
        if(~buffer[i] & 128){
            return 8*i + 7;
        }
    }
    return -1;
}

//set blocknum bit to unavailable
void set_block(unsigned char buffer[], int block_num)
{
    int index = block_num / 8;
    int bit_index = block_num % 8;

    buffer[index] |= 1UL << bit_index;
}

//set blocknum bit to available
void unset_block(unsigned char buffer[], int block_num)
{
    int index = block_num / 8;
    int bit_index = block_num % 8;
    buffer[index] &= ~(1UL << bit_index);
}
void print_buffer(unsigned char buffer[], int size)
{
    printf("%04x: ", 0);
    for (int i = 1; i <= size; i++) {
        printf("%02x ", buffer[i-1]);
        if(i % 8 == 0)
        {
            printf("\n");
            printf("%04x: ", i);
        }
    }
    printf("\n");
}
void init_buffer(unsigned char buffer[], int size)
{
    for (int i = 0; i < size; i++) {
        buffer[i] = 0x0;
    }
}

























