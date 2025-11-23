#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "dictionary.hpp"
#include "board.hpp"

// Update the pool histogram by removing letters used in the new board compared to the old board
void updatePool(const CharHistogram& oldBoard, const CharHistogram& newBoard, CharHistogram& pool)
{
    for (const auto& [ch, newCount] : newBoard)
    {
        int oldCount = oldBoard.contains(ch) ? oldBoard.at(ch) : 0;
        int usedCount = newCount - oldCount;
        if (usedCount > 0)
        {
            if (pool.contains(ch))
            {
                pool[ch] -= usedCount;
                if (pool[ch] == 0)
                    pool.erase(ch);
                else if (pool[ch] < 0)
                    throw std::runtime_error("Error: Board uses more letters than available in the pool.");
            }
            else
            {
                throw std::runtime_error("Error: Board uses letters not in the pool.");
            }
        }
    }
}

bool solve(const Dictionary& dict, Board& board, CharHistogram pool) {
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
    std::vector<size_t> validWordsIdx = dict.validWordsIndices(pool, board.getHistogram());

    // 2. Sort by largest length DESC
    std::sort(validWordsIdx.begin(), validWordsIdx.end(), [&](size_t a, size_t b) {
        return dict[a].length() > dict[b].length();
    });

    // 3. Try to Place words
    for (const auto& idx : validWordsIdx) {
        const std::string& word = dict[idx];
        if (board.addWord(word)) {
            // If the word was successfully added, check board is valid
            if (board.isValid(dict)) {
                // Update the pool by removing the used letters
                CharHistogram newPool(pool);
                updatePool(oldBoard.getHistogram(), board.getHistogram(), newPool);
                // Recursively call for the next iteration
                if (solve(dict, board, newPool))
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

    // Create board and find solution
    Board board;
    if (solve(dict, board, pool)) {
        board.print();
        // Sanity check to confirm all letters in pool were used
        if (board.getHistogram() != pool) {
            throw std::runtime_error("Error: Not all letters were used in the solution.");
        }
    } else {
        std::cout << "No solution exists." << std::endl;
    }
    return 0;
}