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


	//vreify SAP
	consNodeGroup[0].sap(networkNodeGroup,networkInfo,38,16);
	cout<<"--------------------"<<endl;
	vector<Flow* >& flowLib=*(consNodeGroup[0].getFlowLib());
	for (size_t i=0;i<flowLib.size();i++)
	{
		vector<pair<size_t,size_t>* > *path=flowLib[i]->getPath();

		cout<<i+1<<"("<<flowLib[i]->getRestFlow()<<"):";
		for(size_t j=0;j<path->size();j++)
		{
			pair<size_t,size_t>* tmpPair=(*path)[j];
			cout<<tmpPair->first<<"<"<<tmpPair->second<<"> ";
		}
		cout<<flowLib[i]->getEnd()<<endl;
	}

	//SAP
	/*
	size_t start=38;
	size_t end=16;
	size_t neck;
	size_t u;
	size_t h[networkInfo.getNumNode()];
	size_t cur_flow,flow_ans=0,tmp;

	int numh[networkInfo.getNumNode()],curEdges[networkInfo.getNumNode()],pre[networkInfo.getNumNode()];
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		numh[i]=0;
		h[i]=0;
		pre[i]=-1;
		curEdges[i]=0;
	}

	numh[0]=networkInfo.getNumNode();
	u=start;

	cout<<"augment route: "<<endl;
	while(h[start]<networkInfo.getNumNode())
	{
		if(u==end)
		{
			cur_flow=9999;


			static size_t routeIndex=0;
			routeIndex++;
			cout<<routeIndex<<"(";
			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				if(cur_flow>networkNodeGroup[i].getBandWidth(curEdges[i]))
				{
					neck=i;
					cur_flow=networkNodeGroup[i].getBandWidth(curEdges[i]);
				}



			}

			cout<<cur_flow<<"): ";

			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				networkNodeGroup[i].editBandWidth(curEdges[i],-cur_flow);
				size_t antiNode=networkNodeGroup[i].getToIndexNode(curEdges[i]);
				size_t antiEdge=networkNodeGroup[i].getAntiEdgeIndex(curEdges[i]);
				networkNodeGroup[antiNode].editBandWidth(antiEdge,cur_flow);
				
				//verify the route
				cout<<i<<"<"<<curEdges[i]<<">"<<" ";
			}

			cout<<end<<endl;

			flow_ans+=cur_flow;
			u=neck;
		}
		// vector<NetworkEdge*>* adj=networkNodeGroup[u].getAdjEdge();

		// vector<NetworkEdge*>::iterator itr;
		// for (vector<NetworkEdge*>::iterator itr=adj->begin()+curEdges[i];itr!=adj->end();itr++)
		// {
		// 	if((*itr)->getBandWidth()&&h[u]==h[(*itr)->getToIndexNode()]+1)
		// 	{
		// 		break;
		// 	}
		// }

		size_t i;
		for (i=curEdges[u];i<networkNodeGroup[u].getEdgeSize();i++)
		{
			if(networkNodeGroup[u].getBandWidth(i)&&h[u]==h[networkNodeGroup[u].getToIndexNode(i)]+1)
			{
				break;
			}
		}

		if(i!=networkNodeGroup[u].getEdgeSize())
		{
			curEdges[u]=i;
			pre[networkNodeGroup[u].getToIndexNode(i)]=u;
			u=networkNodeGroup[u].getToIndexNode(i);
		}
		else
		{
			if(0==--numh[h[u]])break;
			curEdges[u]=0;

			for(tmp=networkInfo.getNumNode(),i=0;i<networkNodeGroup[u].getEdgeSize();i++)
			{
				if(networkNodeGroup[u].getBandWidth(i))
					tmp=(tmp<h[networkNodeGroup[u].getToIndexNode(i)]?tmp:h[networkNodeGroup[u].getToIndexNode(i)]);
			}
			h[u]=tmp+1;
			++numh[h[u]];
			if(u!=start)
				u=pre[u];
		}

	}*/



	// Route routeOutput;
	// for (size_t i=0;i<networkInfo.getNumCons();i++)
	// {
	// 	vector<size_t>* route=new vector<size_t>();
		
	// 	route->push_back(consNodeGroup[i].getToIndexNode());
	// 	route->push_back(i);
	// 	route->push_back(consNodeGroup[i].getDemand());

	// 	routeOutput.pushRoute(route);
	// }	

	// string strTopo;
	// routeOutput.popRouteAll2Str(strTopo);
	// write_result(strTopo.c_str(), filename);
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
		
		size_t lastIdx0=networkNodeGroup[tmp[0]].getEdgeSize()-1;
		size_t lastIdx1=networkNodeGroup[tmp[1]].getEdgeSize()-1;

		networkNodeGroup[tmp[0]].updateAntiEdgeIndex(lastIdx0,lastIdx1);
		networkNodeGroup[tmp[1]].updateAntiEdgeIndex(lastIdx1,lastIdx0);

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

