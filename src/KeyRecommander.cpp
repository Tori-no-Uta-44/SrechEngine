//
// Created by 李勃鋆 on 24-10-15.
//
#include "../include/KeyRecommander.h"

class CandidateWord;

KeyRecommander::KeyRecommander(const string &query):
	_queryWord(query) {
	_dictionary = Dictionary::getInstance();
}

void KeyRecommander::execute() {
	std::vector<std::pair<std::string, int>> candidateWords = _dictionary->getDictALL();
	std::unordered_set<std::string> uniqueWords;  // 用于存储候选词的并集
	std::vector<std::pair<std::string, int>> uniqueCandidates;

	// 去重并获取并集
	for (const auto &candidate : candidateWords) {
		if (uniqueWords.insert(candidate.first).second) {  // 如果插入成功，说明是新词
			uniqueCandidates.push_back(candidate);
		}
	}

	// 将并集放入 maxHeap
	for (const auto &candidate : uniqueCandidates) {
		int distance = editDistance(_queryWord, candidate.first);
		maxHeap.push(CandidateResult{candidate.first, candidate.second, distance});
	}
}

void KeyRecommander::queryIndexTable() {
	const auto &indexTable = _dictionary->getIndexTable();
	const auto &indexTableCh = _dictionary->getIndexTableCh();

	for (auto word : _queryWord) {
		string singleChar(1, word);

		// 优先查找中文索引表
		auto itCh = indexTableCh.find(singleChar);
		if (itCh != indexTableCh.end()) {
			// 将找到的中文索引插入到 unionPositions
			_unionPositions.insert(itCh->second.begin(), itCh->second.end());
			std::cout << "Found in indexTableCh: " << singleChar << std::endl;
		} else {
			auto it = indexTable.find(singleChar);
			if (it != indexTable.end()) {
				// 将找到的英文索引插入到 unionPositions
				_unionPositions.insert(it->second.begin(), it->second.end());
				std::cout << "Found in indexTable: " << singleChar << std::endl;
			} else {
				std::cout << "Not found in either indexTable or indexTableCh: " << singleChar << std::endl;
			}
		}
	}

	std::cout << "unionPositions size: " << _unionPositions.size() << std::endl;
}

size_t KeyRecommander::length(const std::string &str) {
	std::size_t ilen = 0;
	for (std::size_t idx = 0; idx != str.size(); ++idx) {
		size_t nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}

int KeyRecommander::editDistance(const std::string &lhs, const std::string &rhs) {
	//计算最小编辑距离-包括处理中英文
	size_t lhs_len = length(lhs);
	size_t rhs_len = length(rhs);
	int editDist[lhs_len + 1][rhs_len + 1];
	for (size_t idx = 0; idx <= lhs_len; ++idx) {
		editDist[idx][0] = idx;
	}
	for (size_t idx = 0; idx <= rhs_len; ++idx) {
		editDist[0][idx] = idx;
	}
	std::string sublhs, subrhs;
	for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i,
	     ++lhs_idx) {
		size_t nBytes = nBytesCode(lhs[lhs_idx]);
		sublhs = lhs.substr(lhs_idx, nBytes);
		lhs_idx += (nBytes - 1);
		for (std::size_t dist_j = 1, rhs_idx = 0;
		     dist_j <= rhs_len; ++dist_j, ++rhs_idx) {
			nBytes = nBytesCode(rhs[rhs_idx]);
			subrhs = rhs.substr(rhs_idx, nBytes);
			rhs_idx += (nBytes - 1);
			if (sublhs == subrhs) {
				editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j -
					1];
			}
			else {
				editDist[dist_i][dist_j] =
					triple_min(editDist[dist_i][dist_j - 1] + 1,
					           editDist[dist_i - 1][dist_j] + 1,
					           editDist[dist_i - 1][dist_j - 1] + 1);
			}
		}
	}
	return editDist[lhs_len][rhs_len];
}

int KeyRecommander::triple_min(const int &a, const int &b, const int &c) {
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

size_t KeyRecommander::nBytesCode(const char ch) {
	if (ch & (1 << 7)) {
		int nBytes = 1;
		for (int idx = 0; idx != 6; ++idx) {
			if (ch & (1 << (6 - idx))) {
				++nBytes;
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}

priority_queue<KeyRecommander::CandidateResult> &KeyRecommander::getMaxHeap() {
	return maxHeap;
}

const std::set<int>& KeyRecommander::getUnionPositions() const {
	return _unionPositions;
}