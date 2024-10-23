//
// Created by 李勃鋆 on 24-10-15.
//
#include "../include/SearchEngineServer.h"

#include "../include/Dictionary.h"
#include "../include/KeyRecommander.h"
#include "../include/WebPageQuery.h"

SearchEngineServer::SearchEngineServer(int count, CppJieba *jieba):
	_waitGroup(count),
	_jieba(jieba),
	_cache(32) {
	_conf = Configuration::getInstance();
	_dictionary = Dictionary::getInstance();

	_dictionary->init("/root/a/SearchEngine/data/");
}

void SearchEngineServer::start(unsigned short port) {
	if (_httpserver.track().start(port) == 0) {
		_httpserver.list_routes();
		_waitGroup.wait();
		_httpserver.stop();
	}
	else {
		cerr << "Server Start Failed" << endl;
	}
}

void SearchEngineServer::loadModules() {
	loadDictionary();
	loadWeb();
}

string SearchEngineServer::urlDecode(const std::string &str) {
	std::string decoded;
	size_t i = 0;
	while (i < str.length()) {
		if (str[i] == '%') {
			if (i + 2 < str.length()) {
				char hex_str[3] = {str[i + 1], str[i + 2], '\0'};
				char *endptr;
				unsigned long hex_value = strtoul(hex_str, &endptr, 16);
				// 检查转换是否成功
				if (*endptr == '\0') {
					char ch = static_cast<char>(hex_value);
					decoded += ch;
					i += 3;
					continue;
				}
			}
		}
		decoded += str[i];
		i++;
	}

	return decoded;
}

//http://121.37.7.68:8081/commander
void SearchEngineServer::loadDictionary() {
	_httpserver.GET("/commander", [this](const HttpReq *req, HttpResp *resp, SeriesWork *series) {
		std::string info = req->query("info");
		std::string decoded_info = urlDecode(info);
		std::string response = "Received key1: " + decoded_info;
		std::cout << response << std::endl;
		vector<string> lines = _jieba->cut(decoded_info);
		nlohmann::json resultJson;
		nlohmann::json recommendationsArray = nlohmann::json::array();
		for (const auto &word : lines) {
			if (!word.empty() && word != " " && _conf->getStopWordList().find(word) == _conf->getStopWordList().end()) {
				KeyRecommander keyRecommander(word);
				keyRecommander.execute();
				auto &maxHeap = keyRecommander.getMaxHeap();
				nlohmann::json wordRecommendations = nlohmann::json::array();

				while (!maxHeap.empty()) {
					const auto &candidate = maxHeap.top();
					wordRecommendations.push_back({{"word", candidate.word}, {"frequency", candidate.freq},
					                               {"edit_distance", candidate.dist}});
					maxHeap.pop();
				}

				recommendationsArray.push_back({{"query_word", word}, {"recommendations", wordRecommendations}});
			}
		}

		resultJson["status"] = "success";
		resultJson["recommendations"] = recommendationsArray;

		// 返回 JSON 响应给客户端
		std::string json_response = resultJson.dump();
		resp->append_output_body(json_response);
		resp->add_header_pair("Content-Type", "application/json");
	});
}

//http://121.37.7.68:8081/search
void SearchEngineServer::loadWeb() {
	_httpserver.GET("/search", [this](const HttpReq *req, HttpResp *resp, SeriesWork *series) {
		std::string info = req->query("info");
		std::string decoded_info = urlDecode(info);

		// 先查询缓存，缓存未命中则查询 Redis
		std::string cache_result = getFromCacheOrRedis(decoded_info, resp, series);
		if (!cache_result.empty()) {
			// 缓存命中，直接返回结果
			resp->append_output_body(cache_result);
			resp->add_header_pair("Content-Type", "application/json");
		}
	});
}

string SearchEngineServer::getFromCacheOrRedis(const string &key, HttpResp *resp, SeriesWork *series) {
	// 查询LRU缓存
	std::string cache_result = _cache.get(key);
	if (!cache_result.empty()) {
		return cache_result; // LRU缓存命中
	}

	// 查询Redis，如果命中返回值，否则执行查询
	queryRedis(key, resp, series);
	return "";
}

