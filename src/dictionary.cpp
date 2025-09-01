#include "dictionary.hpp"
#include <fstream>

Dictionary::Dictionary(const std::string& filename)
{
    // Load words from the file
    // Expects line break delimiters
    std::ifstream file(filename);
    std::string word;
    while (std::getline(file, word)) {
        words.push_back(word);
    }
}

std::vector<size_t> Dictionary::validWordsIndices(const std::string& characters) const
{
    std::vector<size_t> validIndices;
    CharHistogram pool = createCharHistogram(characters);
    for (size_t i = 0; i < words.size(); ++i) {
        CharHistogram wordHist = createCharHistogram(words[i]);
        if (canFormWord(wordHist, pool))
            validIndices.push_back(i);
    }
    return validIndices;
}

void Dictionary::filterValidWords(const std::string& characters)
{
    std::vector<size_t> validIndices = validWordsIndices(characters);
    std::vector<std::string> filteredWords;
    for (size_t index : validIndices) {
        filteredWords.push_back(words[index]);
    }
    words = std::move(filteredWords);
}



CharHistogram createCharHistogram(const std::string& str)
{
    CharHistogram hist;
    for (const char& ch: str)
    {
        if (hist.contains(ch))
            hist[ch]++;
        else
            hist[ch] = 0;
    }
    return hist;
}

bool canFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist)
{
    for (const auto& [ch, count]: wordHist)
    {
        if (auto search = poolHist.find(ch); search != poolHist.end())
        {
            if (search->second < count)
                return false;
        }
        else
            return false;
    }
    return true;
}