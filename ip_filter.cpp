#include <algorithm>
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>

using TVecStr = std::vector<std::string>;
using TFile_ip4 = std::vector<TVecStr>;
using TSize = std::string::size_type;

/* pool ip adresses */
TFile_ip4 ip_pool;

/* checkStr */
/*****************************************************************************/
template<typename Ts>
bool checkStr(
		TVecStr &par,
		int n,
		Ts arg0)
{
	/* check */
	assert(arg0 < 255);
	return (par[n] == std::to_string(arg0));
}

/* checkStr */
/*****************************************************************************/
template<class Ts, class... Args>
bool checkStr(
		TVecStr &par,
		int n,
		Ts arg0,
		Args... args)
{
	return checkStr(par, n, arg0) && checkStr(par, n+1, args...);
}

/* filter */
/*****************************************************************************/
template<class ...Args>
TFile_ip4 filter(Args ...args)
{
    /* result pool */
	TFile_ip4 result;

#if defined(LAMBDA)
    std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(res),
        [&](TVecStr &par)
        {
            return checkStr(par, 0, args...);
        });
#else
    for(auto pool : ip_pool)
    {
    	if(checkStr(pool, 0, args...))
    		result.push_back(pool);
    }
#endif

    return result;
}

/* split */
/*****************************************************************************/
TVecStr split(const std::string &str, char d)
{
    TVecStr tmp;
    TSize start = 0;
    TSize stop = str.find_first_of(d);

    while (stop != std::string::npos)
    {
        tmp.push_back(str.substr(start, (stop - start)));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    tmp.push_back(str.substr(start));

    return tmp;
}

/* sort part ip adress */
/*****************************************************************************/
bool sortPart(
		const TVecStr& ip1,
		const TVecStr& ip2,
		unsigned int part)
{
	/* if size parts are same */
	if(ip1[part].size() == ip2[part].size())
		return ip1[part] > ip2[part];
	else
		/* true is bigger size */
		return ip1[part].size() > ip2[part].size();
}

/* sort next part id adress (if previous part is same) */
/*****************************************************************************/
bool sortNextPart(
		const TVecStr& ip1,
		const TVecStr& ip2,
		unsigned int part)
{
	/* check */
	if(part == (ip1.size() - 1))
		return false;

	/* if parts are same */
	if(ip1[part] == ip2[part])
		return sortNextPart(ip1, ip2, part + 1);
	else
		/* sort current part ip */
		return sortPart(ip1, ip2, part);
}

/* sortReverseOrder */
/*****************************************************************************/
void sortReverseOrder(TFile_ip4& ip_pool)
{
	/* predicator for reverse sorted ip pool */
    struct {
        bool operator()(const TVecStr& ip1, const TVecStr& ip2) const
        {
        	return sortNextPart(ip1, ip2, 0);
        }
    } sortPred;

	/* sort */
	std::sort(ip_pool.begin(), ip_pool.end(), sortPred);

}

/* filter_any */
/*****************************************************************************/
TFile_ip4 filter_any(const int num)
{
	/* local ip pool with sorted elements */
	TFile_ip4 sortPool;
	/* check */
	assert(num < 255);

#if defined(LAMBDA)
	/* any_of */
	std::any_of(ip_pool.begin(), ip_pool.end(),
			[=](const TVecStr& ip1)
			{
				return ip1[0] == std::to_string(num);
			});
#else
	for(auto pool : ip_pool)
	{
		for(int i = 0; i < 4; ++i)
			if(pool[i] == std::to_string(num))
				sortPool.push_back(pool);
	}
#endif

	return sortPool;
}

/* printIpPool */
/*****************************************************************************/
void printIpPool(const TFile_ip4& ip_pool)
{
	for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
	{
		for(auto ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
		{
			if(ip_part != ip->cbegin())
				std::cout<< ".";
			std::cout<<*ip_part;
		}
		std::cout<<std::endl;
	}
}

/* main func */
/*****************************************************************************/
int main()
{
	try
	{
		/* parser */
		for (std::string line; std::getline(std::cin, line);)
		{
		   TVecStr v = split(line, '\t');
		   ip_pool.push_back(split(v.at(0), '.'));
		}

		/* reverse lexicographically sort */
		sortReverseOrder(ip_pool);
		/* print ip_pool in reverse order */
		printIpPool(ip_pool);

		/* filter for first part ip same "1" */
		TFile_ip4 ip_pool46 = filter(1);
		/* print filtered pool */
		printIpPool(ip_pool46);

		/* filter for first part ip same "46" and second - "70" */
		TFile_ip4 ip_pool46_70 = filter(46, 70);
		/* print filtered pool */
		printIpPool(ip_pool46_70);

		/* filter for any part ip same as 46 */
		TFile_ip4 ip_pool_any46 = filter_any(46);
		/* print ip_pool_any46 */
		printIpPool(ip_pool_any46);
	}
	catch(const std::exception& exc)
	{
		std::cerr << exc.what() << std::endl;
	}

	return 0;
}
