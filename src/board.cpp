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
    for (int j = minv; j <= maxv; ++j)
    {
        for (int i = minu; i <= maxu; ++i)
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
    for (Board::iterator it=begin();; ++it)
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
        if (it.ended()) break;
    }
    return true;
}

bool Board::addWord(const std::string& word)
{
    // If board is empty, add word at (0,0) horizontally
    if (board.empty())
    {
        for (int i = 0; i < word.length(); ++i)
            board[{i, 0}] = word[i];
        return true;
    }

    // Try to place the word in where there are common letters, always orthogonally
    // We don't try to add the letter in parallel because our assumption is that
    // the longest words have already been placed first.
    for (Board::iterator it=begin(); !it.ended(); ++it)
    {
        if (*it == '\0')
            continue;

        // Check if current letter matches any letter in the word
        for (int w_idx = 0; w_idx < word.length(); ++w_idx)
        {
            // Skip until finding a common letter
            if (*it != word[w_idx])
                continue;

            // Check that word can be placed by checking it won't overlap with
            // other letters            
            bool feasible = true;
            for (int w_check=0; w_check<word.length() && feasible; ++w_check)
            {
                if (w_check == w_idx) continue;
                auto pos = it.horizontal? std::make_pair(it.u, it.v+w_check-w_idx) : std::make_pair(it.u+w_check-w_idx, it.v);
                feasible = board.count(pos) == 0;
            }

            if (feasible)
            {
                for (int w_check=0; w_check<word.length(); ++w_check)
                {
                    auto pos = it.horizontal? std::make_pair(it.u, it.v+w_check-w_idx) : std::make_pair(it.u+w_check-w_idx, it.v);
                    board[pos] = word[w_check];
                }
                return true;
            }
        }
    }

    return false;
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

    if (operator*() == '\0')
        return *this;

    bool exists_adj1 = ptr->board.count(horizontal ? std::make_pair(u, v-1) : std::make_pair(u-1, v)) != 0;
    bool exists_adj2 = ptr->board.count(horizontal ? std::make_pair(u, v+1) : std::make_pair(u+1, v)) != 0;

    // If there is a letter in the first adjacent direction, get delta to the beginning of the word
    int delta = 0;
    if (exists_adj1)
        while (ptr->board.count(horizontal ? std::make_pair(u, v-delta-1) : std::make_pair(u-delta-1, v)) != 0)
            delta++;

    // If exists in either direction and not already visited, add to toCheck
    auto new_pos = horizontal ? std::make_tuple(u, v-delta, false) : std::make_tuple(u-delta, v, true);
    if ((exists_adj1 || exists_adj2) && visited.count(new_pos) == 0)
    {
        toCheck.push_back(new_pos);
        visited.insert(new_pos);
    }

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