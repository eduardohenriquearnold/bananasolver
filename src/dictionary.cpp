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

void Dictionary::filterValidWords(const CharHistogram& hist)
{
    words.erase(std::remove_if(words.begin(), words.end(), [&](const std::string& word) {
        CharHistogram wordHist = createCharHistogram(word);
        return !canFormWord(wordHist, hist, {});
    }), words.end());
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

void filterValidWordsIndex(std::list<size_t>& indices, const Dictionary& dict, const CharHistogram& hist, const CharHistogram& mandatory_hist)
{
    indices.remove_if([&](size_t index) {
        const CharHistogram wordHist = createCharHistogram(dict[index]);
        return !canFormWord(wordHist, hist, mandatory_hist);
    });
}