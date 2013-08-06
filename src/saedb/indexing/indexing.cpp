#include <string>
#include "analyzer.hpp"
#include "indexing.hpp"

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

int WordMap::findId(const std::string word) const {
    auto wi = find(word);
    if (wi != end()) {
        return wi->second;
    }
    else return -1;
}

void Index::addSingle(int doc, int field, const string& value, double avg_len) {
    unique_ptr<TokenStream> stream(ArnetAnalyzer::tokenStream(value));
    unordered_map<int, vector<short>> word_position;
    int position = 0;

    Token token;
    while (stream->next(token)) {
        string term = token.getTermText();
        int term_id = word_map.id(term);
        word_position[term_id].push_back(position++);
    }

    int totalTokens = position;
    for (auto& wp : word_position) {
        int word = wp.first;
        auto& positions = wp.second;
        // insert a new posting item
        Term term{word, field};
        double score = bm25(positions.size(), totalTokens, avg_len);
        (*this)[term].insert(PostingItem{doc, positions, score});
    }
}

vector<string> split(string s, char c) {
    int last = 0;
    vector<string> v;
    for (int i=0; i<s.size(); i++) {
        if (s[i] == c) {
            v.push_back(s.substr(last, i - last));
            last = i + 1;
        }
    }
    v.push_back(s.substr(last, s.size() - last));
    return v;
}

void Index::addSingleCN(int doc, int field, const string& value, double avg_len, const std::set<string>& stopwords) {
    unordered_map<int, vector<short>> word_position;
    int position = 0;
    auto words = split(value, ' ');
    for (int i = 0; i < words.size(); i++ ) {
        string term = words[i];
        auto stop = stopwords.find(term);
        if (stop == stopwords.end()) {
            int term_id = word_map.id(term);
            word_position[term_id].push_back(position++);
        }
    }

    int totalTokens = position;
    for (auto& wp : word_position) {
        int word = wp.first;
        auto& positions = wp.second;
        // insert a new posting item
        Term term{word, field};
        double score = bm25(positions.size(), totalTokens, avg_len);
        (*this)[term].insert(PostingItem{doc, positions, score});
    }
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
        for (auto& field : doc.second) {
            index.addSingle(doc.second.id, 0, field.value, avgLen);
        }
    }

    index.optimize();
    return std::move(index);
}

void Index::optimize() {
    // currently nothing to do.
}

