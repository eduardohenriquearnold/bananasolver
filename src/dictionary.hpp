#pragma once
#include <string>
#include <vector>
#include <unordered_map>

typedef std::unordered_map<char, int> CharHistogram;


struct Dictionary
{
    Dictionary(const std::string& filename);
    // Find all valid words that can be formed with given characters
    std::vector<size_t> validWordsIndices(const std::string& characters) const;
    // Find all valid words that can be formed with given characters, containing at least 1 mandatory letters
    std::vector<size_t> validWordsIndices(const std::string& characters, const std::string& mandatory_letters) const;
    void filterValidWords(const std::string& characters);
    bool contains(const std::string& word) const;

    std::vector<std::string> words;
};


CharHistogram createCharHistogram(const std::string& str);
bool canFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist);

