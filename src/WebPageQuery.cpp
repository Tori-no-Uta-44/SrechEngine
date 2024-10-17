//
// Created by 李勃鋆 on 24-10-15.
//
#include "../include/WebPageQuery.h"

WebPageQuery *WebPageQuery::_webPageQuery = nullptr;
std::once_flag WebPageQuery::_once_flag;

WebPageQuery::WebPageQuery() = default;

WebPageQuery *WebPageQuery::getInstance() {
	call_once(_once_flag, []() {
		_webPageQuery = new WebPageQuery();
		atexit(destroyInstance);
	});
	return _webPageQuery;
}

WebPageQuery::~WebPageQuery() = default;

void WebPageQuery::destroyInstance() {
	if (_webPageQuery != nullptr) {
		delete _webPageQuery;
		_webPageQuery = nullptr;
	}
}

unordered_map<string, vector<pair<string, double>>> WebPageQuery::computeIntersection(
	const unordered_map<string, double> &queryWords) {
	unordered_map<string, vector<pair<string, double>>> resultVec;
	bool isFirstQuery = true;

	for (const auto &queryWord : queryWords) {
		const string &word = queryWord.first;
		auto it = _invertIndexTable.find(word);
		if (it == _invertIndexTable.end()) {
			resultVec.clear(); // 没有找到任何一个查询词，交集为空
			break;
		}

		const vector<pair<string, double>> &currentDocs = it->second;

		if (isFirstQuery) {
			for (const auto &doc : currentDocs) {
				resultVec[doc.first].push_back(doc);
			}
			isFirstQuery = false;
		}
		else {
			unordered_map<string, vector<pair<string, double>>> tempResult;
			for (const auto &doc : currentDocs) {
				if (resultVec.find(doc.first) != resultVec.end()) {
					tempResult[doc.first].push_back(doc);
				}
			}
			resultVec.swap(tempResult);
		}
	}

	return resultVec;
}

void WebPageQuery::loadLibrary() {
	ifstream offset("/root/a/SearchEngine/data/offSetLib_clean_new.txt");
	if (!offset.is_open()) {
		offset.close();
		std::cerr << "Failed to open file" << std::endl;
		return;
	}
	string line;
	int index_new;
	long offset_new, length_new;
	while (offset >> index_new >> offset_new >> length_new) {
		_offsetLib.insert({index_new, {offset_new, length_new}});
	}
	offset.close();

	ifstream invert("/root/a/SearchEngine/data/invertIndex.txt");
	if (!invert.is_open()) {
		invert.close();
		std::cerr << "Failed to open file" << std::endl;
		return;
	}
	line.clear();
	regex wordPattern(R"((\S+?)-->)");
	regex entriesPattern(R"(\((\d+),([0-9.]+)\))");
	smatch wordMatch, entryMatch;
	while (getline(invert, line)) {
		// 获取词语
		if (regex_search(line, wordMatch, wordPattern)) {
			string word = wordMatch[1];

			// 查找所有的 (编号, 分值) 对
			auto entries_begin = sregex_iterator(line.begin(), line.end(), entriesPattern);
			auto entries_end = sregex_iterator();

			for (auto i = entries_begin; i != entries_end; ++i) {
				entryMatch = *i;
				string docID = entryMatch[1];
				double score = stod(entryMatch[2]);

				// 存储到unordered_map
				_invertIndexTable[word].emplace_back(docID, score);
			}
		}
	}
	invert.close();
}

void WebPageQuery::getQueryWordsWeightVector(unordered_map<string, double> &queryWords) {
	size_t sizes = _invertIndexTable.size();
	for (auto &[fst, snd] : queryWords) {
		double tmp = snd * log2(static_cast<double>(sizes) / (snd + 1.0) + 1.0);
		queryWords[fst] = tmp;
	}
}

