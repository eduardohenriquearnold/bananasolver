#include <string>
#include <vector>
#include <unordered_map>

typedef std::unordered_map<char, int> CharHistogram;


struct Dictionary
{
    Dictionary(const std::string& filename);
    std::vector<size_t> validWordsIndices(const std::string& characters) const;
    void filterValidWords(const std::string& characters);

    std::vector<std::string> words;
};


CharHistogram createCharHistogram(const std::string& str);
bool canFormWord(const CharHistogram& wordHist, const CharHistogram& poolHist);

