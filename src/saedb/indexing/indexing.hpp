#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>

#define PUB_TOPIC_UNINIT -2

using namespace std;

namespace indexing {

	struct Term {
		int word;
		int field;
		bool operator==(const indexing::Term x) const
		{
			return (word == x.word) && (field == x.field);
		}
	};
}

namespace std {
    template <>
	struct hash<indexing::Term> {
		size_t operator()(const indexing::Term x) const {
            return std::hash<int>()(x.word) ^ std::hash<int>()(x.field);
        }
    };
}

namespace indexing {

struct PostingItem {
    long unsigned int docId;
    std::vector<short> positions;
    double score;

	bool operator<(const indexing::PostingItem x) const
	{
		return docId < x.docId;
	}

};

struct PostingList : public std::set<PostingItem> {
};

struct Field {
    std::string name;
    std::string value;
};

struct Document : public std::vector<Field> {
    int id;
};

struct DocumentCollection : public std::map<int, Document> {
};

struct WordMap : public std::unordered_map<std::string, int> {
    int id(const std::string word);
    int find_id(const std::string word) const;
};

struct Index : public std::unordered_map<int, PostingList> {
    WordMap word_map;
    WordMap map;
    
	// optimize the index
    void optimize();

    static Index build(DocumentCollection);
};


} // namespace indexing
