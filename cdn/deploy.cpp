#include "deploy.h"
#include <stdio.h>
#include <iostream>

using namespace std;



//You need to complete the function 
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{

	// for(size_t i=0;i<MAX_EDGE_NUM;i++)
	// {
	// 	cout<<topo[i];
	// }

	NetworkInfo networkInfo(line_num,topo);
	networkInfo.loadNetInfo();

	NetworkNode networkNodeGroup[networkInfo.getNumNode()];

	ConsNode consNodeGroup[networkInfo.getNumCons()];

	networkInfo.loadData(networkNodeGroup, consNodeGroup);



	// //verify
	// for (size_t i=0;i<networkInfo.getNumNode();i++)
	// {
	// 	vector<NetworkEdge*>* adj=networkNodeGroup[i].getAdjEdge();
	// 	for (vector<NetworkEdge*>::iterator itr=adj->begin();itr!=adj->end();itr++)
	// 	{
	// 		cout<<i<<":"<<(*itr)->getToIndexNode()<<" "<<(*itr)->getBandWidth()<<" "<<(*itr)->getCostUnit()<<endl;
	// 	}
	// }

	// for (size_t i=0;i<networkInfo.getNumCons();i++)
	// {
	// 	cout<<i<<":"<<" "<<consNodeGroup[i].getToIndexNode()<<" "<<consNodeGroup[i].getDemand()<<endl;
	// }


	Route routeOutput;
	for (size_t i=0;i<networkInfo.getNumCons();i++)
	{
		vector<size_t>* route=new vector<size_t>();
		
		route->push_back(consNodeGroup[i].getToIndexNode());
		route->push_back(i);
		route->push_back(consNodeGroup[i].getDemand());

		routeOutput.pushRoute(route);
	}	

	string strTopo;
	routeOutput.popRouteAll2Str(strTopo);
	write_result(strTopo.c_str(), filename);
	// // Output demo
	// char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// write_result(topo_file, filename);

}


//**************************************************Class NetworkInfo**************************************************
NetworkInfo::~NetworkInfo()
{}

int NetworkInfo::str2Vec(char *str, vector<int>& vec)
{

	int ten[]={1,10,100,1000,10000,100000,1000000};
	int index=0;
	int begin=0;
	int end;
	int bit=0;
	
	// cout<<"("<<str[0]<<")"<<endl;
	vec.clear();
	while(bit<100)
	{

		if(str[index]==' '||str[index]=='\r'||str[index]==(char)(NULL))
		{
			end=index-1;
			int sum=0;
			for(int i=end;i>=begin;i--)
			{
				sum+=(str[i]-'0')*ten[end-i];
			}
			vec.push_back(sum);
			bit++;
			begin=index+1;
		}
		index++;
		if(str[index-1]=='\r'||str[index]==(char)(NULL))
		{
			break;
		}


	}
	// cout<<"<<"<<vec.size()<<">>"<<endl;
	if(bit>=100)
		return -1;
	return 0;
}

int NetworkInfo::loadNetInfo()
{
	vector<int> tmp;
	
	//read numNode numEdge numCons
	str2Vec(topo[0],tmp);
	numNode=tmp[0];
	numEdge=tmp[1];
	numCons=tmp[2];
	cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<endl;

	//read cost of server
	str2Vec(topo[2],tmp);
	costServer=tmp[0];
	cout<<tmp[0]<<endl;
	return 0;
}

int NetworkInfo::loadData(NetworkNode networkNodeGroup[], ConsNode consNodeGroup[])
{
	vector<int> tmp;	

	//read network node info 0:headNode 1:toNode 2:bandWidth 3:costUnit
	size_t lineHead=4;
	size_t lineEnd=lineHead+numEdge-1;
	for(size_t line=lineHead;line<=lineEnd;line++)
	{
		str2Vec(topo[line],tmp);
		networkNodeGroup[tmp[0]].addEdge(tmp[1], tmp[2], tmp[3]);
		networkNodeGroup[tmp[1]].addEdge(tmp[0], tmp[2], tmp[3]);
		// cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<" "<<tmp[3]<<endl;
	}

	//read consumer node info 0:consNode 1:toNode 2:demand
	lineHead=lineEnd+2;
	lineEnd=lineHead+numCons-1;
	for(size_t line=lineHead;line<=lineEnd;line++)
	{
		str2Vec(topo[line],tmp);
		consNodeGroup[tmp[0]].update(tmp[1],tmp[2]);
		networkNodeGroup[tmp[1]].connectCons(tmp[0]);
		// cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<endl;
	}

	return 0;
}

//**************************************************Class NetworkNode**************************************************
NetworkNode::NetworkNode()
{
	toIndexCons=-1;
}

NetworkNode::~NetworkNode()
{
	while (adjEdge.size()!=0)
	{
		NetworkEdge* tmp=adjEdge.back();
		delete tmp;
		adjEdge.pop_back();
	}
}

int NetworkNode::addEdge(size_t _toIndexNode, size_t _bandWidth, size_t _costUnit)
{
	NetworkEdge* tmp=new NetworkEdge(_toIndexNode, _bandWidth, _costUnit);
	adjEdge.push_back(tmp);
	return 0;
}

//**************************************************Class NetworkEdge**************************************************
NetworkEdge::~NetworkEdge()
{}

//**************************************************Class ConsNode**************************************************
ConsNode::ConsNode()
{
	demand=0;
	toIndexNode=-1;
}

ConsNode::~ConsNode()
{}

//**************************************************Class Route**************************************************
Route::Route()
{}

Route::~Route()
{}

int Route::pushRoute(vector<size_t>* route)
{
	routeG.push_back(route);
	return 0;
}

int Route::popRouteAll2Str(string& str)
{

	ostringstream tmp;
	tmp<<routeG.size()<<"\n\n";
	for(size_t i=0;i<routeG.size();i++)
	{
		for (size_t j=0;j<routeG[j]->size()-1;j++)
		{
			tmp<<(*(routeG[i]))[j]<<" ";
		}

		tmp<<routeG[i]->back();
		
		if (i!=routeG.size()-1)
		{
			tmp<<"\n";
		}
	}
	str=tmp.str();

	return 0;
}