#include <fstream>
#include <algorithm>

#include "dictionary.hpp"

Dictionary::Dictionary(const std::string& filename)
{
    // Load words from the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open dictionary file: " + filename);
    }
    // Expects line break delimiters
    std::string word;
    while (std::getline(file, word)) {
        words.push_back(word);
    }
}

std::vector<size_t> Dictionary::validWordsIndices(const CharHistogram& hist, const CharHistogram& mandatory_hist={}) const
{
    std::vector<size_t> validIndices;
    for (size_t i = 0; i < words.size(); ++i) {
        // TODO: potentially pre-compute and store histograms for all dict words to speed this up
        const CharHistogram wordHist = createCharHistogram(words[i]);
        if (canFormWord(wordHist, hist, mandatory_hist))
            validIndices.push_back(i);
    }
    return validIndices;
}


void Dictionary::filterValidWords(const CharHistogram& hist)
{
    std::vector<size_t> validIndices = validWordsIndices(hist);
    std::vector<std::string> filteredWords;
    filteredWords.reserve(validIndices.size());
    for (size_t index : validIndices) {
        filteredWords.push_back(words[index]);
    }
    words = std::move(filteredWords);
}

bool Dictionary::contains(const std::string& word) const
{
    // TODO: if slow, optimize with hash set
    return std::find(words.begin(), words.end(), word) != words.end();
}

CharHistogram createCharHistogram(const std::string& str)
{
    CharHistogram hist;
    for (const char& ch: str)
    {
        if (hist.contains(ch))
            hist[ch]++;
        else
            hist[ch] = 1;
    }
    return hist;
}

bool canFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist, const CharHistogram& mandatoryHist)
{
    bool mandatory_found = mandatoryHist.empty();
    for (const auto& [ch, count]: wordHist)
    {
        if (auto search = poolHist.find(ch); search != poolHist.end())
        {
            if (search->second < count)
                return false;
        }
        else
            return false;

        if (!mandatory_found && mandatoryHist.contains(ch))
            mandatory_found = true;
    }
    return mandatory_found;
}