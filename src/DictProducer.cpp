//
// Created by 李勃鋆 on 24-10-11.
//

#include "../include/DictProducer.h"

DictProducer::DictProducer(const string &dir) {
	_stopWordList = Configuration::getInstance()->getStopWordList();
	DirScanner dir_scanner;
	dir_scanner.traverse(dir);
	_files = dir_scanner.files();
}

DictProducer::DictProducer(const string &dir, SplitTool *splitTool):
	_splitTool(splitTool) {
	_stopWordList = Configuration::getInstance()->getStopWordList();
	DirScanner dir_scanner;
	dir_scanner.traverse(dir);
	_files = dir_scanner.files();
}

void DictProducer::showFiles() const {
	for (auto &file : _files) {
		cout << file << endl;
	}
}

void DictProducer::getFiles() {
	for (auto &file : _files) {
		if (file == "/root/a/infomation/txt/china_daily.txt" || file == "/root/a/infomation/txt/english.txt" || file ==
			"/root/a/infomation/txt/The_Holy_Bible.txt") {
			buildEnDict(file);
		}
		else if (file == "/root/a/infomation/txt/stop/stop_words_eng.txt" || file ==
			"/root/a/infomation/txt/stop/stop_words_zh.txt") {
			continue;
		}
		else {
			buildCnDict(file);
		}
	}
}

string DictProducer::dealLine(const string &line) {
	string result;
	result.reserve(line.size());
	bool lastWasAlpha = false;
	for (const char &ch : line) {
		if (isalpha(ch)) {
			result += static_cast<char>(tolower(ch));
			lastWasAlpha = true;
		}
		else {
			if (lastWasAlpha) {
				result += ' ';
				lastWasAlpha = false;
			}
		}
	}
	if (!result.empty() && result.back() == ' ') {
		result.pop_back();
	}
	return result;
}

void DictProducer::buildEnDict(const string &path) {
	_lines.clear();
	ifstream stream(path);
	if (!stream.is_open()) {
		cerr << "Failed to open the file." << endl;
		stream.close();
		return;
	}
	string line;
	while (getline(stream, line)) {
		_lines.push_back(line);
		istringstream buffer(dealLine(line));
		string word;
		while (buffer >> word) {
			// 过滤单个字母的非停用词
			if (word.size() > 1 || _stopWordList.find(word) != _stopWordList.end()) {
				// 确保非停用词才添加到词典
				if (_stopWordList.find(word) == _stopWordList.end()) {
					_dict[word]++;
				}
			}
		}
	}
	stream.close();
}

string DictProducer::cleanText(const string &filePath) {
	ifstream inFile(filePath);
	stringstream cleanedContent;
	string line;

	if (!inFile.is_open()) {
		cerr << "无法打开文件: " << filePath << endl;
		inFile.close();
		return "";
	}

	bool newParagraph = false;

	while (getline(inFile, line)) {
		if (line.empty()) {
			// 空行代表段落结束，加一个空格分隔段落
			newParagraph = true;
		}
		else {
			if (newParagraph) {
				newParagraph = false;
			}
			cleanedContent << line;
		}
	}

	inFile.close();
	return cleanedContent.str();
}

