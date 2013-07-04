#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>



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
    int docId;
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
    int id(const std::string word) {
        auto wi = find(word);
        if (wi != end()) {
            return wi->second;
        } else {
            int i = (int) size();
            insert(make_pair(word, i));
            return i;
        }
    }
    
    int find_id(const std::string word) const{
        auto wi = find(word);
        if (wi != end()) {
            return wi->second;
        }
        else return -1;
    }
};

struct Index : public std::unordered_map<Term, PostingList> {
    WordMap word_map;
    
	// optimize the index
    void optimize();

    static Index build(DocumentCollection);
};


} // namespace indexing
