#include "../disk/disk.h"
#include "../io/file.h"
int main(){
    //RUNS IF TEST 02 HAS BEEN RUN
    //DEMONSTRATES PERSISTANCE BY RUNNING EVEN AFTER EXIT OF TEST01
    init();
    readFile("/movies", "wishlist.txt");

    return 1;

}