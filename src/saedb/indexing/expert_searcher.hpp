#pragma once
#include "search.hpp"
#include "query.hpp"
#include "features.hpp"

#define AUTHOR_BASE 0
#define PUBLICATION_BASE 1 << 32
#define JCONF_BASE 1 << 33

/*
* We only provide document based expert search( search expert by search corresponding publications first)
* This class provide interface for feature plugin, thus one could add possible features and their weight to this class.
* each feature extractor could enumerate the candidate and return a score, a overall sum of feature score would be
* used for ranking the expert
*/

using namespace indexing;

class ExpertSearcher
{
public:
	ExpertSearcher(indexing::Index& index, int max_pub_count = 5000);
	~ExpertSearcher();
	SearchResult search(string query, const std::unique_ptr<MappedGraph>& g);
	void AddFeature(FeatureExtractor* feature, double weight);
private:
	vector<feature_with_weight> features;
	indexing::Index& index;
	int pub_count;
};
