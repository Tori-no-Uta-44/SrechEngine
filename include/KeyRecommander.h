//
// Created by 李勃鋆 on 24-10-15.
//

#ifndef KEYRECOMMANDER_H
#define KEYRECOMMANDER_H
#include "Dictionary.h"
#include "func.h"

class KeyRecommander {
public:
	struct CandidateResult {
		std::string word;
		int freq;
		int dist;

		// 优先按 dist 升序，dist 相同时按 freq 降序
		bool operator<(const CandidateResult &other) const {
			if (dist != other.dist) {
				return dist > other.dist; // 距离小的优先级高
			}
			return freq < other.freq; // 距离相同，频率高的优先级高
		}
	};

	explicit KeyRecommander(const string &query);

	void execute(); //执行查询
	[[nodiscard]] const std::set<int> &getUnionPositions() const;

	void queryIndexTable(); //查询索引


	size_t length(const std::string &str);

	int editDistance(const std::string &lhs, const std::string &rhs); //计算最小编辑距离


	int triple_min(const int &a, const int &b, const int &c);

	size_t nBytesCode(const char ch);

	priority_queue<CandidateResult> &getMaxHeap();

private:
	std::set<int> _positions;
	std::set<int> _unionPositions;
	Dictionary *_dictionary;
	string _queryWord;
	string _sought;
	priority_queue<CandidateResult> maxHeap;
};
#endif //KEYRECOMMANDER_H
