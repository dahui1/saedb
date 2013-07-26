#pragma once
#include <vector>
#include "query.hpp"
#include "io/mgraph.hpp"
#define AUTHOR_FEATURE_ALL_TOPIC -1

using std::vector;
using namespace sae::io;

namespace indexing {

struct ExtractFeatureParam
{
public:
	int doc_id;
};

struct SupportDocFeatureParam: public ExtractFeatureParam
{
public:
	vector<QueryItem> support_docs;
};

class FeatureExtractor
{
public:
	virtual ~FeatureExtractor() { };
	virtual void init(const vector<string>& query_words) {};
	virtual double get_score(ExtractFeatureParam* param_ptr, const std::unique_ptr<MappedGraph>& g) = 0;
};

typedef pair<FeatureExtractor*,double> feature_with_weight;

class BasicExpertFeature: public FeatureExtractor
{
public:
	virtual void init(const vector<string>& query_words);
	virtual double get_score(ExtractFeatureParam* param_ptr, const std::unique_ptr<MappedGraph>& g);
private:
	double conf_score(int conf_id, const std::unique_ptr<MappedGraph>& g);
	DocumentCollection authors, pubs;
};

}
