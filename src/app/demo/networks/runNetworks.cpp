#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <strstream>

#include "glog/logging.h"
#include "networks.hpp"
#include "test/testharness.hpp"

using namespace std;
using namespace sae::io;
using namespace Networks;

vector<VData> vdVector;
map<string,string> id2name;
set<his::edge> nedges;
int kpersons;

void buildGraph(string& graphpath, const string& filepath) {
    sae::io::GraphBuilder<int> b;
    b.AddVertexDataType("VData");
    b.AddEdgeDataType("EData");       
    int n, m;	
	ifstream in2;
	in2.open(filepath + "/id2name.txt");
	string line,f,s;
	while(getline(in2,line))
	{
		int len = line.length();
		int firstpos = line.find_first_of(' ');
		f = line.substr(0,firstpos);
		s = line.substr(firstpos+1,len-firstpos-1);
		id2name[f] = s;
	}
        in2.close();
	ifstream in;		
	in.open(filepath + "/community.txt");

	int comsID;
	int index = 0;
	while(in>>comsID)
	{
		b.AddVertex(index, VData{index,0.0,comsID}, "VData");
		vdVector.push_back(VData{index,0.0,comsID});
		index++;
	}		
	in.close();
	in.open(filepath + "/options.txt");
	in>>kpersons;
	in.close();
	in.open(filepath + "/data.txt");//¶ÁÈ¡dblp_graphÖÐµÄÍ¼Êý¾Ý
	in>>n>>m;
	for(int i=0;i<m;++i)
	{
		int fv,sv; 
		in>>fv>>sv;
		nedges.insert(his::edge{fv,sv});
		nedges.insert(his::edge{sv,fv});
		if(vdVector[fv].community & vdVector[sv].community)
		{
			b.AddEdge(fv, sv,  EData{1.0}, "EData");
			b.AddEdge(sv, fv,  EData{1.0}, "EData");
		}
		else
		{
			b.AddEdge(fv, sv,  EData{0.5}, "EData");
			b.AddEdge(sv, fv,  EData{0.5}, "EData");
		}
	}
	vdVector.clear();
	in.close();

    b.Save(graphpath.c_str());
    LOG(INFO) << "Finish building networks graph.. ";
}

vector<his::Outdata>  runNetworks(string& path) {
    string graphpath = saedb::test::TempFileName();
    buildGraph(graphpath, path);
    LOG(INFO) << "Start running networks..";
    graph_type graph;
    graph.load_format(graphpath);
    saedb::IEngine<networks> *engine = new saedb::EngineDelegate<networks>(graph);
    engine->signalAll();
    engine->start();

	double max_NC = 0;
	for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        max_NC = max(max_NC,graph.vertex(i).parse<VData>().val);
    }
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
		VData vd = graph.vertex(i).parse<VData>();
		vd.val /= max_NC;
		vd.degree = graph.vertex(i).num_in_edges();
		strstream ss;
		ss >> vd.index;
		string stemp;
		ss << stemp;
	    graph.vertex(i).update<VData>(vd);
		vdVector.push_back(vd);
    }
	sort(vdVector.begin(),vdVector.end());
 	ofstream out;
	out.open("/home/yaohui/download/resultNET.txt");
	for(int i=0;i<(int)vdVector.size();++i)
	{
		strstream ss;
		string stemp;
		ss<<vdVector[i].index;
		ss>>stemp;
		string st = id2name[ stemp ];
		out<<vdVector[i].index<<"	"<<st.substr(0,st.length()-1)<<"	"<<vdVector[i].val<<endl;
	}
	out.close();
    vector<his::Outdata> result;
	int k = 0;
	int index = 0;
	for(int i=0;i<(int)vdVector.size();++i)
	{
		if(vdVector[i].degree < 2) continue;
		k++;
		if(k > kpersons) break;
        	his::Outdata ot;
		ot.id = vdVector[i].index;
		strstream ss;
		string stemp;
		ss<<vdVector[i].index;
		ss>>stemp;
		ot.name = id2name[ stemp];
		for(int j=0;j<20;++j)
		{
			if(vdVector[i].community & (1 << j))
			{
				ot.communities.push_back(j);
			}	
		}
		ot.score = vdVector[i].val;
		
		int inedgesnum = graph.vertex(vdVector[i].index).num_in_edges();
		sae::io::EdgeIteratorPtr v = graph.vertex(vdVector[i].index).in_edges();
		while(v->Alive())
	        {
			his::VDataDetail odd;
			odd.id = v->SourceId();
			strstream ss2;
			ss2<<odd.id;
			ss2>>stemp;
			odd.name = id2name[ stemp ];
			VData vd = sae::serialization::convert_from_string<VData>(v->Source()->Data());
			for(int k=0;k<20;++k)
			{
				if(vd.community & (1 << k))
				{
					odd.communities.push_back(k);	
				}
			}
			odd.score = vd.val;

			ot.neighbor.push_back(odd);
			v->Next();
		}
		int neighborLen = ot.neighbor.size();
		for(int k=0;k<neighborLen;++k)
		{
			for(int p=k+1;p<neighborLen;++p)
			{
				if(nedges.find(his::edge{ot.neighbor[k].id,ot.neighbor[p].id}) != nedges.end())
				{
					ot.nei_edges.push_back(his::edge{k,p});
				}
			}
		}
		result.push_back(ot);
	}

    LOG(INFO) << "Finish running networks..";
    delete engine;
    const char* rm = ("rm " + graphpath + ".*").c_str();
    system(rm);
    vdVector.clear();
    id2name.clear();
    nedges.clear();
    return result;
}
