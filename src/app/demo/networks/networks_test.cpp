#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include "networks.hpp"
#include "test/testharness.hpp"

using namespace std;
using namespace sae::io;
int n,m;
vector<VData> vdVector;

/*
 * serialization/deserialization for vertex data
 */

struct networksTest {
    string filepath;

    networksTest() {
        filepath = saedb::test::TempFileName();
        sae::io::GraphBuilder<int> b;
        b.AddVertexDataType("VData");
        b.AddEdgeDataType("EData");       
		
		ifstream in;		
		in.open("/home/ling/xgy/src/app/demo/networks/data.txt");//读取dblp_graph中的图数据
		in>>n>>m;
		
		for(int i=0;i<n;++i)
		{
			b.AddVertex(i,  VData{i,0.0}, "VData");
		}
		
		for(int i=0;i<m;++i)
		{
			int fv,sv; 
			in>>fv>>sv;
			b.AddEdge(fv, sv,  EData{1.0}, "EData");
			b.AddEdge(sv, fv,  EData{1.0}, "EData");
		}
		in.close();


        b.Save(filepath.c_str());
        cout << "networksTest:Generating graph data done. " << endl;
    }

    ~networksTest() {
        // TODO remove temp graph files
    }
};

TEST(networksTest, networks) {
    cout << "networksTest:[networksTest] Start test." << endl;
    graph_type graph;
    graph.load_format(filepath);
    saedb::IEngine<networks> *engine = new saedb::EngineDelegate<networks>(graph);
    engine->signalAll();
    engine->start();

    // for debug purpose, print out all
	for (auto i = 0; i < graph.num_local_vertices(); i ++) {
	    sae::io::EdgeIteratorPtr v = graph.vertex(i).in_edges();
		while(v->Alive())
		{
			int sourceID = v->SourceId();
			double edgeVal = sae::serialization::convert_from_string<EData>(v->Data()).sum;
			cout<<"e["<< i << "," << sourceID << "]:"<<setprecision(16)<<edgeVal<<endl;
			v->Next();
		}
    }
	double max_NC = 0;
	for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        max_NC = max(max_NC,graph.vertex(i).parse<VData>().val);
    }
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
		VData vd = graph.vertex(i).parse<VData>();
		vd.val /= max_NC;
	    graph.vertex(i).update<VData>(vd);
		vdVector.push_back(vd);
        cout << "v[" << i << "]: " <<setprecision(16) << graph.vertex(i).parse<VData>().val << endl;
    }
	sort(vdVector.begin(),vdVector.end());
	for(int i=0;i<graph.num_local_vertices();++i)
	{
		cout << "v[" << vdVector[i].index << "]: " <<setprecision(16) << vdVector[i].val << endl;
	}

    // compare with known answers
   // vector<double> results = {0.15, 0.2775, 0.385875, 0.477994};
    //for (auto i = 0; i < graph.num_local_vertices(); i ++) {
   //     ASSERT_TRUE(abs(results[i] - graph.vertex(i).parse<double>()) < TOLERANCE);
   //     ASSERT_TRUE(graph.vertex(i).data_type_name() == "VData");
   // }
    cout << "networksTest:[networksTest] End test." << endl;
    delete engine;
}

int main(){
    saedb::test::RunAllTests();
}

