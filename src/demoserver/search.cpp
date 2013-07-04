#include "search.hpp"
#include "indexing/search.hpp"
#include "interface.pb.h"

using namespace std;
using namespace demoserver;
using namespace indexing;

namespace {
    DocumentCollection testData() {
        Document doc1, doc2, doc3;
        doc1.id = 1;
        doc2.id = 2;
        doc3.id = 3;
        doc1.push_back({"title", "initial document"});
        doc1.push_back({"content", "To maximize the scale of the community around a project, by reducing the friction for new Contributors and creating a scaled participation model with strong positive feedbacks;"});
        doc2.push_back({"title", "second document"});
        doc2.push_back({"content", "To relieve dependencies on key individuals by separating different skill sets so that there is a larger pool of competence in any required domain;"});
        doc3.push_back({"title", "third document"});
        doc3.push_back({"content", "To allow the project to develop faster and more accurately, by increasing the diversity of the decision making process;"});
        DocumentCollection dc;
        dc[1] = doc1;
        dc[2] = doc2;
        dc[3] = doc3;
        return dc;
    }

    Index testDataIndex(DocumentCollection dc) {
        Index index = Index::build(dc);
        return index;
    }
}

namespace demoserver {

bool EntitySearch(const string& input, string& output) {
    EntitySearchRequest request;
    request.ParseFromString(input);

    string query = request.query();
    EntitySearchResponse response;
    response.set_total_count(10);
    response.set_query(query);

    DocumentCollection dc = testData();
    Searcher searcher(testDataIndex(dc));
    SearchResult result = searcher.search(query);
    for (int i=0; i<result.size(); i++)
    {
        DetailedEntity *de = response.add_entities();
        int id = result[i].docId;
        de->set_title(dc[i][0].value);
        de->set_id(id);
    }

    return response.SerializeToString(&output);
}

}
