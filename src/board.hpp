#pragma once
#include <string>
#include <unordered_map>
#include "dictionary.hpp"

// Hash function for pair<unsigned int, unsigned int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 + (hash2 * 1024); // Combine the two hash values
    }

    std::size_t operator() (const std::tuple<int, int, bool>& tuple) const {
        auto hash1 = std::hash<int>{}(std::get<0>(tuple));
        auto hash2 = std::hash<int>{}(std::get<1>(tuple));
        auto hash3 = std::hash<bool>{}(std::get<2>(tuple));
        return hash3 + (hash1 * 1024) + (hash2 * 1024 * 1024); // Combine the three hash values
    }
};


class Board 
{
private:
    //Board is represented as a map of (x,y) -> letter
    // X -> horizontal axis, pointing right
    // Y -> vertical axis, pointing down
    std::unordered_map<std::pair<int, int>, char, pair_hash> board;

    struct iterator {
        int u, v;
        bool horizontal;
        const Board* ptr;
        std::vector<std::tuple<int, int, bool>> toCheck;
        std::unordered_set<std::tuple<int, int, bool>, pair_hash> visited;

        iterator(const Board* boardPtr) : u(-1), v(0), horizontal(true), ptr(boardPtr) {if (boardPtr->board.size()) toCheck.emplace_back(0, 0, true);}
        iterator& operator++();
        bool ended() const;
        char operator*() const;
    };
public:
    Board(){}
    Board(const Board& other): board(other.board) {}

    void print() const;
    size_t size() const { return board.size(); }

    // Checks if all words on the board are valid (any word touching)
    bool isValid(const Dictionary& dict) const;

    // Return histogram of letters on the board
    CharHistogram getHistogram() const;

    // Try to add a word to the board
    bool addWord(const std::string& word, char intersectionChar);

    // Iterators
    iterator begin() const;
};