void DictProducer::buildCnDict(const string &path) {
	_lines.clear();
	ifstream stream(path);
	if (!stream.is_open()) {
		stream.close();
		cerr << "Failed to open the file." << endl;
		return;
	}
	string cleanedText = cleanText(path);
	_lines = _splitTool->cut(cleanedText);
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	for (auto &word : _lines) {
		// 将 UTF-8 字符串转换为宽字符字符串
		wstring wword = converter.from_bytes(word);

		// 使用 lambda 表达式过滤特殊字符
		wword.erase(remove_if(wword.begin(), wword.end(), [](wchar_t ch) {
			// 半角数字、全角数字
			if ((ch >= 0x30 && ch <= 0x39) || (ch >= 0xFF10 && ch <= 0xFF19))
				return true;
			// 半角英文字母、全角英文字母
			if ((ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A) ||
				(ch >= 0xFF21 && ch <= 0xFF3A) || (ch >= 0xFF41 && ch <= 0xFF5A))
				return true;
			// 拉丁字母变体（带重音符号的字符）
			if ((ch >= 0x00C0 && ch <= 0x00FF) || (ch >= 0x0100 && ch <= 0x017F) ||
				(ch >= 0x0180 && ch <= 0x024F))
				return true;
			// 半角标点符号
			if ((ch >= 0x21 && ch <= 0x2F) || (ch >= 0x3A && ch <= 0x40) ||
				(ch >= 0x5B && ch <= 0x60) || (ch >= 0x7B && ch <= 0x7E))
				return true;
			// 全角标点符号
			if ((ch >= 0xFF01 && ch <= 0xFF0F) || (ch >= 0xFF1A && ch <= 0xFF20) ||
				(ch >= 0xFF3B && ch <= 0xFF40) || (ch >= 0xFF5B && ch <= 0xFF5E))
				return true;
			// 特殊符号：中点、长破折号、双引号变体、空心圆等
			if (ch == 0x00B7 || ch == 0x2014 || ch == 0x2033 || ch == 0x25CB)
				return true;
			// 引号变体
			if (ch >= 0x2018 && ch <= 0x201D)
				return true;
			// 罗马数字
			if (ch >= 0x2160 && ch <= 0x217F)
				return true;
			// 圆圈数字
			if (ch >= 0x2460 && ch <= 0x24FF)
				return true;
			// 特殊符号（省略号、其他括号等）
			if ((ch >= 0x3001 && ch <= 0x300F) || (ch >= 0x3010 && ch <= 0x3011) ||
				(ch >= 0xFF08 && ch <= 0xFF09) || (ch >= 0xFF5B && ch <= 0xFF5D) ||
				(ch >= 0x3014 && ch <= 0x3015) || ch == 0x2026)
				return true;

			return false;
		}), wword.end());

		// 将处理后的宽字符字符串转换回 UTF-8
		word = converter.to_bytes(wword);

		// 检查停用词并统计词频
		if (!word.empty() && word != " " && _stopWordList.find(word) == _stopWordList.end()) {
			_dict_Ch[word]++;
		}
	}
	stream.close();
}

void DictProducer::showDict() const {
	for (auto &[fst,snd] : _dict) {
		cout << fst << "->" << snd << endl;
	}
	std::cout << "---" << endl;
	for (const auto &_line : _lines) {
		std::cout << _line << endl;
	}
	std::cout << "---" << endl;
	for (auto &[fst,snd] : _index) {
		std::cout << fst << ":";
		for (auto &i : snd) {
			std::cout << i << ",";
		}
		std::cout << std::endl;
	}
}

void DictProducer::pushDict() {
	ofstream writer("/root/a/SearchEngine/data/dictionary.txt", ios::app);
	if (!writer.is_open()) {
		writer.close();
		cerr << "Failed to open the file." << endl;
	}
	for (auto &[fst,snd] : _dict) {
		writer << fst << "|" << snd << endl;
	}
	writer.close();

	ofstream writerCh("/root/a/SearchEngine/data/dictionaryCh.txt", ios::app);
	if (!writerCh.is_open()) {
		writerCh.close();
		cerr << "Failed to open the file." << endl;
	}
	for (auto &[fst,snd] : _dict_Ch) {
		writerCh << fst << "|" << snd << endl;
	}
	writerCh.close();
}

void DictProducer::storeDict(const char *filepath) {
	buildIndex(_dict, _index);
	ofstream writer(filepath, ios::app);
	if (!writer.is_open()) {
		cerr << "Failed to open the file." << endl;
		writer.close();
	}
	for (auto &[fst,snd] : _index) {
		writer << fst << ":";
		for (auto &i : snd) {
			writer << i << ",";
		}
		writer << std::endl;
	}
	writer.close();
}

void DictProducer::storeDictCh(const char *filepath) {
	buildIndex(_dict_Ch, _index_Ch);
	ofstream writer(filepath, ios::app);
	if (!writer.is_open()) {
		writer.close();
		cerr << "Failed to open the file." << endl;
	}
	for (auto &[fst,snd] : _index_Ch) {
		writer << fst << ":";
		for (auto &i : snd) {
			writer << i << ",";
		}
		writer << std::endl;
	}
	writer.close();
}

void DictProducer::buildIndex(const map<string, int> &dict, map<string, set<int>> &ind) {
	int i = 0; //记录下标
	for (auto &elem : dict) {
		string word = elem.first;
		size_t charNums = word.size() / getByteNum_UTF8(word[0]);
		for (size_t idx = 0, n = 0; n != charNums; ++idx, ++n) //得到字符数
		{
			//按照字符个数切割
			size_t charLen = getByteNum_UTF8(word[idx]);
			string subWord = word.substr(idx, charLen); //按照编码格式，进行拆分
			ind[subWord].insert(i);
			idx += (charLen - 1);
		}
		++i;
	}
}

size_t DictProducer::getByteNum_UTF8(const char byte) {
	int byteNum = 0;
	for (size_t i = 0; i < 6; ++i) {
		if (byte & (1 << (7 - i)))
			++byteNum;
		else
			break;
	}
	return byteNum == 0 ? 1 : byteNum;
}
