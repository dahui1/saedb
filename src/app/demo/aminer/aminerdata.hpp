#include <iostream>
#include <unordered_map>
#include <string>
#include "io/mgraph.hpp"
#include "serialization/serialization_includes.hpp"
#include "aminer.hpp"
#include "indexing/search.hpp"
#include "expert_searcher.hpp"
#include "analyzer.hpp"

#define BM25_K 2.0
#define BM25_B 0.75

using namespace std;
using namespace sae::io;

template <typename T>
inline T parse(const string& data) {
    return sae::serialization::convert_from_string<T>(data);
}

template <typename T>
inline std::unique_ptr<T> pparse(const string& data) {
    T *ret;
    std::stringstream stream;
    stream.str(data);
    sae::serialization::ISerializeStream decoder(&stream);
    decoder >> (*ret);
    return std::unique_ptr<T>(ret);
}

struct AMinerData {
	AMinerData(char const * prefix);

    ~AMinerData() {
    }

    indexing::SearchResult searchPublications(const string& query) {
        return indexing::Searcher(pub_index).search(query, g);
    }

    indexing::SearchResult searchAuthors(const string& query) {
        return ExpertSearcher(pub_index).search(query, g);
    }

public:
    indexing::Index getAuthorIndex() {
    	return author_index;
    }

    indexing::Index getPubIndex() {
    	return pub_index;
    }

    indexing::Index getConfIndex() {
    	return jconf_index;
    }

    std::unique_ptr<MappedGraph> getGraph() {
    	return g;
    }

    static AMinerData& instance()
    {
    	static AMinerData instance("aminer");
    	return instance;
    }

    template<typename T>
    T get(vid_t id) {
        auto vi = g->Vertices();
        vi->MoveTo(id);
        return parse<T>(vi->Data());
    }

private:
    indexing::Index author_index, pub_index, jconf_index;
    std::unique_ptr<MappedGraph> g;
};

