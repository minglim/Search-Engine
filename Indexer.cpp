#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class IndexerImpl
{
public:
    bool incorporate(string url, WordBag& wb);
    vector<UrlCount> getUrlCounts(string word);
    bool save(string filenameBase);
    bool load(string filenameBase);

private:
	struct IdCounts
	{
		int id;
		int count;
	};
	MyMap<string, int> urlToId;
	MyMap<int, string> idToUrl;
	MyMap<string, vector<IdCounts> > wordToIdCounts;
	void writeItem(ostream& stream, string& s)
	{
		stream << s << endl;
	}
	void writeItem(ostream& stream, int& i)
	{
		stream << i << endl;
	}
	void writeItem(ostream& stream, vector<IdCounts>& v)
	{
		// put the size of the vector before the list in order to differentiate between vectors
		int i = v.size();
		stream << i << endl;
		for (int i = 0; i < v.size(); i++)
		{
			stream << v[i].count << endl;
			stream << v[i].id << endl;
		}
	}
	template<typename KeyType, typename ValueType>
	bool saveMyMap(string filename, MyMap<KeyType,ValueType>& m)
	{
		ofstream stream(filename);
		if (!stream)
			return false;
		stream << m.size() << endl;
		KeyType temp;
		for (ValueType* ptr = m.getFirst(temp); ptr != NULL; ptr = m.getNext(temp))
		{
			writeItem(stream, temp);
			writeItem(stream, *ptr);
		}
		return true;
	}
	bool readItem(istream& stream, string& s)
	{
		return getline(stream, s);
	}
	bool readItem(istream& stream, int& i)
	{
		string line;
		if ( ! getline(stream, line)) // return false if getline fails
			return false;
		// create an input stringstream whose text will be a copy of line
		istringstream iss(line); // #include <sstream> for istringstream
		// Try to extract an int from the stream, returning true if it
		// succeeds, or false if it fails (because the line has no
		// integer). 
		return iss >> i;
	}
	bool readItem(istream& stream, vector<IdCounts>& v)
	{
		// first integer will always be the size of the vector; use this integer in the for loop
		int k;
		if (!readItem(stream, k))
			return false;
		for (int i = 0; i < k; i++)
		{
			int x;
			if (!readItem(stream, x))
				return false;
			IdCounts temporary;
			temporary.count = x;
			int j;
			if (!readItem(stream, j))
				return false;
			temporary.id = j;
			v.push_back(temporary);
		}
		return true;
	}
	template<typename KeyType, typename ValueType>
	bool loadMyMap(string filename, MyMap<KeyType,ValueType>& m)
	{
		m.clear();
		ifstream stream(filename);
		if (!stream)
			return false;
		int k;
		stream >> k;
		stream.ignore(10000, '\n');
		if (!stream)
			return false;
		for (int i = 0; i < k; i++)
		{
			KeyType key;
			if ( ! readItem(stream, key))
				return false;
			ValueType value;
			if ( ! readItem(stream, value))
				return false;
			m.associate(key, value);
		}
		return true;
	}
};

bool IndexerImpl::incorporate(string url, WordBag& wb)
{
	// Was Url already incorporated?
	int* check = urlToId.find(url);
	if (check != NULL)
		return false;
	// use random in order to create a balanced tree
	string* temp;
	int id;
	do
	{
		id = rand() % 1000000000;
		temp = idToUrl.find(id);
	}
	while (temp != NULL);
	// this saves memory by only storing the Url twice
	urlToId.associate(url, id);
	idToUrl.associate(id, url);
	string word;
	int count;
	// loop through map and incorporate Url with IdCount
	bool gotAWord = wb.getFirstWord(word, count);
	while (gotAWord)
	{
		IdCounts temporary;
		temporary.id = *(urlToId.find(url));
		temporary.count = count;
		vector<IdCounts>* v = wordToIdCounts.find(word);
		if (v == NULL)
		{
			vector<IdCounts> newVector;
			newVector.push_back(temporary);
			wordToIdCounts.associate(word, newVector);
			gotAWord = wb.getNextWord(word,count);
		}
		else
		{
			(*v).push_back(temporary);
			gotAWord = wb.getNextWord(word, count);
		}
	}
    return true;  
}

vector<UrlCount> IndexerImpl::getUrlCounts(string word)
{
	// create a new UrlCount vector and convert all the ID's to Urls
	strToLower(word);
	vector<IdCounts>* found = wordToIdCounts.find(word);
	if (found != NULL)
	{
		vector<UrlCount> result;
		for (int i = 0; i < (*found).size(); i++)
		{
			UrlCount temporary;
			string* temp = idToUrl.find((*found)[i].id);
			temporary.url = *temp;
			temporary.count = (*found)[i].count;
			result.push_back(temporary);
		}
		return result;
	}
	vector<UrlCount> empty;
    return empty;  
}

bool IndexerImpl::save(string filenameBase)
{
    return saveMyMap(filenameBase + ".u2i", urlToId) &&
		saveMyMap(filenameBase + ".i2u", idToUrl) &&
		saveMyMap(filenameBase + ".w2ic", wordToIdCounts); 
}

bool IndexerImpl::load(string filenameBase)
{
    return loadMyMap(filenameBase + ".u2i", urlToId) &&
		loadMyMap(filenameBase + ".i2u", idToUrl) &&
		loadMyMap(filenameBase + ".w2ic", wordToIdCounts);
}

//******************** Indexer functions *******************************

// These functions simply delegate to IndexerImpl's functions.
// You probably don't want to change any of this code.

Indexer::Indexer()
{
    m_impl = new IndexerImpl;
}

Indexer::~Indexer()
{
    delete m_impl;
}

bool Indexer::incorporate(string url, WordBag& wb)
{
    return m_impl->incorporate(url, wb);
}

vector<UrlCount> Indexer::getUrlCounts(string word)
{
    return m_impl->getUrlCounts(word);
}

bool Indexer::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool Indexer::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}