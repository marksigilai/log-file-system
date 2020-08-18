#include "../disk/disk.h"
#include "../io/file.h"
int main(){
    init();
    // the files


    char* data = "General ideas and instructions for the assignment";
    char* data1 = "Pulp Fiction, Other Movie, Etc";
	char* data2 = "Seven - 10/10";

    //files subjects exists from test01
    createDirectory("/", "assignments");
    createDirectory("/assignments", "CSC");
    createFile("/assignments/CSC", "assignment1.txt");
    createDirectory("/", "movies");
    createFile("/movies", "wishlist.txt");
    createDirectory("/movies", "thrillers");
    createFile("/movies/thrillers", "ratings.txt");

	//writing to a file in root directory
	writeFile("/assignments/CSC","assignment1.txt", data);
	writeFile("/movies", "wishlist.txt", data1);
	writeFile("/movies/thrillers","ratings.txt" , data2);

	//writing to a file that does not exist - should return error
	writeFile("/", "subjects", data2);

	//reading from files stuff and info on root directory should print info
	readFile("assignments/CSC","assignment1.txt");
	readFile("/movies", "wishlist.txt");
	readFile("/movies/thrillers", "ratings.txt");

	//delete a directory that is not empty should return error
    deleteFile("movies", "");

    //delete ratings.txt and print it should return error
    deleteFile("/movies/thrillers", "ratings.txt");
    readFile("/movies/thrillers", "ratings.txt");


    return 1;


}