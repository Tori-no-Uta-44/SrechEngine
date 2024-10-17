//
// Created by 李勃鋆 on 24-10-14.
//
#include "../include/PageLibPreprocessor.h"

PageLibPreprocessor::PageLibPreprocessor(Configuration *conf, CppJieba *jieba):
	_conf(conf), _jieba(jieba) {
	_stopWordList = _conf->getStopWordList();
}

void PageLibPreprocessor::doProcess(long offset, long length) {
	ifstream file("/root/a/SearchEngine/data/ripePage_clean.txt");
	if (!file.is_open()) {
		file.close();
		std::cerr << "Failed to open" << std::endl;
		return;
	}
	file.seekg(offset);
	std::string xmlData(length, '\0');
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
			_pageLib.push_back(rssItem);
			cutRedundantPages();
		}
	}

	file.close();
}

void PageLibPreprocessor::readInfoFromFile() {
	ifstream offsetFile("/root/a/SearchEngine/data/offSetLib_clean.txt");
	if (!offsetFile.is_open()) {
		offsetFile.close();
		std::cerr << "Failed to open file." << std::endl;
		return;
	}
	long offset, length;
	int num;
	while (offsetFile >> num >> offset >> length) {
		doProcess(offset, length);
	}
	offsetFile.close();
}

void PageLibPreprocessor::cutRedundantPages() {
	_lines = _jieba->cut(_pageLib.back().description);
	// wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	for (auto &word : _lines) {

		// 检查停用词并统计词频
		if (!word.empty() && word != " " && _stopWordList.find(word) == _stopWordList.end()) {
			// _dict_Ch[word]++;
			addWord(word,_pageLib.back().docid,1,_wordMap);
		}
	}
}

void PageLibPreprocessor::addWord(const std::string& word, const std::string& articleId, long frequency, std::unordered_map<std::string, std::vector<WordInfo>>& wordMap) {
	auto& infos = wordMap[word];
	bool found = false;

	// 检查是否已有该文章的记录
	for (auto& info : infos) {
		if (info.articleId == articleId) {
			info.frequency += frequency;  // 累加频率
			found = true;
			break;
		}
	}

	// 如果没有找到该文章的记录，则新增一个WordInfo并增加 articleCount
	if (!found) {
		infos.push_back({word, articleId, frequency, static_cast<long>(infos.size() + 1)});
	}
}

void PageLibPreprocessor::buildInvertIndexTable() {
	long totalDocs = static_cast<long>(_pageLib.size());

	// 记录每个单词的权重以及各篇文章的权重和
	std::unordered_map<std::string, std::unordered_map<std::string, double>> weightPerDoc; // 每篇文章的各单词权重
	std::unordered_map<std::string, double> normFactors; // 每篇文章的归一化因子

	for (auto &[word_out, infos] : _wordMap) {
		long df = infos.empty() ? 0 : infos.front().articleCount;
		double idf = std::log2(static_cast<double>(totalDocs) / (static_cast<double>(df) + 1.0) + 1.0);

		for (auto &[word, articleId, frequency, articleCount] : infos) {
			auto tf = static_cast<double>(frequency);
			double weight = tf * idf;
			weightPerDoc[articleId][word_out] = weight;
			normFactors[articleId] += weight * weight; // 累加每篇文章的权重平方和
		}
	}

	// 计算归一化因子并填充倒排索引表
	for (auto &[articleId, sumOfSquares] : normFactors) {
		double normFactor = std::sqrt(sumOfSquares); // 计算归一化因子
		for (auto &[word_out, weight] : weightPerDoc[articleId]) {
			_invertIndexTable[word_out].emplace_back(articleId, weight / normFactor); // 插入归一化后的权重
		}
	}
}

void PageLibPreprocessor::storeOnDisk() {
	ofstream writer("/root/a/SearchEngine/data/invertIndex.txt", ios::app);
	if (!writer.is_open()) {
		writer.close();
		cerr << "Failed to open the file." << endl;
	}
	// 打印最终的倒排索引表
	for (const auto &[word, entries] : _invertIndexTable) {
		writer << word << "-->";
		for (const auto &[articleId, weight] : entries) {
			writer << "(" << articleId << "," << weight << ")";
		}
		writer << std::endl;
	}
	writer.close();
}
