#include "console.hpp"

void Console::print(const std::string &text){
    std::cout << text << std::endl;
}

int Console::scan() {
    int i;
    std::cin >> i;
    return i;
}
