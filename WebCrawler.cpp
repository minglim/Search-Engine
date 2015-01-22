#include "provided.h"
#include "http.h"
#include <string>
#include <list>
using namespace std;

class WebCrawlerImpl
{
public:
    void addUrl(string url);
    int getNumberOfUrls() const;
    void crawl(void (*callback)(string url, bool success));
    bool save(string filenameBase);
    bool load(string filenameBase);

private:
	list<string> m_url;
	Indexer m_index;
};

void WebCrawlerImpl::addUrl(string url)
{
	m_url.push_back(url);
}

int WebCrawlerImpl::getNumberOfUrls() const
{
	return m_url.size();
}

void WebCrawlerImpl::crawl(void (*callback)(string url, bool success))
{
	while (m_url.size() != 0)
	{
		// use HTTP class to download data from the internet
		string site = m_url.front();
		m_url.pop_front();
		string page;
		bool download = true;
		if (!HTTP().get(site, page))
			download = false;
		else
		{
			// create WordBag and incorporate if data is successfully downloaded
			WordBag w(page);
			if (!m_index.incorporate(site, w))
				download = false;
		}
		// call the callback function
		callback(site, download);
	}
}

bool WebCrawlerImpl::save(string filenameBase)
{
    return m_index.save(filenameBase);
}

bool WebCrawlerImpl::load(string filenameBase)
{
    return m_index.load(filenameBase);
}

//******************** WebCrawler functions *******************************

// These functions simply delegate to WebCrawlerImpl's functions.
// You probably don't want to change any of this code.

WebCrawler::WebCrawler()
{
    m_impl = new WebCrawlerImpl;
}

WebCrawler::~WebCrawler()
{
    delete m_impl;
}

void WebCrawler::addUrl(string url)
{
    m_impl->addUrl(url);
}

int WebCrawler::getNumberOfUrls() const
{
    return m_impl->getNumberOfUrls();
}

void WebCrawler::crawl(void (*callback)(string url, bool success))
{
    m_impl->crawl(callback);
}

bool WebCrawler::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool WebCrawler::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
