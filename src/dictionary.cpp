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

    // Pre-compute histograms for all words
    wordHistograms.reserve(words.size());
    for (const auto& w : words)
        wordHistograms.push_back( createCharHistogram(w) );
}

std::vector<size_t> Dictionary::validWordsIndices(const CharHistogram& hist, const CharHistogram& mandatory_hist) const
{
    std::vector<size_t> validIndices;
    // Reserve max possible size to avoid multiple allocations
    validIndices.reserve(words.size());
    for (size_t i = 0; i < words.size(); ++i) {
        const CharHistogram& wordHist = getWordHistogram(i);
        if (canFormWord(wordHist, hist, mandatory_hist))
            validIndices.push_back(i);
    }
    return validIndices;
}


void Dictionary::filterValidWords(const CharHistogram& hist)
{
    std::vector<size_t> validIndices = validWordsIndices(hist);

    // Sort words by length descending, which will be useful for solving algorithm
    std::sort(validIndices.begin(), validIndices.end(), [&](size_t a, size_t b) {
        return words[a].length() > words[b].length();
    });

    std::vector<std::string> filteredWords;
    filteredWords.reserve(validIndices.size());
    for (size_t index : validIndices) {
        filteredWords.push_back(words[index]);
    }
    words = std::move(filteredWords);

    // Filter wordHistograms accordingly
    std::vector<CharHistogram> filteredHistograms;
    filteredHistograms.reserve(validIndices.size());
    for (size_t index : validIndices) {
        filteredHistograms.push_back(wordHistograms[index]);
    }
    wordHistograms = std::move(filteredHistograms);

    // Create wordSet for fast lookup
    for (const auto& w : words) {
        wordSet.insert(w);
    }
}

bool Dictionary::contains(const std::string& word) const
{
    return wordSet.contains(word);
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

bool canFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist, const CharHistogram& mandatoryHist, char* intersectionChar)
{
    int mandatory_used = 0;
    for (const auto& [ch, count]: wordHist)
    {
        int count_in_pool = poolHist.contains(ch) ? poolHist.at(ch) : 0;
        if (count_in_pool < count)
        {
            if (count - count_in_pool > 1 || !mandatoryHist.contains(ch) || mandatory_used>0)
                return false;
            mandatory_used++;
            if (intersectionChar)
                *intersectionChar = ch;
        }
    }

    if (!mandatoryHist.empty())
    {
        // Have already used one mandatory character
        if (mandatory_used == 1)
            return true;

        // Haven't used a mandatory character yet, check if there is at least one
        // overlapping character with mandatoryHist and use the first one found
        // Ideally we would return all possible intersection characters, but that
        // would require modifications to the add word logic to try all possibilities.
        for (const auto& [ch, count]: wordHist)
        {
            if (mandatoryHist.contains(ch))
            {
                if (intersectionChar)
                    *intersectionChar = ch;
                return true;
            }
        }

        return false;
    }
    return true;
}

// Checks that no more than 2 letters are missing
// 1 or 0 missing letters is acceptable in case one of them could be found in
// the mandatory histogram
bool weaklyCanFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist)
{
    int missing_letters = 0;
    for (const auto& [ch, count]: wordHist)
    {
        int count_in_pool = poolHist.contains(ch) ? poolHist.at(ch) : 0;
        if (count_in_pool < count)
        {
            missing_letters += (count - count_in_pool);
            if (missing_letters >= 2)
                return false;
        }
    }
    return true;
}