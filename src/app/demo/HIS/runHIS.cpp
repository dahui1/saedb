#include "HIS.hpp"
#include "test/testharness.hpp"

using namespace std;
using namespace sae::io;
using namespace his;

vector<int> coms;
vector<int> target_communities;
map<string,string> id2namemap;
set<edge> edges;
int kperson;
int countbit(int n) { return (n==0)?0:(1+countbit(n&(n-1)));}

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


void buildGraph(const string& graphpath, const string& filepath) {
    sae::io::GraphBuilder<int> b;
    b.AddVertexDataType("VData");
    b.AddEdgeDataType("EData");        
    int m, n;
	ifstream in2;
	in2.open(filepath + "/id2name.txt");
	string line,f,s;
	while(getline(in2,line))
	{	
		int len = line.length();
		int firstpos = line.find_first_of(' ');
		f = line.substr(0,firstpos);
		s = line.substr(firstpos+1,len-firstpos-1);
		id2namemap[f] = s;
	}
	in2.close();
	ifstream in;
	in.open(filepath + "/community.txt");
	int comID,target_comID;
	while(in>>comID)
	{
		coms.push_back(comID);
	}
	in.close();
	in.open(filepath + "/options.txt");
	in>>kperson;
	cout<<"kperson:"<<kperson<<endl;
        in>>target_comID;
	cout<<"target_comID:"<<target_comID<<endl;
	for(int i=0;i<20;++i)
	{
		if(target_comID & (1 << i))
		{
			cout<<"1<<i:"<<(1<<i)<<endl;
			target_communities.push_back(1 << i);
		}
	}
	in.close();
	in.open(filepath + "/data.txt");
	in>>n>>m;
	comsLen = target_communities.size();
	alpha.clear();
	beta.clear();
	alpha.resize(comsLen);
	beta.resize(two(comsLen));
	for(int j=0;j<two(comsLen);++j)
	{
		int res = countbit(j);
		if(res == 0) beta[j] = 0;
		else if(res == 1) beta[j] = 0;
		else if(res == 2) beta[j] = 0.17;
		else if(res == 3) beta[j] = 0.25;
		else if(res == 4) beta[j] = 0.29;
		else if(res == 5) beta[j] = 0.30;
		else if(res >= 6) beta[j] = 0.35;
	}
	for(int i=0;i<n;++i)
	{
		VData vdata;
		vdata.pagerank = 1.0;
		vdata.community = coms[i];
		vdata.Iv.resize(0);
		vdata.Iv2.resize(0);
		vdata.Pv.resize(0);
		vdata.Hv.resize(0);
		vdata.Hv2.resize(0);
		b.AddVertex(i,  vdata, "VData");
	}
	for(int i=0;i<m;++i)
	{
		int fv,sv; 
		in>>fv>>sv;
		edges.insert(edge{fv,sv});
		edges.insert(edge{sv,fv});
		b.AddEdge(fv, sv,  EData{0}, "EData");
		b.AddEdge(sv, fv,  EData{0}, "EData");
	}
	in.close();

    b.Save(graphpath.c_str());
    cout << "HISTest:Generating graph data done. " << endl;
}

