#include <unistd.h>

#include <iostream>
int main(int argc, char *argv[]) {
    std::cout << "wait_begin" << std::endl;
    fflush(stdout);
    sleep(20);
    std::cout << "wait_end" << std::endl;
    fflush(stdout);
    return 0;
}