#include <iostream>
#include <unordered_set>

#include "board.hpp"

void Board::print() const
{
    // Get minimum and maximum indices
    int minu(0), minv(0), maxu(0), maxv(0);
    for (const auto& [pos, letter] : board)
    {
        if (pos.first < minu) minu = pos.first;
        if (pos.second < minv) minv = pos.second;
        if (pos.first > maxu) maxu = pos.first;
        if (pos.second > maxv) maxv = pos.second;
    }

    // Print Board
    for (int i = minu; i <= maxu; ++i)
    {
        for (int j = minv; j <= maxv; ++j)
        {
            auto it = board.find({i, j});
            if (it != board.end())
            {
                std::cout << it->second;
            }
            else
            {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

std::string Board::uniqueLetters() const
{
    std::unordered_set<char> letters;
    for (const auto& [pos, letter] : board)
    {
        letters.insert(letter);
    }
    return std::string(letters.begin(), letters.end());
}


bool Board::isValid(const Dictionary& dict) const
{
    std::string currentWord;
    for (Board::iterator it=begin(); !it.ended(); ++it)
    {
        if (*it == '\0')
        {
            // End of a word, check for word
            if (!dict.contains(currentWord)) return false;
            currentWord.clear();
        }
        else
        {
            currentWord += *it;
        }
    }
    return true;
}

// Iterator functions
Board::iterator Board::begin() const
{
    return ++iterator(this);
}

Board::iterator& Board::iterator::operator++()
{
    if (operator*() == '\0' && !toCheck.empty())
    {
        u = std::get<0>(toCheck.back());
        v = std::get<1>(toCheck.back());
        horizontal = std::get<2>(toCheck.back());
        visited.insert(toCheck.back());
        toCheck.pop_back();
    }
    else
    {
        if (horizontal)
            u++;
        else
            v++;
    }

    bool exists_adj1 = ptr->board.count(horizontal ? std::make_pair(u, v-1) : std::make_pair(u-1, v)) != 0;
    bool exists_adj2 = ptr->board.count(horizontal ? std::make_pair(u, v+1) : std::make_pair(u+1, v)) != 0;

    int delta = 0;
    // If there is a letter in the first adjacent direction, move until the beginning of the word
    if (exists_adj1)
        while (ptr->board.count(horizontal ? std::make_pair(u, v-delta-1) : std::make_pair(u-delta-1, v)) != 0)
            delta++;

    // If exists in either direction and not already visited, add to toCheck
    if ((exists_adj1 || exists_adj2) && visited.count(horizontal ? std::make_tuple(u, v-delta, false) : std::make_tuple(u-delta, v, true)) == 0)
        toCheck.emplace_back(horizontal ? std::make_tuple(u, v-delta, false) : std::make_tuple(u-delta, v, true));

    return *this;
}

bool Board::iterator::ended() const
{
    return toCheck.empty() && operator*() == '\0';
}

char Board::iterator::operator*() const
{
    if (ptr->board.count({u, v}) == 0) return '\0';
    return ptr->board.at({u, v});
}