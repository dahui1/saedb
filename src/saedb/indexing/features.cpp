#include "features.hpp"
#include "indexing.hpp"
#include "query.hpp"
#include "aminer.hpp"
#include <math.h>

#define SEARCH_STEP 100

namespace indexing {

void BasicExpertFeature::init(const vector<string>& query_words)
{

}

double BasicExpertFeature::get_score(ExtractFeatureParam* param_ptr, const std::unique_ptr<MappedGraph>& g)
{
	auto vi = g->Vertices();
	vi->MoveTo(param_ptr->doc_id);
	auto author = sae::serialization::convert_from_string<Author>(vi->Data());
	Publication pub;
	double score = 0.0;
	if(author)
	{
		for (QueryItem& item : ((SupportDocFeatureParam*)param_ptr)->support_docs)
		{
			auto vp = g->Vertices();
			vp->MoveTo(item.docId);
			pub = sae::serialization::convert_from_string<Publication>(vi->Data());
			if(pub)
			{
				int confID = pub.jconf;
				int nCitations = pub.citation_number;
				if(confID==-1 && nCitations>0)
					score += item.score * sqrt((double)nCitations)/2.0;
				else if(confID!=-1)
					score += item.score*conf_score(confID, g);
			}
		}
		double h_index = author.h_index;
		score+=h_index;
	}
	return score;
}

double BasicExpertFeature::conf_score(int conf_id, const std::unique_ptr<MappedGraph>& g)
{//if conf_id == -1 return sqrt(pub->nCitations);
	auto vc = g->Vertices();
	vc->MoveTo(conf_id);
	JConf jc = sae::serialization::convert_from_string<JConf>(vc->Data());
	return 1;//jc.id;
}

}
