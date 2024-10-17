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
#include <workflow/RedisMessage.h>
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

	WFFacilities::WaitGroup _waitGroup;
	HttpServer _httpserver;
	Configuration *_conf;
	Dictionary *_dictionary;
	CppJieba *_jieba;
};
#endif //SEARCHENGINESERVER_H
