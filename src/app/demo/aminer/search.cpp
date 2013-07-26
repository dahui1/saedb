#include "search.hpp"
#include "indexing/search.hpp"
#include "interface.pb.h"

using namespace std;
using namespace demoserver;
using namespace indexing;


namespace demoserver {
	bool AuthorSearch(const string& input, string& output) {
		EntitySearchRequest request;
		request.ParseFromString(input);
		string query = request.query();

		AMinerData& aminer = AMinerData::instance();
	    auto result = aminer.searchPublications("data mining");

	    EntitySearchResponse response;
	    response.set_total_count(result.total_count);
	    response.set_query(query);

	    for (auto& i : result) {
	    	DetailedEntity *de = response.add_entity();
	        auto p = aminer.get<Publication>(i.docId);
	    	de->set_title(p.title);
	    	de->set_id(i.docId);
	    }

		return response.SearializeToString(&output);
	}

}
