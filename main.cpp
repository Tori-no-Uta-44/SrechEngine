#include "func.h"
#include "include/Configuration.h"
#include "include/DictProducer.h"
#include "include/CppJieba.h"
#include "include/Dictionary.h"
#include "include/DirScanner.h"
#include "include/PageLib.h"
#include "include/PageLibPreprocessor.h"
#include "include/SearchEngineServer.h"
#include "include/WebPage.h"
#include "include/WebPageQuery.h"

int main(int argc, char *argv[]) {
	auto conf = Configuration::getInstance();
	auto *cpp_jieba = new CppJieba();
	auto *dir = new DirScanner();
	WebPageQuery *web_page_query = WebPageQuery::getInstance();
	web_page_query->loadLibrary();
	// conf->getStopWordList();
	// DictProducer dict_producer("/root/a/infomation/txt", cpp_jieba);
	// dict_producer.getFiles();
	// dict_producer.pushDict();
	// dict_producer.storeDict("/root/a/SearchEngine/data/dictionaryIndex.txt");
	// dict_producer.storeDictCh("/root/a/SearchEngine/data/dictionaryIndexCh.txt");

	// PageLib page_lib(conf, dir);
	// page_lib.create();
	// page_lib.store();
	//
	// WebPage web_page("/root/a/SearchEngine/data/ripePage.txt",*conf,*cpp_jieba);
	// web_page.loadArticlesFromOffsets("/root/a/SearchEngine/data/offSetLib.txt");
	// web_page.dump();
	//
	// web_page.processDoc("/root/a/SearchEngine/data/ripePage_clean.txt", "/root/a/SearchEngine/data/offSetLib_clean.txt", *conf, *cpp_jieba);
	// web_page.dump_new();

	// PageLibPreprocessor page_lib_preprocessor(conf,cpp_jieba);
	// page_lib_preprocessor.readInfoFromFile();
	// page_lib_preprocessor.buildInvertIndexTable();
	// page_lib_preprocessor.storeOnDisk();

	SearchEngineServer search_engine_server(1, cpp_jieba);
	search_engine_server.loadModules();
	search_engine_server.start(8081);
	delete dir;
	delete cpp_jieba;
	return 0;
}
