//
// Created by 李勃鋆 on 24-10-13.
//
#include "../include/WebPage.h"

WebPage::WebPage(const string &doc, Configuration &config, CppJieba &jieba):
	_doc(doc), _config(config), _jieba(jieba) {
}

int WebPage::getDocId() const {
	return _docId;
}

string WebPage::getDoc() const {
	return _doc;
}

void WebPage::processDoc(const std::string &filePath, const std::string &offsetFilePath, Configuration &config,
                         CppJieba &jieba) {
	ifstream file(filePath);
	ifstream offsetFile(offsetFilePath);
	ifstream behindFiles(filePath);
	ifstream behind_offsetFile(offsetFilePath);
	if (!file.is_open() || !offsetFile.is_open()) {
		std::cerr << "Failed to open file(s)." << std::endl;
		return;
	}

	long offset, length, behind_offset, behind_length;
	int num, behind_num;
	bool flag = true;
	string data1, data2;
	while (offsetFile >> num >> offset >> length) {
		file.seekg(offset);
		std::string fileData(length, '\0');
		file.read(&fileData[0], length);

		XMLDocument doc;
		XMLElement *docElem;
		if (doc.Parse(fileData.c_str()) == XML_SUCCESS) {
			docElem = doc.FirstChildElement("doc");
			if (docElem) {
				XMLElement *descElem = docElem->FirstChildElement("description");
				if (descElem && descElem->GetText()) {
					// 输出描述内容
					data1.clear();
					data1 = descElem->GetText();
					// std::cout << "Description: " << data1 << std::endl;
				}
			}
		}
		else {
			continue;
		}

		while (behind_offsetFile >> behind_num >> behind_offset >> behind_length) {
			if (behind_num > num) {
				behindFiles.seekg(behind_offset);
				std::string behindFileData(behind_length, '\0');
				behindFiles.read(&behindFileData[0], behind_length);
				XMLDocument behindDoc;
				if (behindDoc.Parse(behindFileData.c_str()) == XML_SUCCESS) {
					XMLElement *docElem_behind = behindDoc.FirstChildElement("doc");
					if (docElem_behind) {
						XMLElement *descElem = docElem_behind->FirstChildElement("description");
						if (descElem && descElem->GetText()) {
							data2.clear();
							data2 = descElem->GetText();
							// std::cout << "behind Description: " << data2 << std::endl;
							flag = jieba.complete(data1, data2);
							if (flag) {
								// std::cout << "Complete Success" << std::endl;
								break;
							}
						}
					}
				}
				else {
					continue;
				}
			}
			else {
				continue;
			}
		}
		if (flag == false) {
			RssItem rssItem;
			if (docElem == nullptr) {
				return;
			}
			XMLElement *title = docElem->FirstChildElement("title");
			XMLElement *link = docElem->FirstChildElement("link");
			XMLElement *pubDate = docElem->FirstChildElement("pubDate");
			XMLElement *description = docElem->FirstChildElement("description");

			rssItem.title = title && title->GetText() ? title->GetText() : "";
			rssItem.link = link && link->GetText() ? link->GetText() : "";
			rssItem.pubDate = pubDate && pubDate->GetText() ? pubDate->GetText() : "";
			rssItem.description = description && description->GetText() ? removeHtmlTags(description->GetText()) : "";
			_rss_new.push_back(rssItem);
		}
	}
	behindFiles.close();
	behind_offsetFile.close();
	offsetFile.close();
	file.close();
}

std::string WebPage::removeHtmlTags(const std::string &html) {
	// 定义一个正则表达式来匹配 HTML 标签
	const std::regex tagRegex("(<!--(.|\n)*?-->|<[^>]*>)");
	// 使用 regex_replace 将 HTML 标签替换为空字符串
	return std::regex_replace(html, tagRegex, "");
}

void WebPage::loadArticlesFromOffsets(const std::string &offsetFilePath) {
	std::ifstream offsetFile(offsetFilePath);
	if (!offsetFile.is_open()) {
		offsetFile.close();
		std::cerr << "Failed to open offset file" << std::endl;
		return;
	}

	long offset, length;
	int num;
	while (offsetFile >> num >> offset >> length) {
		parseRssByOffset(offset, length); // 调用函数解析特定偏移处的 XML 数据
	}

	offsetFile.close();
}

