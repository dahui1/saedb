#include <unordered_map>
#include <cstring>
#include <algorithm>
#include "expert_searcher.hpp"
#include "indexing.hpp"
#include "features.hpp"
#include "io/mgraph.hpp"

using namespace std;
using namespace indexing;
using namespace sae::io;

bool sortByScore(const QueryItem& A, const QueryItem& B) {
	return A.score > B.score;
}

ExpertSearcher::ExpertSearcher(Index& _index, int max_pub_count)
	:index(_index),
	pub_count(max_pub_count)
{
	this->AddFeature(new BasicExpertFeature(), 1.0);
}

ExpertSearcher::~ExpertSearcher()
{
	for (feature_with_weight& feature : features)
	{
		delete feature.first;
	}
}

void ExpertSearcher::AddFeature(FeatureExtractor* feature, double weight)
{
	if(feature)
	{
		this->features.push_back(make_pair(feature, weight));
	}
}

SearchResult ExpertSearcher::search(string query, const std::unique_ptr<MappedGraph>& g)
{
	SearchResult result;
	SearchResult enumer;
	Document pub;
	unordered_map<int, vector<QueryItem>> author_pubs;
	unordered_map<int, vector<QueryItem>>::iterator author_pubs_iter;

	//Init each featrues for ranking
	vector<string> query_words;
	string::size_type start = 0, index;
	string substring;
	do {
		index = query.find_first_of(" ", start);
		if (index != string::npos) {
			substring = query.substr(start, index - start);
			query_words.push_back(substring);
			start = query.find_first_not_of(" ", index);
			if (start == string::npos)
				break;
		}
	} while (index != string::npos);
	substring = query.substr(start);
	query_words.push_back(substring);

	for (feature_with_weight& feature : features)
	{
		feature.first->init(query_words);
	}
	//searching section
	Searcher basic_searcher(this->index);
	enumer = basic_searcher.search(query, g);

	QueryItem item;
	vector<QueryItem>::iterator it = enumer.begin();
	for(int i=0;i<this->pub_count && it != enumer.end(); ++i, ++it)
	{
		vector<vid_t> authorIDs;
		auto vi = g->Vertices();
		vi->MoveTo(it->docId);
		pub = sae::serialization::convert_from_string<Publication>(vi->Data());
		if(pub)
		{
			auto edgeIt = vi->InEdges();
			while (edgeIt->Alive()) {
				if (edgeIt->Typename() == "Author") {
					authorIDs.push_back(edgeIt->TargetId());
				}
				edgeIt->Next();
			}
			for (vid_t naid : authorIDs )
			{
				if(naid!=-1)
					author_pubs[naid].push_back(*it);
			}
		}
	}

	//start caculation score using each feature
	for(author_pubs_iter = author_pubs.begin(); author_pubs_iter!= author_pubs.end(); ++author_pubs_iter)
	{
		/*if(special_ids.find(author_pubs_iter->first)!=special_ids.end())
			item.score = 50000;
		else
			item.score = 0.0;*/
		item.docId = author_pubs_iter->first;

		SupportDocFeatureParam param;
		param.doc_id = item.docId;
		param.support_docs = author_pubs_iter->second;
		for (feature_with_weight& feature : features)
		{
			item.score += feature.second * feature.first->get_score(&param, g);
		}
		result.push_back(item);
	}
	std::sort(result.begin(), result.end(), sortByScore);

	return result;
}

