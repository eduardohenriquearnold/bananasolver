#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <numeric>

#include "dictionary.hpp"
#include "board.hpp"

// Remove letters used in wordHist from pool (except intersectionChar which is already on the board)
void updatePool(CharHistogram& pool, const CharHistogram& wordHist, char intersectionChar) {
    for (const auto& [ch, count] : wordHist)
    {
        int usedCount = count;
        if (ch == intersectionChar)
            usedCount -= 1; // One letter is already on the board

        if (usedCount > 0)
        {
            if (!pool.contains(ch))
                throw std::runtime_error("Error: Word uses letters not in the pool.");

            pool[ch] -= usedCount;
            if (pool[ch] == 0)
                pool.erase(ch);
            else if (pool[ch] < 0)
                throw std::runtime_error("Error: Word uses more letters than available in the pool.");
        }
    }
}

bool solve(const Dictionary& dict, Board& board, CharHistogram pool, std::vector<size_t> validWordsIdx) {
    // Implement the backtracking algorithm to find all valid words

    // Steps
    // 1. Remove from validWordsIdx all words that cannot be formed with current pool (weak check)
    // 2. Get word from top, try to place it in the board
    //    Because dictionary is sorted by length descending, we get longest words first
    // 4. If it could be placed, check for valid board (all words touching are valid)
    // 5. If valid, repeat from (1) until pool is empty
    //    If invalid or not placed, reset board and get next word from list. If list empty, return False (back-track).
    if (pool.empty())
        return true;

    const Board oldBoard(board);
    const CharHistogram boardHist = board.getHistogram();

    // 1. Remove words that will never be able to be formed with current pool
    {
        size_t current_valid = 0;
        for (size_t i = 0; i < validWordsIdx.size(); ++i) {
            const CharHistogram& wordHist = dict.getWordHistogram(validWordsIdx[i]);
            if (weaklyCanFormWord(wordHist, pool))
                validWordsIdx[current_valid++] = validWordsIdx[i];
        }
        validWordsIdx.resize(current_valid);
    }

    // 2. Given valid words, see if they can be formed given current pool + board letters
    //    If so, try to add them to the board
    for (const auto& idx : validWordsIdx) 
    {
        const std::string& word = dict[idx];
        const CharHistogram& wordHist = dict.getWordHistogram(idx);
        char intersectionChar;
        if (!canFormWord(wordHist, pool, boardHist, &intersectionChar))
            continue;

        if (board.addWord(word, intersectionChar)) 
        {
            // If the word was successfully added, check board is valid
            if (board.isValid(dict))
            {
                // Update the pool by removing the used letters
                CharHistogram newPool(pool);
                updatePool(newPool, wordHist, intersectionChar);
                // Recursively call for the next iteration
                if (solve(dict, board, newPool, validWordsIdx))
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
    std::string pool_str(argv[1]);
    std::transform(pool_str.begin(), pool_str.end(), pool_str.begin(), ::toupper);
    CharHistogram pool = createCharHistogram(pool_str);

    // Initialize and filter dictionary to contain only feasible words given pool
    Dictionary dict(dictionaryPath);
    dict.filterValidWords(pool);

    // Initialize valid words indices
    std::vector<size_t> validWordsIdx(dict.size());
    std::iota(validWordsIdx.begin(), validWordsIdx.end(), 0);

    // Create board and find solution
    Board board;
    if (solve(dict, board, pool, validWordsIdx)) {
        std::cout << board;
        // Sanity check to confirm all letters in pool were used
        if (board.getHistogram() != pool) {
            throw std::runtime_error("Error: Not all letters were used in the solution.");
        }
    } else {
        std::cout << "No solution exists." << std::endl;
    }
    return 0;
}