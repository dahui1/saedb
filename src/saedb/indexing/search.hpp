#pragma once
#include <string>
#include <vector>
#include "indexing.hpp"
#include "query.hpp"
#include "io/mgraph.hpp"

using namespace sae::io;

namespace indexing {

struct SearchResult : public std::vector<QueryItem> {

};

struct Searcher {
	Searcher(const Index& index) : index(index) {
	}
	SearchResult search(const std::string& query) {
		SearchResult result;
		auto q = buildQuery(query, index);
		QueryItem item;
		while (q->next(item)) {
			result.push_back(item);
		}
		return result;
	}

private:
	const Index& index;
};

}