string SearchEngineServer::getFromCacheOrRedis(const string &key, HttpResp *resp, SeriesWork *series,
                                               SubLRUCache &subLRU) {
	string sub_lru_result = subLRU.get(key);
	if (!sub_lru_result.empty()) {
		return sub_lru_result;
	}
	string lru_result = _cache.get(key);
	if (!lru_result.empty()) {
		subLRU.put(key, lru_result);
		return lru_result;
	}
	queryRedis(key, resp, series);
	return "";
}

void SearchEngineServer::queryRedis(const std::string &key, HttpResp *resp, SeriesWork *series) {
	WFRedisTask *redis_task_get = WFTaskFactory::create_redis_task("redis://127.0.0.1", 1,
	[this, key, resp, series](WFRedisTask *redis_task) {
		protocol::RedisResponse *response = redis_task->get_resp();
		protocol::RedisValue redis_value;
		int state = redis_task->get_state();
		int error = redis_task->get_error();
		if (state == WFT_STATE_SUCCESS) {
			response->get_result(redis_value);
			handleRedisResponse(redis_value, key, resp, series);
		}else {
			fprintf(stderr,"Redis task failed. State: %d, Error: %d\n",state, error);
		}
	});

	redis_task_get->get_req()->set_request("HGET", {"info", key});
	series->push_back(redis_task_get); // 将查询任务添加到 Series
}

void SearchEngineServer::handleRedisResponse(const protocol::RedisValue &redis_value, const std::string &key,
                                             HttpResp *resp, SeriesWork *series) {
	if (redis_value.is_nil()) {
		// Redis未命中，执行查询
		handleCacheMiss(key, resp, series);
	}
	else if (redis_value.is_error()) {
		fprintf(stderr, "%*s\n", static_cast<int>(redis_value.string_view()->size()),
		        redis_value.string_view()->c_str());
	}
	else if (redis_value.is_string()) {
		// Redis命中，直接返回
		std::string redis_result(redis_value.string_view()->data(), redis_value.string_view()->size());
		_cache.put(key, redis_result); // 更新LRU缓存
		resp->append_output_body(redis_result);
		resp->add_header_pair("Content-Type", "application/json");
	}
}

void SearchEngineServer::handleCacheMiss(const std::string &key, HttpResp *resp, SeriesWork *series) {
	// 查询逻辑放入单独的函数
	std::string query_result = executeQuery(key);

	// 将查询结果写入Redis
	WFRedisTask *redis_task_set = WFTaskFactory::create_redis_task("redis://127.0.0.1", 1, nullptr);
	redis_task_set->get_req()->set_request("HSET", {"info", key, query_result});
	series->push_back(redis_task_set); // 将插入任务添加到 Series

	// 更新LRU缓存
	_cache.put(key, query_result);

	// 返回查询结果
	resp->append_output_body(query_result);
	resp->add_header_pair("Content-Type", "application/json");
}

std::string SearchEngineServer::executeQuery(const std::string &decoded_info) {
	vector<string> lines = _jieba->cut(decoded_info);
	WebPageQuery *web_page_query = WebPageQuery::getInstance();
	unordered_map<string, double> termFrequency;

	for (const auto &word : lines) {
		if (!word.empty() && word != " " && _conf->getStopWordList().find(word) == _conf->getStopWordList().end()) {
			termFrequency[word]++;
		}
	}

	web_page_query->getQueryWordsWeightVector(termFrequency);

	std::priority_queue<WebPageQuery::Result> results;
	web_page_query->executeQuery(termFrequency, &results);

	priority_queue<WebPageQuery::Result> cp = results;
	vector<string> elements;
	while (!cp.empty()) {
		elements.push_back(cp.top().docID);
		cp.pop();
	}

	// 创建 JSON 响应
	return web_page_query->createJson(elements, lines);
}

//http://121.37.7.68:8081/searching
void SearchEngineServer::loadSearchWeb() {
	_httpserver.GET("/searching", [this](const HttpReq *req, HttpResp *resp, SeriesWork *series) {
		std::string info = req->query("info");
		std::string decoded_info = urlDecode(info);

		// 先查询缓存，缓存未命中则查询 Redis
		std::string cache_result = getFromCacheOrRedis(decoded_info, resp, series);
		if (!cache_result.empty()) {
			// 缓存命中，直接返回结果
			resp->append_output_body(cache_result);
			resp->add_header_pair("Content-Type", "application/json");
		}
	});
}
