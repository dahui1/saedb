#include <string>
#include "analyzer.hpp"
#include "indexing.hpp"

#define BM25_K 2.0
#define BM25_B 0.75

using namespace std;
using namespace indexing;

int WordMap::id(const std::string word) {
    auto wi = find(word);
    if (wi != end()) {
        return wi->second;
    } else {
        int i = (int) size();
        insert(make_pair(word, i));
        return i;
    }
}

int WordMap::find_id(const std::string word) const {
    auto wi = find(word);
    if (wi != end()) {
        return wi->second;
    }
    else return -1;
}

Index Index::build(DocumentCollection docs) {
	Index index;
	int count = docs.size();
	double avgLen = 0;
	for (auto& doc : docs) {
		for (auto& field : doc.second) {
			string value = field.value;
			for (unsigned i = 0; i < value.length(); i++)
				if (value[i] == ' ' && i != value.length()-1)
					avgLen++;
			avgLen++;
		}
	}
	avgLen = avgLen / (double)count;

	for (auto& doc : docs) {
		cout << doc.second.id << endl;
		for (auto& field : doc.second) {
			string& value = field.value;
			unique_ptr<TokenStream> stream(ArnetAnalyzer::tokenStream(value));
			unordered_map<int, vector<short int> > word_position;
			int position = 0;

			Token token;
			while (stream->next(token)) {
				string term = token.getTermText();
				int term_id = index.word_map.id(term);
				cout << term << endl;
				word_position[term_id].push_back(position++);
			}

			int totalTokens = position;
			for (auto& wp : word_position) {
				int word_id = wp.first;
				int field_id = 0; // TODO
				auto& positions = wp.second;
				int freq = static_cast<int>(positions.size());
				double score = (freq * (BM25_K + 1)) / (freq + BM25_K * (1 - BM25_B + BM25_B * totalTokens / avgLen));
				// insert a new posting item
				index[word_id].insert(PostingItem{doc.second.id, positions, score});
			}
		}
	}
	index.optimize();
	return std::move(index);
}

void Index::optimize() {
	// current nothing to do.
}

