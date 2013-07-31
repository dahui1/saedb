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

    inline bool operator==(Term const &another) const
    {
        return (word == another.word) && (field == another.field);
    }
};

}

namespace std {

template <>
struct hash<indexing::Term> {
    size_t operator()(const indexing::Term term) const {
        return std::hash<int>()(term.word) ^ std::hash<int>()(term.field);
    }
};

}

namespace indexing {

const float BM25_K = 2.0;
const float BM25_B = 0.75;

struct PostingItem {
    int docId;
    std::vector<short> positions;
    double score;

    bool operator<(const indexing::PostingItem& x) const
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

struct Index : public std::unordered_map<Term, PostingList> {
    WordMap word_map;

    // optimize the index
    void optimize();

    static Index build(DocumentCollection);
};

} // namespace indexing
