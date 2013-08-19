#pragma once

#include "sae_include.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
using namespace std;

#define two(X) (1<<(X)) //2的X次方
#define twoL(X) (((int64)(1))<<(X))  //2的X次方，结果可以为Long型

double RESET_PROB = 0.15;
double TOLERANCE = 1.0E-2;
const double MAX_DOUBLE = 1.0E100;
const double MIN_DOUBLE = -1.0E100;

typedef double vertex_data_type;
typedef double edge_data_type;
typedef float message_data_type;

struct VData {
    double pagerank; //节点排名，即重要性
	int comsLen;//commubity的数量
	int community;//所有community的二进制缩略表示
	vector<double>alpha;//alpha值的集合
	vector<double>beta;//beta值的集合
	vector<double>Iv;
	vector<double>Hv;
	vector<double>Pv;
	vector<double>Iv2;
	vector<double>Hv2;
};

struct EData {
    double dis;
};
namespace sae {
    namespace serialization {
        namespace custom_serialization_impl {
            template <>
            struct serialize_impl<OSerializeStream, VData> {
                static void run(OSerializeStream& ostr, VData& d) {
                    ostr << d.pagerank << d.comsLen << d.community <<d.alpha <<d.beta << d.Iv << d.Hv << d.Pv << d.Iv2 << d.Hv2;
                }
            };
            template <>
            struct deserialize_impl<ISerializeStream, VData> {
                static void run(ISerializeStream& istr, VData& d) {
                    istr >> d.pagerank >> d.comsLen >> d.community >> d.alpha >> d.beta >> d.Iv >> d.Hv >> d.Pv >> d.Iv2 >> d.Hv2;
                }
            };
        }
    }
}

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
		for(int i=0;i<vd.comsLen;++i)
		{
			for(int j=0;j<two(vd.comsLen);++j)
			{
				if(j & (1<<i))
				{
					//cout<<"i:"<<i<<" j:"<<j<<" "<<vd.alpha[i]<< " "<<vd.beta[j]<<endl;
					vd.Pv[i] = max(vd.Pv[i],vd.alpha[i] * vd.Iv[i] + vd.beta[j] * vd.Hv[j]);
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
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, vertex_type& vertex,
                 edge_type& edge) const {
        VData vd = vertex.parse<VData>();
		for(int i=0;i<vd.comsLen;++i)
		{
			VData sourceVD = edge.source().parse<VData>();
			vd.Iv2[i] = vd.Iv[i];
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
		for(int i=0;i<two(vd.comsLen);++i)
		{
			for(int j=0;j<vd.comsLen;++j)
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
