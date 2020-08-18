CC := gcc
CFLAGS := -g -Wall -Wno-deprecated-declarations -Werror

all: test01 test02 test03

clean:
	rm -rf test01 test02 test03 ../disk/vdisk test01.dSYM/ test02.dSYM/ test03.dSYM/

test01: test01.c ../disk/disk.c ../io/file.c
	$(CC) $(CFLAGS) test01.c ../disk/disk.c ../io/file.c -o test01

test02: test02.c ../disk/disk.c ../io/file.c
	$(CC) $(CFLAGS) test02.c ../disk/disk.c ../io/file.c -o test02

test03: test03.c ../disk/disk.c ../io/file.c
	$(CC) $(CFLAGS) test03.c ../disk/disk.c ../io/file.c -o test03

