#include "provided.h"
#include "MyMap.h"
#include <string>
using namespace std;

class WordBagImpl
{
public:
    WordBagImpl(const string& text);
    bool getFirstWord(string& word, int& count);
    bool getNextWord(string& word, int& count);
	
private:
	MyMap<string, int> wordToCount;
};

WordBagImpl::WordBagImpl(const string& text)
{
	// get words and counts then associate on MyMap
	string copy = text;
	strToLower(copy);
	Tokenizer t(copy);
	string w;
	while (t.getNextToken(w))
	{
		int* check = wordToCount.find(w);
		if (check != NULL)	// found
		{
			*check = *check + 1;
			continue;
		}
		wordToCount.associate(w, 1);
	}
}

bool WordBagImpl::getFirstWord(string& word, int& count)
{
	int* number = wordToCount.getFirst(word);
	if (number == NULL)
		return false;  
	count = *number;
	return true;
}

bool WordBagImpl::getNextWord(string& word, int& count)
{
	int* number = wordToCount.getNext(word);
	if (number == NULL)
		return false;
    count = *number;
	return true;
}

//******************** WordBag functions *******************************

// These functions simply delegate to WordBagImpl's functions.
// You probably don't want to change any of this code.

WordBag::WordBag(const std::string& text)
{
    m_impl = new WordBagImpl(text);
}

WordBag::~WordBag()
{
    delete m_impl;
}

bool WordBag::getFirstWord(string& word, int& count)
{
    return m_impl->getFirstWord(word, count);
}

bool WordBag::getNextWord(string& word, int& count)
{
    return m_impl->getNextWord(word, count);
}