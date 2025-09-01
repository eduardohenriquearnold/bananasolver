#include <iostream>
#include <algorithm>
#include "dictionary.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <letters>" << std::endl;
        return 1;
    }

    std::string pool(argv[1]);
    std::transform(pool.begin(), pool.end(), pool.begin(), ::toupper);

    Dictionary dict("../dictionary.txt");
    dict.filterValidWords(pool);

    for (const auto& word: dict.words) {
        std::cout << word << std::endl;
    }
    return 0;
}