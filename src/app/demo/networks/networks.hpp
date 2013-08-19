#pragma once

#include "sae_include.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

double RESET_PROB = 0.15;
double TOLERANCE = 1.0E-2;
const double MAX_DOUBLE = 1.0E100;
const double MIN_DOUBLE = -1.0E100;

typedef double vertex_data_type;
typedef double edge_data_type;
typedef float message_data_type;

struct VData {
	int index;
    double val;
	bool operator < (const VData& a) const 
	{
		return val > a.val;
	}
};

struct EData {
    double dis;
	double sum;
};

typedef saedb::sae_graph graph_type;

class networks:
public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
        /*if((int)vertex.id() == 0){
            vertex.update<double>(0.0);
        }else{
            vertex.update<double>(MAX_DOUBLE);
        }*/
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
        /*double newval = edge.parse<EData>().dis + edge.source().parse<double>();
		if(newval < vertex.parse<double>())
		{
			vertex.update<double>(newval);
		}*/
		
		double sumC = 0;
		sae::io::EdgeIteratorPtr v = edge.source().out_edges();
		while(v->Alive())
		{
			//cout<<v->GlobalId()<<":"<< sae::serialization::convert_from_string<EData>(v->Data()).dis<<endl;
			int targetID = v->TargetId();
			if(vertex.id() == targetID)
			{
				v->Next();
				continue;
			}			
			double edgeVal = sae::serialization::convert_from_string<EData>(v->Data()).dis;
			sae::io::EdgeIteratorPtr v2 = edge.target().in_edges();
			while(v2->Alive())
			{
				int sourceID2 = v2->SourceId();
				double edgeVal2 = sae::serialization::convert_from_string<EData>(v2->Data()).dis;
				if(sourceID2 == targetID)
				{
					sumC += edgeVal * edgeVal2;
				}
				v2->Next();
			}
			v->Next();
		}
		sumC = (edge.parse<EData>().dis + sumC) * (edge.parse<EData>().dis + sumC);
		edge.update<EData>(EData{edge.parse<EData>().dis,sumC});
        return sumC;
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){
        //const double newval = min(total,vertex.parse<double>());
		VData vd = vertex.parse<VData>();
		vd.val = total;
        vertex.update<VData>(vd);
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    void scatter(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
        //if (vertex.parse<double>() + edge.parse<EData>().dis < edge.target().parse<double>())
        //    context.signal(edge.target());
    }

};
