#include "search.hpp"
#include "expert_searcher.hpp"

using namespace indexing;
using namespace std;

int main() {
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
    Index index = Index::build(dc);
    ExpertSearcher searcher(index);
    SearchResult result = searcher.search("project develop");
    for (unsigned i=0; i<result.size(); i++)
    {
    	cout << result[i].docId << "        ";
		cout << result[i].score << endl;
    }
    system("pause");
    return 0;
}