void WebPage::parseRssByOffset(long offset, long length) {
	std::ifstream file("/root/a/SearchEngine/data/ripePage.txt");
	if (!file.is_open()) {
		file.close();
		std::cerr << "Failed to open ripePage.txt" << std::endl;
		return;
	}
	// 定位到特定的偏移量位置并读取数据
	file.seekg(offset);
	std::string xmlData(length, '\0');
	file.read(&xmlData[0], length);
	if (file.gcount() != length) {
		std::cerr << "Warning: Could not read the expected length of data." << std::endl;
	}
	XMLDocument doc;
	if (doc.Parse(xmlData.c_str()) != XML_SUCCESS) {
		std::cerr << "Failed to parse XML data at offset: " << offset << ". Skipping this segment." << std::endl;
		return;
	}

	XMLElement *channel = doc.FirstChildElement("rss")->FirstChildElement("channel");
	if (!channel) {
		std::cerr << "No <channel> element found in XML segment at offset: " << offset << ". Skipping this segment." <<
			std::endl;
		return;
	}

	XMLElement *item = channel->FirstChildElement("item");
	while (item != nullptr) {
		RssItem rssItem;

		XMLElement *title = item->FirstChildElement("title");
		XMLElement *link = item->FirstChildElement("link");
		XMLElement *pubDate = item->FirstChildElement("pubDate");
		XMLElement *description = item->FirstChildElement("description");

		rssItem.title = title && title->GetText() ? title->GetText() : "";
		rssItem.link = link && link->GetText() ? link->GetText() : "";
		rssItem.pubDate = pubDate && pubDate->GetText() ? pubDate->GetText() : "";
		rssItem.description = description && description->GetText() ? removeHtmlTags(description->GetText()) : "";

		_rss.push_back(rssItem);
		item = item->NextSiblingElement("item");
	}

	file.close();
}

void WebPage::dump() {
	std::ofstream fout("/root/a/SearchEngine/data/ripePage_clean.txt");
	if (!fout.is_open()) {
		fout.close();
		std::cerr << "Failed to open file" << std::endl;
		return;
	}

	int count = 1;
	int index = 1;
	for (const auto &[title, link, pubDate, description] : _rss) {
		long startOffset = fout.tellp(); // 获取起始偏移量

		// 写入文章内容
		fout << "<doc>" << std::endl;
		fout << "\t<docid>" << count++ << "</docid>" << std::endl;
		fout << "\t<title>" << title << "</title>" << std::endl;
		fout << "\t<link>" << link << "</link>" << std::endl;
		fout << "\t<pubDate>" << pubDate << "</pubDate>" << std::endl;
		fout << "\t<description>" << description << "</description>" << std::endl;
		fout << "</doc>" << std::endl;

		long length = fout.tellp() - startOffset; // 计算长度
		_offsetLib[index++] = {startOffset, length}; // 存储偏移量和长度
	}
	fout.close(); // 关闭 `ripePage_clean.txt` 文件

	// 写入 `offSetLib_clean.txt` 文件
	std::ofstream writer("/root/a/SearchEngine/data/offSetLib_clean.txt");
	if (!writer.is_open()) {
		writer.close();
		std::cerr << "Failed to open offSetLib_clean.txt" << std::endl;
		return;
	}
	for (const auto &[idx, offsetLen] : _offsetLib) {
		writer << idx << " " << offsetLen.first << " " << offsetLen.second << std::endl;
	}
	writer.close(); // 关闭 `offSetLib_clean.txt` 文件
}

void WebPage::dump_new() {
	std::ofstream fout("/root/a/SearchEngine/data/ripePage_clean_new.txt");
	if (!fout.is_open()) {
		fout.close();
		std::cerr << "Failed to open file" << std::endl;
		return;
	}

	int count = 1;
	int index = 1;
	for (const auto &[title, link, pubDate, description] : _rss_new) {
		long startOffset = fout.tellp();
		fout << "<doc>" << std::endl;
		fout << "\t<docid>" << count++ << "</docid>" << std::endl;
		fout << "\t<title>" << title << "</title>" << std::endl;
		fout << "\t<link>" << link << "</link>" << std::endl;
		fout << "\t<pubDate>" << pubDate << "</pubDate>" << std::endl;
		fout << "\t<description>" << description << "</description>" << std::endl;
		fout << "</doc>" << std::endl;

		long length = fout.tellp() - startOffset; // 计算长度
		_offsetLib_new[index++] = {startOffset, length}; // 存储偏移量和长度
	}
	fout.close();

	std::ofstream writer("/root/a/SearchEngine/data/offSetLib_clean_new.txt");
	if (!writer.is_open()) {
		writer.close();
		std::cerr << "Failed to open offSetLib_clean.txt" << std::endl;
		return;
	}
	for (const auto &[idx, offsetLen] : _offsetLib_new) {
		writer << idx << " " << offsetLen.first << " " << offsetLen.second << std::endl;
	}
	writer.close();
}

bool operator==(const WebPage &lhs, const WebPage &rhs) {
	return lhs._jieba.complete(lhs._docContent, rhs._docContent);
}

bool operator<(const WebPage &lhs, const WebPage &rhs) {
	return lhs._docId < rhs._docId;
}
