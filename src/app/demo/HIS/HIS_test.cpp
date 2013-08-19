#include <iostream>
#include <cmath>
#include <fstream>
#include <set>
#include <vector>

#include "HIS.hpp"
#include "test/testharness.hpp"


using namespace std;
using namespace sae::io;
int n,m;
vector<int> coms;
vector<int> target_communities;
int countbit(int n) { return (n==0)?0:(1+countbit(n&(n-1)));}

/*
 * serialization/deserialization for vertex data
 */

struct HISTest {
    string filepath;

    HISTest() {
        filepath = saedb::test::TempFileName();
        sae::io::GraphBuilder<int> b;
        b.AddVertexDataType("VData");
        b.AddEdgeDataType("EData");        

		ifstream in;
		in.open("/home/ling/xgy/src/app/demo/HIS/community2.txt");//读取community中的id
		int comID,target_comID;
		while(in>>comID)
		{
			coms.push_back(comID);
		}
		in.close();
		in.open("/home/ling/xgy/src/app/demo/HIS/target_communities.txt");//读取target_communities中的id
		while(in>>target_comID)
		{
			target_communities.push_back(target_comID);
		}
		in.close();
		in.open("/home/ling/xgy/src/app/demo/networks/data.txt");//读取dblp_graph中的图数据
		in>>n>>m;
		int comsLen = target_communities.size();
		for(int i=0;i<n;++i)
		{
			VData vdata;
			vdata.pagerank = 1.0;
			vdata.comsLen = comsLen;
			vdata.community = coms[i];
			vdata.Iv.resize(comsLen,0);
			vdata.Iv2.resize(comsLen,0);
			vdata.Pv.resize(comsLen,0);
			vdata.Hv.resize(two(comsLen),0);
			vdata.Hv2.resize(two(comsLen),MAX_DOUBLE);
			vdata.alpha.resize(comsLen,0.3);
			vdata.beta.resize(two(comsLen));
			for (int j=0;j<two(comsLen);j++) 
			{
				int res = countbit(j);
				if (res==0) vdata.beta[j]=0;
				else if (res==1) vdata.beta[j]=0;
				else if (res==2) vdata.beta[j]=0.17;
				else if (res==3) vdata.beta[j]=0.25;
				else if (res==4) vdata.beta[j]=0.29;
				else if (res==5) vdata.beta[j]=0.30;
				else if (res>=6) vdata.beta[j]=0.35;
			}
			b.AddVertex(i,  vdata, "VData");
		}
		for(int i=0;i<m;++i)
		{
			int fv,sv; 
			in>>fv>>sv;
			b.AddEdge(fv, sv,  EData{0}, "EData");
			b.AddEdge(sv, fv,  EData{0}, "EData");
		}
		in.close();

        b.Save(filepath.c_str());
        cout << "HISTest:Generating graph data done. " << endl;
    }

    ~HISTest() {
        // TODO remove temp graph files
    }
};

TEST(HISTest, HIS) {
    cout << "HISTest:[HISTest] Start test." << endl;
    graph_type graph;
    graph.load_format(filepath);
    saedb::IEngine<HIS_init> *engine = new saedb::EngineDelegate<HIS_init>(graph);
    for(int i=0;i<100;++i)
	{
		engine->signalAll();
		engine->start();
		
		double max_pagerank = 0.0;
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			max_pagerank = max(max_pagerank,graph.vertex(j).parse<VData>().pagerank);
		}
		if (max_pagerank < 1E-10) break;
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			VData vd = graph.vertex(j).parse<VData>();
			vd.pagerank /= max_pagerank;
			graph.vertex(j).update<VData>(vd);
    	}
		cout<<i<<endl;
	}

	for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        VData vd = graph.vertex(i).parse<VData>();
		int comsLen = target_communities.size();
		for(int j=0;j<comsLen;++j)
		{
			if((vd.community & target_communities[j]) == target_communities[j])
			{
				vd.Iv[ j ] = vd.pagerank; 
			}
		}
		graph.vertex(i).update<VData>(vd);
    }
    int step;
	for(step=0;step <100;++step)
	{
		saedb::IEngine<HIS_Pv> *engine2 = new saedb::EngineDelegate<HIS_Pv>(graph);
		engine2->signalAll();
		engine2->start();
		
		saedb::IEngine<HIS_IHv2> *engine3 = new saedb::EngineDelegate<HIS_IHv2>(graph);
		engine3->signalAll();
		engine3->start();
		double maxval = 0.0;
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			VData vd = graph.vertex(j).parse<VData>();
			for(int i=0;i<vd.comsLen;++i)
			{
				maxval = max(maxval,fabs(vd.Iv[i]-vd.Iv2[i]));
			}
			for(int i=0;i<two(vd.comsLen);++i)
			{
				maxval = max(maxval,fabs(vd.Hv[i]-vd.Hv2[i]));
			}
		}
		if(maxval > 1E-10)
		{
			for (auto j = 0; j < graph.num_local_vertices(); j ++) 
			{
				VData vd = graph.vertex(j).parse<VData>();
				for(int i=0;i<vd.comsLen;++i)
				{
					vd.Iv[i] = vd.Iv2[i];
					vd.Iv2[i] = 0;
					vd.Pv[i] = 0;
				}
				
				for(int i=0;i<two(vd.comsLen);++i)
				{
					vd.Hv[i] = vd.Hv2[i];
					vd.Hv2[i] = MAX_DOUBLE;
				}
				graph.vertex(j).update(vd);
			}
		}
		else
		{
			break;
		}
	}
	cout<<"run:"<<step<<"times."<<endl;
	cout<<"Importance I:"<<endl;
	for (auto j = 0; j < graph.num_local_vertices(); j ++) 
	{
		VData vd = graph.vertex(j).parse<VData>();		
		for(int i=0;i<vd.comsLen;++i)
		{
			cout<<vd.Iv[i]<<" ";
		}
		cout<<endl;
	}
	cout<<"structural hole score:"<<endl;
	for (auto j = 0; j < graph.num_local_vertices(); j ++)
	{
	    VData vd = graph.vertex(j).parse<VData>();
		for(int i=0;i<two(vd.comsLen);++i)
		{
			cout<<vd.Hv[i]<<" ";
		}
		cout<<endl;
	}
	
    cout << "HISTest:[HISTest] End test." << endl;
    delete engine;
}

int main(){
    saedb::test::RunAllTests();
}

