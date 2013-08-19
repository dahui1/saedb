#pragma once

#include "sae_include.hpp"
#include <iostream>
using namespace std;

double RESET_PROB = 0.15;
double TOLERANCE = 1.0E-2;
const double MAX_DOUBLE = 1.0E100;
const double MIN_DOUBLE = -1.0E100;

typedef double vertex_data_type;
typedef double edge_data_type;
typedef float message_data_type;

struct VData {
    double dis;
};

struct EData {
    double dis;
};

typedef saedb::sae_graph graph_type;

class xgytest:
public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
        //vertex.data() = msg;
        if((int)vertex.id() == 0){
            vertex.update<double>(0.0);
        }else{
            vertex.update<double>(MAX_DOUBLE);
        }
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
        double newval = edge.parse<EData>().dis + edge.source().parse<double>();
		if(newval < vertex.parse<double>())
		{
			vertex.update<double>(newval);
		}
		
		
		sae::io::EdgeIteratorPtr v = vertex.in_edges();
		while(v->Alive())
		{
			cout<<v->GlobalId()<<":"<< sae::serialization::convert_from_string<EData>(v->Data()).dis<<endl;;
			v->Next();
		}
        return newval;
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){
        //const double newval = min(total,vertex.parse<double>());
        //vertex.update<double>(newval);
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::OUT_EDGES;
    }

    void scatter(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
        if (vertex.parse<double>() + edge.parse<EData>().dis < edge.target().parse<double>())
            context.signal(edge.target());
    }

};
