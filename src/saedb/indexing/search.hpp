#pragma once
#include <string>
#include <vector>
#include "indexing.hpp"
#include "query.hpp"

namespace indexing {

struct SearchResult : public std::vector<QueryItem> {
	int total_count = 0;
};

struct Searcher {
	Searcher(const Index& index) : index(index) {
	}
	SearchResult search(const std::string& query) {
		SearchResult result;
		auto q = buildQuery(query, index);
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