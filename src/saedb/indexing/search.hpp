#pragma once
#include <string>
#include <vector>
#include "indexing.hpp"
#include "query.hpp"

namespace indexing {

struct SearchResult : public std::vector<QueryItem> {
};

struct Searcher {
    Searcher(const Index& index) : index(index) {
    }
	
	//type = 0: English; type = 1: Chinese
    SearchResult search(const std::string& query, const int type) {
        SearchResult result;
        auto q = buildQuery(query, index, type);
        if (q) {
            QueryItem item;
            while (q->next(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

private:
    const Index& index;
};

}
