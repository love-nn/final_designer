#include <iostream>
#include <fstream>

int main() {
    std::ofstream ff ("test.txt",std::ios::app) ;
    ff << "test test test " ;
    return 0;
}