// int NetworkNode::mergeDir(NetworkNode networkNodeGroup[])
// {

// 	for (size_t i=0;i<adjEdge.size();i++)
// 	{
// 		NetworkNode& toNode=networkNodeGroup[getToIndexNode(i)];
// 		for(size_t j=0;j<toNode.getEdgeSize();j++)
// 		{
// 			if(toNode.getToIndexNode(j)<)
// 		}
		
// 	}
// 	return 0;
// }

//**************************************************Class NetworkEdge**************************************************
NetworkEdge::~NetworkEdge()
{}

//**************************************************Class ConsNode**************************************************
ConsNode::ConsNode()
{
	demand=0;
	toIndexNode=-1;
	totalFlow=0;
}

ConsNode::~ConsNode()
{}

int ConsNode::sap(NetworkNode networkNodeGroup[],NetworkInfo networkInfo,size_t start, size_t end)
{
	size_t neck=-1;
	size_t u;
	size_t h[networkInfo.getNumNode()];
	size_t cur_flow,flow_ans=0,tmp;

	int numh[networkInfo.getNumNode()],curEdges[networkInfo.getNumNode()],pre[networkInfo.getNumNode()];
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		numh[i]=0;
		h[i]=0;
		pre[i]=-1;
		curEdges[i]=0;
	}

	numh[0]=networkInfo.getNumNode();
	u=start;

	cout<<"augment route: "<<endl;
	while(h[start]<networkInfo.getNumNode())
	{
		if(u==end)
		{
			cur_flow=9999;


			static size_t routeIndex=0;
			routeIndex++;

			Flow *tmp=new Flow(start,end);

			cout<<routeIndex<<"(";
			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				if(cur_flow>networkNodeGroup[i].getBandWidth(curEdges[i]))
				{
					neck=i;
					cur_flow=networkNodeGroup[i].getBandWidth(curEdges[i]);
				}



			}

			cout<<cur_flow<<"): ";
			tmp->editFlow(cur_flow);

			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				networkNodeGroup[i].editBandWidth(curEdges[i],-cur_flow);
				size_t antiNode=networkNodeGroup[i].getToIndexNode(curEdges[i]);
				size_t antiEdge=networkNodeGroup[i].getAntiEdgeIndex(curEdges[i]);
				networkNodeGroup[antiNode].editBandWidth(antiEdge,cur_flow);
				
				//verify the route
				cout<<i<<"<"<<curEdges[i]<<">"<<" ";

				tmp->pushPair(i,curEdges[i]);
			}

			cout<<end<<endl;

			flowLib.push_back(tmp);
			totalFlow+=cur_flow;

			flow_ans+=cur_flow;
			u=neck;
		}
		// vector<NetworkEdge*>* adj=networkNodeGroup[u].getAdjEdge();

		// vector<NetworkEdge*>::iterator itr;
		// for (vector<NetworkEdge*>::iterator itr=adj->begin()+curEdges[i];itr!=adj->end();itr++)
		// {
		// 	if((*itr)->getBandWidth()&&h[u]==h[(*itr)->getToIndexNode()]+1)
		// 	{
		// 		break;
		// 	}
		// }

		size_t i;
		for (i=curEdges[u];i<networkNodeGroup[u].getEdgeSize();i++)
		{
			if(networkNodeGroup[u].getBandWidth(i)&&h[u]==h[networkNodeGroup[u].getToIndexNode(i)]+1)
			{
				break;
			}
		}

		if(i!=networkNodeGroup[u].getEdgeSize())
		{
			curEdges[u]=i;
			pre[networkNodeGroup[u].getToIndexNode(i)]=u;
			u=networkNodeGroup[u].getToIndexNode(i);
		}
		else
		{
			if(0==--numh[h[u]])break;
			curEdges[u]=0;

			for(tmp=networkInfo.getNumNode(),i=0;i<networkNodeGroup[u].getEdgeSize();i++)
			{
				if(networkNodeGroup[u].getBandWidth(i))
					tmp=(tmp<h[networkNodeGroup[u].getToIndexNode(i)]?tmp:h[networkNodeGroup[u].getToIndexNode(i)]);
			}
			h[u]=tmp+1;
			++numh[h[u]];
			if(u!=start)
				u=pre[u];
		}

	}
	return 0;
}

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

//**************************************************Class Route**************************************************
//**************************************************Class Flow**************************************************
int Flow::pushPair(size_t nodeIndex, size_t edgeIndex)
{
	pair<size_t, size_t>* _pair=new pair<size_t, size_t>(nodeIndex,edgeIndex);
	path.push_back(_pair);
	return 0;
}