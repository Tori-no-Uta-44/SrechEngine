//
// Created by 李勃鋆 on 24-10-15.
//

#ifndef WEBPAGEQUERY_H
#define WEBPAGEQUERY_H
#include "CppJieba.h"
#include "WebPage.h"
#include "func.h"

class WebPageQuery {
public:
	struct Result {
		std::string docID; // 文档的ID
		double similarity; // 与查询的余弦相似度

		// 定义小于运算符，优先级队列会根据此运算符排序
		bool operator<(const Result &other) const {
			return similarity < other.similarity; // 使得相似度最高的在队列顶部
		}
	};

	static WebPageQuery *getInstance();

	unordered_map<string, vector<pair<string, double>>> computeIntersection(
		const unordered_map<string, double> &queryWords);

	void loadLibrary(); //加载库文件

	void getQueryWordsWeightVector(unordered_map<string, double> &queryWords); //计算查询词的权重值

	bool executeQuery(const unordered_map<string, double> &queryWords,
	                  std::priority_queue<Result> *resultQueue); //执行查询

	string createJson(vector<string> &docIdVec, const vector<string> &queryWords);

	double calculateNorm(const unordered_map<string, double> &vec);

	double calculateCosineSimilarity(const unordered_map<string, double> &queryWords,
	                                 const vector<pair<string, double>> &docWords,
	                                 double queryNorm);

	WebPageQuery(const WebPageQuery &other) = delete;

	WebPageQuery &operator=(const WebPageQuery &other) = delete;

	WebPageQuery(WebPageQuery &&other) = delete;

	WebPageQuery &operator=(WebPageQuery &&other) = delete;

private:
	WebPageQuery();

	~WebPageQuery();

	static void destroyInstance();
	typedef struct RssItem {
		string docid;
		string title;
		string link;
		string pubDate;
		string description;
	} RssItem;
	unordered_map<int, string> _pageLib; //网页库
	unordered_map<int, pair<long, long>> _offsetLib; //偏移库
	unordered_map<string, vector<pair<string, double>>> _invertIndexTable; //倒排索引表
	static WebPageQuery *_webPageQuery;
	static std::once_flag _once_flag;
};
#endif //WEBPAGEQUERY_H
