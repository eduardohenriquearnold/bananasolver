#pragma once
#include <string>
#include <vector>
#include <list>
#include <unordered_map>

typedef std::unordered_map<char, int> CharHistogram;


struct Dictionary
{
    // Load dictionary from file
    Dictionary(const std::string& filename);

    // Remove all words that cannot be formed with given characters histogram
    void filterValidWords(const CharHistogram& hist);

    // Check if word is in dictionary
    bool contains(const std::string& word) const;

    const std::string& operator[](size_t index) const { return words[index]; }
    size_t size() const { return words.size(); }

    private:
        std::vector<std::string> words;
};


CharHistogram createCharHistogram(const std::string& str);

// Check if a word (given its histogram) can be formed with given pool histogram and a single letter from mandatory histogram.
// If mandatoryHist is empty, check if word can be formed with poolHist alone.
// If intersectionChar is provided, set it to the character from mandatoryHist used in the word (if mandatoryHist is provided).
bool canFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist, const CharHistogram& mandatoryHist, char* intersectionChar=nullptr);

// Given initial list of word indices for a dictionary, filter out those that cannot be formed with given histogram and mandatory histogram.
void filterValidWordsIndex(std::list<size_t>& indices, const Dictionary& dict, const CharHistogram& hist, const CharHistogram& mandatory_hist={});