bool WebPageQuery::executeQuery(const unordered_map<string, double> &queryWords,
                                std::priority_queue<Result> *resultQueue) {
	// 1. 计算查询向量的模长
	double queryNorm = calculateNorm(queryWords);

	// 2. 计算交集
	unordered_map<string, vector<pair<string, double>>> resultVec = computeIntersection(queryWords);
	if (resultVec.empty()) {
		return false; // 没有找到符合条件的文档
	}

	const int MAX_CAPACITY = 16;

	// 3. 计算每个文档的余弦相似度并存入优先级队列
	for (const auto &entry : resultVec) {
		const string &docID = entry.first;
		const vector<pair<string, double>> &docWords = entry.second;

		// 计算当前文档与查询的余弦相似度
		double cosineSimilarity = calculateCosineSimilarity(queryWords, docWords, queryNorm);

		// 如果队列未满，直接插入
		if (resultQueue->size() < MAX_CAPACITY) {
			resultQueue->push(Result{docID, cosineSimilarity});
		}
		else {
			// 队列已满，比较新元素与队列中最小相似度的元素
			if (cosineSimilarity > resultQueue->top().similarity) {
				resultQueue->pop(); // 移除当前最小相似度的元素
				resultQueue->push(Result{docID, cosineSimilarity});
			}
		}
	}

	return true;
}

string WebPageQuery::createJson(vector<string> &docIdVec, const vector<string> &queryWords) {
	ifstream file("/root/a/SearchEngine/data/ripePage_clean_new.txt");
	if (!file.is_open()) {
		file.close();
		std::cerr << "Failed to open" << std::endl;
	}
	_pageLib.clear();
	int i=1;
	for (auto &index : docIdVec) {
		if(auto search=_offsetLib.find(stoi(index));search != _offsetLib.end()) {
			long offset=search->second.first;
			long length=search->second.second;
			file.seekg(offset);
			string xmlData(length, '\0');
			file.read(&xmlData[0], length);
			XMLDocument doc;
			if (doc.Parse(xmlData.c_str()) == XML_SUCCESS) {
				XMLElement *docElem = doc.FirstChildElement("doc");
				if (docElem) {
					RssItem rssItem;
					XMLElement *docid= docElem->FirstChildElement("docid");
					XMLElement *title = docElem->FirstChildElement("title");
					XMLElement *link = docElem->FirstChildElement("link");
					XMLElement *pubDate = docElem->FirstChildElement("pubDate");
					XMLElement *description = docElem->FirstChildElement("description");

					rssItem.docid = docid && docid->GetText() ? docid->GetText() : "";
					rssItem.title = title && title->GetText() ? title->GetText() : "";
					rssItem.link = link && link->GetText() ? link->GetText() : "";
					rssItem.pubDate = pubDate && pubDate->GetText() ? pubDate->GetText() : "";
					rssItem.description = description && description->GetText() ? description->GetText() : "";
					_pageLib.insert({i,rssItem.description});
					++i;
				}
			}
		}
	}


	nlohmann::json resultJson;
	nlohmann::json recommendationsArray = nlohmann::json::array();
	for (auto &[index,description] : _pageLib) {
		nlohmann::json recommendation;
		recommendation["index"] = index;
		recommendation["description"] = description;
		recommendationsArray.push_back(recommendation);
	}
	resultJson["status"] = "success";
	resultJson["recommendations"] = recommendationsArray;
	return resultJson.dump();
}

double WebPageQuery::calculateNorm(const unordered_map<string, double> &vec) {
	double norm = 0.0;
	for (const auto &entry : vec) {
		norm += entry.second * entry.second;
	}
	return std::sqrt(norm);
}

double WebPageQuery::calculateCosineSimilarity(const unordered_map<string, double> &queryWords,
                                               const vector<pair<string, double>> &docWords,
                                               double queryNorm) {
	double dotProduct = 0.0;
	double docNorm = 0.0;

	for (const auto &docWord : docWords) {
		const string &word = docWord.first;
		double docTFIDF = docWord.second;

		auto queryIt = queryWords.find(word);
		if (queryIt != queryWords.end()) {
			dotProduct += queryIt->second * docTFIDF;
		}
		docNorm += docTFIDF * docTFIDF;
	}

	docNorm = std::sqrt(docNorm);
	return (docNorm > 0 && queryNorm > 0) ? (dotProduct / (queryNorm * docNorm)) : 0.0;
}
