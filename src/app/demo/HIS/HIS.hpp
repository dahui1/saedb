#pragma once

#include "sae_include.hpp"
#include <iostream>
#include <vector>
#include <strstream>
#include <cmath>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

#define two(X) (1<<(X)) //2µÄX´Î·½
#define twoL(X) (((int64)(1))<<(X))  //2µÄX´Î·½£¬½á¹û¿ÉÒÔÎªLongÐÍ

static vector<double> alpha;
static vector<double> beta;
static int comsLen;

namespace his {

const double RESET_PROB = 0.15;
const double MAX_DOUBLE = 1.0E100;
const double MIN_DOUBLE = -1.0E100;

typedef double vertex_data_type;
typedef double edge_data_type;
typedef float message_data_type;

struct VData {
        double pagerank; 
	int community;
	vector<double>Iv;
	vector<double>Hv;
	vector<double>Pv;
	vector<double>Iv2;
	vector<double>Hv2;
};

struct VDataDetail
{
	int id;
	string name;
	vector<int> communities;
	double score;
};

struct edge
{
	int f;
	int l;
	bool operator < (const edge &a) const 
	{
		if(f < a.f) return true;
		else if(f == a.f) 
		{
			return l < a.l;
		}
		else return false;
	}
};

struct Outdata
{
	int id;
	string name;
	vector<int> communities;
	double score;
	double pagerank;
	vector<VDataDetail> neighbor;	
	vector<edge> nei_edges;
};


struct VDatares{
	int index;
	double score;
	int community;
	double pagerank;
/*	bool operator < (const VDatares& a) const 
	{
		return score > a.score;
	}*/
};

/*
int cmp1(const VDatares& v1,const VDatares& v2)
{
	if(v1.score > v2.score) return 1;
	else return 0;
}

int cmp2(const VDatares& v1,const VDatares& v2)
{
	if(v1.pagerank > v2.pagerank) return 1;
	return 0;
}
*/
struct EData {
    double dis;
};
}
namespace sae {
    namespace serialization {
        namespace custom_serialization_impl {
            template <>
            struct serialize_impl<OSerializeStream, his::VData> {
                static void run(OSerializeStream& ostr, his::VData& d) {
                    ostr << d.pagerank << d.community  << d.Iv << d.Hv << d.Pv << d.Iv2 << d.Hv2;
                }
            };
            template <>
            struct deserialize_impl<ISerializeStream, his::VData> {
                static void run(ISerializeStream& istr, his::VData& d) {
                    istr >> d.pagerank >> d.community  >> d.Iv >> d.Hv >> d.Pv >> d.Iv2 >> d.Hv2;
                }
            };
        }
    }
}

namespace his {
typedef saedb::sae_graph graph_type;

class HIS_init: public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
       return ((1.0 - RESET_PROB) / edge.source().num_out_edges()) * edge.source().parse<VData>().pagerank;
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){
        const double newval = total + RESET_PROB;
		VData vd = vertex.parse<VData>();
		vd.pagerank = newval;
        vertex.update<VData>(vd);
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
    }

};


class HIS_Pv: public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    double gather(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){  
		VData vd = vertex.parse<VData>();
		for(int i=0;i<comsLen;++i)
		{
			for(int j=0;j<two(comsLen);++j)
			{
				if(j & (1<<i))
				{
					//cout<<"i:"<<i<<" j:"<<j<<" "<<vd.alpha[i]<< " "<<vd.beta[j]<<endl;
					vd.Pv[i] = max(vd.Pv[i],alpha[i] * vd.Iv[i] + beta[j] * vd.Hv[j]);
				}
			}
		}
		vertex.update<VData>(vd);
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
    }

};

class HIS_IHv2: public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
                VData vd = vertex.parse<VData>();
                vd.Iv2 = vd.Iv;
                vertex.update<VData>(vd);
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
        VData vd = vertex.parse<VData>();
		for(int i=0;i<comsLen;++i)
		{
			VData sourceVD = edge.source().parse<VData>();
		//	vd.Iv2[i] = vd.Iv[i];
			if(sourceVD.Pv[i] > vd.Iv2[i])
			{
				vd.Iv2[i] = sourceVD.Pv[i];							
			}			
		}
		vertex.update<VData>(vd);
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){  
	    VData vd = vertex.parse<VData>();
		for(int i=0;i<two(comsLen);++i)
		{
			for(int j=0;j<comsLen;++j)
			{
				if(i & (1<<j))
				{
					if(vd.Hv2[i] > vd.Iv2[j])
					{
						vd.Hv2[i] = vd.Iv2[j];
					}
				}
			}
		}
		vertex.update<VData>(vd);
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
    }

};

}
vector<his::Outdata> runHIS(string& path, string);
