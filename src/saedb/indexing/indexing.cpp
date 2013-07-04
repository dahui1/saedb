#include <string>
#include "analyzer.hpp"
#include "indexing.hpp"

#define BM25_K 2.0
#define BM25_B 0.75

#define avgLen 30

using namespace std;
using namespace indexing;

Index Index::build(DocumentCollection docs) {
	Index index;
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
				Term term{word_id, field_id};
				// insert a new posting item
				index[term].insert(PostingItem{doc.second.id, positions, score});
			}
		}
	}
	index.optimize();
	return std::move(index);
}

void Index::optimize() {
	// current nothing to do.
}