vector<Outdata> runHIS(string& path,string flag) {
    string graphpath = saedb::test::TempFileName();
    buildGraph(graphpath, path);
    cout << "Start running HIS.." << endl;
    graph_type graph;
    graph.load_format(graphpath);
    saedb::IEngine<HIS_init> *engine = new saedb::EngineDelegate<HIS_init>(graph);
    for(int i=0;i<20;++i)
	{
		engine->signalAll();
		engine->start();
		double max_pagerank_cha = 0.0;
		double max_pagerank = 0.0;
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			max_pagerank = max(max_pagerank,graph.vertex(j).parse<VData>().pagerank);
		}
		if (max_pagerank < 1E-5) break;
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			VData vd = graph.vertex(j).parse<VData>();
			double temp = vd.pagerank;
			vd.pagerank /= max_pagerank;
			max_pagerank_cha = max(max_pagerank_cha,fabs(temp-vd.pagerank));
			graph.vertex(j).update<VData>(vd);
         	}
		if(max_pagerank_cha < 1E-5) break;
	cout<<i<<endl;
	}

	for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        	VData vd = graph.vertex(i).parse<VData>();
		vd.Iv.resize(comsLen,0);
		vd.Iv2.resize(comsLen,0);
		vd.Pv.resize(comsLen,0);
		vd.Hv.resize(two(comsLen),0);
		vd.Hv2.resize(two(comsLen),MAX_DOUBLE);
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
		cout<<"step:"<<step<<endl;
		saedb::IEngine<HIS_Pv> *engine2 = new saedb::EngineDelegate<HIS_Pv>(graph);
		engine2->signalAll();
		engine2->start();
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			VData vd = graph.vertex(j).parse<VData>();
		}		
		
		saedb::IEngine<HIS_IHv2> *engine3 = new saedb::EngineDelegate<HIS_IHv2>(graph);
		engine3->signalAll();
		engine3->start();
		double maxval = 0.0;
		for (auto j = 0; j < graph.num_local_vertices(); j ++) {
			VData vd = graph.vertex(j).parse<VData>();
			for(int i=0;i<comsLen;++i)
			{
				maxval = max(maxval,fabs(vd.Iv[i]-vd.Iv2[i]));
			}
			for(int i=0;i<two(comsLen);++i)
			{
				maxval = max(maxval,fabs(vd.Hv[i]-vd.Hv2[i]));
			}
		}
		if(maxval > 1E-5)
		{		
			for (auto j = 0; j < graph.num_local_vertices(); j ++) 
			{
				VData vd = graph.vertex(j).parse<VData>();
				for(int i=0;i<comsLen;++i)
				{
					vd.Iv[i] = vd.Iv2[i];
					vd.Iv2[i] = 0;
					vd.Pv[i] = 0;
				}
				
				for(int i=0;i<two(comsLen);++i)
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
        vector<VDatares> vdatares;

	for (auto j = 0; j < graph.num_local_vertices(); j ++)
	{
	    VData vd = graph.vertex(j).parse<VData>();
		vdatares.push_back(VDatares{j,vd.Hv[two(comsLen)-1],vd.community,vd.pagerank});
	}
	
	if(flag == "HIS") sort(vdatares.begin(),vdatares.end(),cmp1);
	else if(flag == "Pagerank") sort(vdatares.begin(),vdatares.end(),cmp2);
	ofstream out;
	out.open("/home/yaohui/download/resultHIS.txt");
	for(int i=0;i<(int)vdatares.size();++i)
	{
		strstream ss;
		string stemp;
		ss<<vdatares[i].index;
		ss>>stemp;
		string st = id2namemap[ stemp ];
		out<<vdatares[i].index<<"	"<<st.substr(0,st.length()-1)<<"	"<<vdatares[i].score<<endl;
	}
	out.close();
        kperson = min((int)vdatares.size(),kperson);
        vector<Outdata> result;
	for(int i=0;i<kperson;++i)
	{
		Outdata ot;
		ot.id = vdatares[i].index;
		strstream ss;
		string stemp;
		ss<<vdatares[i].index;
		ss >> stemp;
		ot.name = id2namemap[ stemp ];
		for(int j=0;j<comsLen;j++)
		{
			if(vdatares[i].community & target_communities[j])
			{
				ot.communities.push_back(j);
			}
		}
		ot.score = vdatares[i].score;
		ot.pagerank = vdatares[i].pagerank;
		int inedgesnum = graph.vertex(vdatares[i].index).num_in_edges();
		sae::io::EdgeIteratorPtr v = graph.vertex(vdatares[i].index).in_edges();
		while(v->Alive())
		{
			VDataDetail odd;
			odd.id = v->SourceId();
			strstream ss2;
			ss2<<odd.id;
			ss2 >> stemp;
			odd.name = id2namemap[ stemp];
			VData vd = sae::serialization::convert_from_string<VData>(v->Source()->Data());
			for(int k=0;k<comsLen;++k)
			{
           			if(vd.community & target_communities[k])
				{
					odd.communities.push_back(k);
				}
			}	
			odd.score = vd.Hv[two(comsLen)-1];		
			ot.neighbor.push_back(odd);
			v->Next();
		}
		int neighborLen = ot.neighbor.size();
		for(int k=0;k<neighborLen;++k)
		{
			for(int p=k+1;p<neighborLen;++p)
			{
				if(edges.find(edge{ot.neighbor[k].id,ot.neighbor[p].id}) != edges.end())
				{
					ot.nei_edges.push_back(edge{k,p});
				}
			}
		}
		result.push_back(ot);
	}
	
    cout << "Finish running HIS.." << endl;
    delete engine;
    coms.clear();
    target_communities.clear();
    id2namemap.clear();
    edges.clear();
    const char* rm = ("rm " + graphpath + ".*").c_str();
    system(rm);
    return result;
}
