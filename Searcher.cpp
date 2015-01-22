#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// used to connect url to it's score
struct relevantUrlAndScore
{
	string url;
	int score;
};
// needed because sorting in descending order in scores
bool customCompare(const relevantUrlAndScore &a, const relevantUrlAndScore &b)
{
	if (a.score > b.score)
		return true;	// a should be before b
	return false;	// a should be after b
}
class SearcherImpl
{
public:
    vector<string> search(string terms);
    bool load(string filenameBase);

private:
	Indexer m_i;
	vector<string> m_terms;
	vector<UrlCount> m_data;
	vector<UrlCount> m_relevantData;
	vector<string> m_url;
	vector<string> m_relevantUrl;
	vector<relevantUrlAndScore> m_result;
};

vector<string> SearcherImpl::search(string terms)
{
	// clear all private member vectors before beginning a new pass through search
	m_terms.clear();
	m_data.clear();
	m_relevantData.clear();
	m_url.clear();
	m_relevantUrl.clear();
	m_result.clear();
	// make all terms lower case, tokenize them, and create a vector of unique words
	strToLower(terms);
	Tokenizer t(terms);
	string w;
	while (t.getNextToken(w))
	{
		int i;
		for (i = 0; i < m_terms.size(); i++)
		{
			if (w == m_terms[i])
				break;
		}
		if (i == m_terms.size())
			m_terms.push_back(w);
	}
	// calculate N and T
	int N = m_terms.size();
	int T;
	int calculate = 0.7*N;
	if (calculate > 1)
		T = calculate;
	else
		T = 1;
	// traverse through every word and put all the UrlCount vectors each word maps to in one vector
	for (int k = 0; k < m_terms.size(); k++)
	{
		vector<UrlCount> temp = m_i.getUrlCounts(m_terms[k]);
		m_data.insert(m_data.end(), temp.begin(), temp.end());
	}
	// create a vector of strings containing all unique Urls
	for (int m = 0; m < m_data.size(); m++)
	{
		vector<string>::iterator a, b, iter;
		a = m_url.begin();
		b = m_url.end();
		iter = find(a, b, m_data[m].url);
		if (iter == b)
			m_url.push_back(m_data[m].url);
	}
	// create a vector of relevant Urls; do this by counting how many times each Url appears in the
	// UrlCount vector; if the count is equivalent to N, then the Url is relevant and should be added to
	// the vector
	vector<string>::iterator a, b, iter;
	if (!m_url.empty())
	{
		a = m_url.begin();
		b = a+1;
	}
	for (int n = 0; n < m_url.size(); n++)
	{
		int count = 0; 
		int i = 0;
		while (i < m_data.size())
		{
			iter = find(a, b, m_data[i].url);
			if (iter != b)	// found
			{
				count++;
				if (count == T)
				{
					m_relevantUrl.push_back(m_url[n]);
					break;
				}
				i++;
			}
			else
				i++;
		}
		if (n+1 != m_url.size())
		{
			a=a+1;
			b=b+1;
		}
	}
	// create a vector of relevant UrlCount; will be useful when computing the score of each site;
	// do this by finding all the vectors in m_data that contain a relevant Url and store these vectors
	// in a new vector
	if (!m_relevantUrl.empty())
	{
		a = m_relevantUrl.begin();
		b = a+1;
	}
	for (int i = 0; i < m_relevantUrl.size(); i++)
	{
		int k = 0;
		while (k < m_data.size())
		{
			iter = find(a, b, m_data[k].url);
			if (iter != b)	// found
			{
				m_relevantData.push_back(m_data[k]);
				k++;
			}
			else
				k++;
		}
		if (i+1 != m_relevantUrl.size())
		{
			a=a+1;
			b=b+1;
		}
	}
	// compute score of each Url and put it in a vector along with the Url;
	// find the all the UrlCount vectors that contain the relevant Url and sum up the counts;
	// the sum is the score for the Url
	if (!m_relevantUrl.empty())
	{
		a = m_relevantUrl.begin();
		b = a+1;
	}
	for (int i = 0; i < m_relevantUrl.size(); i++)
	{
		int score = 0;
		int k = 0;
		while (k < m_relevantData.size())
		{
			iter = find(a, b, m_relevantData[k].url);
			if (iter != b)	// found
			{
				score += m_relevantData[k].count;
				k++;
			}
			else
				k++;
		}
		relevantUrlAndScore temp;
		temp.url = m_relevantUrl[i];
		temp.score = score;
		m_result.push_back(temp);
		if (i+1 != m_relevantUrl.size())
		{
			a=a+1;
			b=b+1;
		}
	}
	// sort in descending order based on score
	sort(m_result.begin(), m_result.end(), customCompare);
	// create new vector and copy all the Urls from m_result to the new vector;
	// return the new vector to finish the function
	vector<string> searchResult;
	for (int i = 0; i < m_result.size(); i++)
	{
		searchResult.push_back(m_result[i].url);
	}
	return searchResult;
}

bool SearcherImpl::load(string filenameBase)
{
    return m_i.load(filenameBase);

}

//******************** Searcher functions *******************************

// These functions simply delegate to SearcherImpl's functions.
// You probably don't want to change any of this code.

Searcher::Searcher()
{
    m_impl = new SearcherImpl;
}

Searcher::~Searcher()
{
    delete m_impl;
}

vector<string> Searcher::search(string terms)
{
    return m_impl->search(terms);
}

bool Searcher::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
