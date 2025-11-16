#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "dictionary.hpp"
#include "board.hpp"

bool solve(const Dictionary& dict, Board& board, std::string pool) {
    // Implement the backtracking algorithm to find all valid words

    // Steps
    // 1. Find all valid words formed with current pool + 1 letter already in board
    // 2. Sort by largest length DESC
    // 3. Get word from top, try to place it in the board
    // 4. If it could be placed, check for valid board (all words touching are valid)
    // 5. If valid, repeat from (1) until pool is empty
    //    If invalid or not placed, reset board and get next word from list. If list empty, return False (back-track).
    if (pool.empty())
        return true;

    Board oldBoard(board);

    // 1. Find all valid words formed with current pool + 1 letter already in board
    std::vector<size_t> validWordsIdx = dict.validWordsIndices(pool, board.uniqueLetters());

    // 2. Sort by largest length DESC
    std::sort(validWordsIdx.begin(), validWordsIdx.end(), [&](size_t a, size_t b) {
        return dict.words[a].length() > dict.words[b].length();
    });

    // 3. Try to Place words
    for (const auto& idx : validWordsIdx) {
        const std::string& word = dict.words[idx];
        if (board.addWord(word)) {
            // If the word was successfully added, check board is valid
            if (board.isValid(dict)) {
                // Update the pool by removing the used letters
                std::string new_pool(pool);
                for (char c : word)
                {
                    auto it = std::find(new_pool.begin(),new_pool.end(),c);
                    if (it != new_pool.end())
                        new_pool.erase(it);
                }
                // Recursively call for the next iteration
                if (solve(dict, board, new_pool))
                    return true;
            }
            // In case of failure, reset the board to previous state
            board = oldBoard;
        }
    }

    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <letters> [dictionary_path]" << std::endl;
        return 1;
    }

    // Set dictionary path to default or user-provided value
    const std::string dictionaryPath = (argc > 2) ? argv[2] : "./dictionary.txt";

    // Get upper-case version of the letters available in the pool
    std::string pool(argv[1]);
    std::transform(pool.begin(), pool.end(), pool.begin(), ::toupper);

    // Initialize and filter dictionary to contain only feasible words given pool
    Dictionary dict(dictionaryPath);
    dict.filterValidWords(pool);

    // Create board and find solution
    Board board;
    if (solve(dict, board, pool)) {
        board.print();
    } else {
        std::cout << "No solution exists." << std::endl;
    }
    return 0;
}