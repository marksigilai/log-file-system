#include "../disk/disk.h"
#include "../io/file.h"
int main(){
	//creates a vdisk if one does not exist
	//loads data from previous disk to memory if it exists
    init();

    char* data = "Some stuff to write to file stuff.txt";
    char* data1 = "Other words to be written to info.txt";
	char* data2 = "nrskn s sdfg dfg sfg  dfg sdf g dsfg  dfg";

    //both syntaxes work for root
    createDirectory("", "subjects");
    createFile("/", "stuff.txt");
    createDirectory("", "images");
    createFile("/", "info.txt");

	//writing to a file in root directory
	writeFile("/","stuff.txt", data);
	writeFile("", "info.txt", data1);

	//reading from files stuff and info on root directory should print info
	readFile("/","info.txt");
	readFile("/", "stuff.txt");

    //writing more information to file info.txt
    writeFile("", "info.txt", data2);

    //reading from info.txt should print both data and data2
    readFile("/","info.txt");

    return 1;

//
//     printf("-------------------------------------\n");
//     createDirectory("/subjects", "CSC");
//     createDirectory("/subjects/CSC", "CSC360");
//     createDirectory("/subjects/CSC/CSC360", "Assignment1");
//     createFile("/subjects", "sigi.txt");
//     createFile("/subjects/CSC", "solution.txt");
//     printf("-------------------------------------\n");
//     createFile("/none",  "this.txt");
//     writeFile("/none", "this.txt", data);
//     printf("-------------------------------------\n");
//     writeFile("/subjects", "sigi.txt", data1);
//     writeFile("/subjects", "sigi.txt", data2);
//     printf("-------------------------------------\n");
//     writeFile("/subjects/CSC", "solution.txt", data2);
//     readFile("/subjects", "sigi.txt");
//     //deleteFile("/subjects", "sigi.txt");
//     readFile("/subjects", "sigi.txt");
//     readFile("root", "sigi.txt");
//     saveProgress();
//     //exit(1);
//     printf("-------------------------------------\n");
//     setup();
//     readFile("/subjects", "sigi.txt");
//     readFile("root", "sigi.txt");

}