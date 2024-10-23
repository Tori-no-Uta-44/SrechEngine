//
// Created by 李勃鋆 on 24-10-15.
//

#ifndef SEARCHENGINESERVER_H
#define SEARCHENGINESERVER_H
#include "nlohmann/json.hpp"
#include "Configuration.h"
#include "func.h"
#include <WFFacilities.h>
#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>
#include <workflow/WFHttpServer.h>
#include "CppJieba.h"
#include "Dictionary.h"
#include "LRU.h"
#include <workflow/RedisMessage.h>

#include "SubLRUCache.h"
using namespace wfrest;

class SearchEngineServer {
public:
	explicit SearchEngineServer(int count, CppJieba *jieba);

	void start(unsigned short port);

	string urlDecode(const std::string &str);

	void loadModules();

private:
	void loadDictionary();

	void loadWeb();

	void loadSearchWeb();

	string getFromCacheOrRedis(const string &key, HttpResp *resp, SeriesWork *series);

	string getFromCacheOrRedis(const string &key, HttpResp *resp, SeriesWork *series, SubLRUCache &subLRU);

	void queryRedis(const std::string &key, HttpResp *resp, SeriesWork *series);

	void handleRedisResponse(const protocol::RedisValue &redis_value, const string &key, HttpResp *resp,
	                         SeriesWork *series);

	void handleCacheMiss(const std::string &key, HttpResp *resp, SeriesWork *series);

	std::string executeQuery(const std::string &decoded_info);

	void syncSubLRUWithGlobal(SubLRUCache &subLRU);

	WFFacilities::WaitGroup _waitGroup;
	HttpServer _httpserver;
	Configuration *_conf;
	Dictionary *_dictionary;
	CppJieba *_jieba;
	LRU _cache;
	unordered_map<std::thread::id, SubLRUCache> _subLRUMap;
};
#endif //SEARCHENGINESERVER_H
