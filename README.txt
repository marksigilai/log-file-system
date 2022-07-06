Mark Sigilai


The log file system implemented here is able to create, write to and delete files and directories.  These are stored the disk.

Block 0 contains the superblock with information about the disk
Block 1 contains the frees vector that is initialized 0's, each bit represents whether a block is free(0) or not(1)
Block 2-14 contain inodes storage, with each block containing 10 inodes of size 48bytes each. Total of 120 inodes available for use with inodes[0] always being the root inode
Blocks 14-4096 are free for data storage

All inodes are managed in the array of 120 inodes and when values are changed, the function saveProgress() saves the new inode information to the disk.
This is done after the completion of a create, write or delete instead of after each change due to how tasking reading and writing is to the system.
There is a possible loss of inode information is an exit occurs during an action and before the saveProgress() has been executed.

Directories are allocated a new block when created. Entries are 32 bytes each, and 16 entries are saved on one directory block.
Made persistent when corresponding inodes are saved through saveProgress().

Writes to files are allocated a new block each time with the new data. The block is written to immediately and is therefore always persistent, thus no risk with a possible exit/shutdown.
It is possible to inplement a buffer to store writes and then save this once the buffer is full, but this brings risk.
Possible issue with this system is that a new block is always allocated for each write which is wasteful if blocks contain little information


TESTS
--------

Test 3 must be run after test2 as it shows the persistence of the system by reinitializing the data from test2 and reading it.
This shows that the system does not lose information after a shutdown.

The function init() must be called before tests.
