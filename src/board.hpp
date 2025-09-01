#include <string>
#include <unordered_map>
#include "dictionary.hpp"

class Board 
{
public:
    Board();
    Board(const Board& other);

    void print() const;

    // Checks if all words on the board are valid (any word touching)
    bool isValid(const Dictionary& dict) const;

    // Get all unique letters already in the board
    std::string uniqueLetters() const;

    // Try to add a word to the board
    bool addWord(const std::string& word);

private:



};