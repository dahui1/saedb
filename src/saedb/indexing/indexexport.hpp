#pragma once
#include "indexing.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <stdlib.h>
#include "serialization/serialization_includes.hpp"

template <typename T>
inline T parse(const string& data) {
    return sae::serialization::convert_from_string<T>(data);
}

void indexexport(std::vector<indexing::Index> index_shards, std::string type) {
    std::ofstream wm(type+"_wordmap.txt");
    std::ofstream indexdata(type+"_index.txt");
    for (int i = 0; i < index_shards.size(); i++) {
        indexing::Index temp = index_shards[i];
        cout << "Serializing data of shard" << i << "..." << endl;
        //Export word map:        
        auto wordmap = temp.word_map;
        std::string code = sae::serialization::convert_to_string(wordmap);
		wm << code << '\n';
	}
    code = sae::serialization::convert_to_string((unordered_map)index_shards);
    indexdata << code;
}