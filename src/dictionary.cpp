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

std::vector<size_t> Dictionary::validWordsIndices(const std::string& characters) const
{
    std::vector<size_t> validIndices;
    const CharHistogram pool = createCharHistogram(characters);
    for (size_t i = 0; i < words.size(); ++i) {
        // TODO: potentially pre-compute and store histograms for all dict words to speed this up
        const CharHistogram wordHist = createCharHistogram(words[i]);
        if (canFormWord(wordHist, pool))
            validIndices.push_back(i);
    }
    return validIndices;
}

// Return valid word indices that can be formed with characters and must include at least one mandatory letter
std::vector<size_t> Dictionary::validWordsIndices(const std::string& characters, const std::string& mandatory_letters) const
{
    if (mandatory_letters.empty())
        return validWordsIndices(characters);

    std::vector<size_t> validIndices, validIndices_cur;
    for (const char& c: mandatory_letters)
    {
        validIndices_cur = validWordsIndices(characters + std::string(1, c));
        for (const auto& idx : validIndices_cur) {
            const std::string& word = words[idx];
            if (word.find(c) != std::string::npos) {
                validIndices.push_back(idx);
            }
        }
    }

    // Remove non-unique indices
    std::sort(validIndices.begin(), validIndices.end());
    validIndices.erase(std::unique(validIndices.begin(), validIndices.end()), validIndices.end());
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