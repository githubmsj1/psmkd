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

	//initialize ConsNode 
	for(size_t i=0;i<networkInfo.getNumCons();i++)
		consNodeGroup[i].initCons(networkInfo);

	//verify network
	cout<<"**************************************\n";
	cout<<"NumNode: "<<networkInfo.getNumNode()<<endl;
	cout<<"NumEdge: "<<networkInfo.getNumEdge()<<endl;
	cout<<"NumCons: "<<networkInfo.getNumCons()<<endl;
	cout<<"CostServer: "<<networkInfo.getCostServer()<<endl;
	
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		vector<NetworkEdge*>* adj=networkNodeGroup[i].getAdjEdge();
		for (vector<NetworkEdge*>::iterator itr=adj->begin();itr!=adj->end();itr++)
		{
			cout<<i<<":"<<(*itr)->getToIndexNode()<<" "<<(*itr)->getBandWidth()<<" "<<(*itr)->getCostUnit()<<endl;
		}
	}
	for (size_t i=0;i<networkInfo.getNumCons();i++)
	{
		cout<<i<<":"<<" "<<consNodeGroup[i].getToIndexNode()<<" "<<consNodeGroup[i].getDemand()<<endl;
	}
	cout<<"++++++++++++++++++++++++++++++++++++++\n";

	// vector<NetworkNode> copyNetworkNodeGroup;
	// for (size_t i=0;i<networkInfo.getNumNode();i++)
	// {
	// 	NetworkNode tmp(networkNodeGroup[i]);
	// 	copyNetworkNodeGroup.push_back(tmp);
	// }
	// cout<<"--------------"<<endl;
	// //verify network
	// for (size_t i=0;i<networkInfo.getNumNode();i++)
	// {
	// 	vector<NetworkEdge*>* adj=copyNetworkNodeGroup[i].getAdjEdge();
	// 	for (vector<NetworkEdge*>::iterator itr=adj->begin();itr!=adj->end();itr++)
	// 	{
	// 		cout<<i<<":"<<(*itr)->getToIndexNode()<<" "<<(*itr)->getBandWidth()<<" "<<(*itr)->getCostUnit()<<endl;
	// 	}
	// }
	// for (size_t i=0;i<networkInfo.getNumCons();i++)
	// {
	// 	cout<<i<<":"<<" "<<consNodeGroup[i].getToIndexNode()<<" "<<consNodeGroup[i].getDemand()<<endl;
	// }


	vector<size_t>consIndex;
	for(size_t i=0;i<networkInfo.getNumCons();i++)
		consIndex.push_back(i);

	// sort client according to demand
	for(size_t i=0;i<networkInfo.getNumCons()-1;i++)
	{
		for(size_t j=i+1;j<networkInfo.getNumCons();j++)
		{
			if (consNodeGroup[consIndex[i]].getDemand()>consNodeGroup[consIndex[j]].getDemand())
			{
				size_t tmp=consIndex[i];
				consIndex[i]=consIndex[j];
				consIndex[j]=tmp;
			}
		}
	}
	//show cons idx sorted by demand
	cout<<"**************************************\n";
	cout<<"sort cons node by demand: \n";
	for(size_t i=0;i<consIndex.size();i++)
	{
		cout<<consIndex[i]<<"<"<<consNodeGroup[consIndex[i]].getDemand()<<">("<<consNodeGroup[consIndex[i]].getToIndexNode()<<") ";
	}
	cout<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++\n";


	
	//bfs sap map
	for(size_t i=0;i<networkInfo.getNumCons();i++)
	{
		consNodeGroup[i].sapBfs(networkNodeGroup,networkInfo,consNodeGroup[i].getToIndexNode());
	}

	//test whether the node of low demand is available get from other server; the 5th server position
	for(size_t i=0;i<networkInfo.getNumCons();i++)
	{
		if(consNodeGroup[consIndex[i]].getRestFlow(5)>(long int)consNodeGroup[consIndex[i]].getDemand())
		{
			size_t tmp=consIndex[0];
			consIndex[0]=consIndex[i];
			consIndex[i]=tmp;
			break;
		}
	}

	// // sort client according to restflow
	// for(size_t i=0;i<networkInfo.getNumCons()-1;i++)
	// {
	// 	for(size_t j=i+1;j<networkInfo.getNumCons();j++)
	// 	{
	// 		if (consNodeGroup[consIndex[i]].getRestFlow(3)<consNodeGroup[consIndex[j]].getRestFlow(3))
	// 		{
	// 			size_t tmp=consIndex[i];
	// 			consIndex[i]=consIndex[j];
	// 			consIndex[j]=tmp;
	// 		}
	// 	}
	// }
	// //show cons idx sorted by restflow : 5
	// cout<<"**************************************\n";
	// cout<<"sort cons node by restFlow: \n";
	// for(size_t i=0;i<consIndex.size();i++)
	// {
	// 	cout<<consIndex[i]<<"<"<<consNodeGroup[consIndex[i]].getRestFlow(3)<<">("<<consNodeGroup[consIndex[i]].getToIndexNode()<<") ";
	// }
	// cout<<endl;
	// cout<<"++++++++++++++++++++++++++++++++++++++\n";

	vector<size_t>serverPos;
	for(size_t i=1;i<consIndex.size();i++)
	{
		serverPos.push_back(consNodeGroup[consIndex[i]].getToIndexNode());
	}

	//collect flow only for the node of 0 
	// consNodeGroup[consIndex[0]].sap(networkNodeGroup,networkInfo,consNodeGroup[consIndex[1]].getToIndexNode(),consNodeGroup[consIndex[0]].getToIndexNode());
	consNodeGroup[consIndex[0]].saps(networkNodeGroup,networkInfo,serverPos,consNodeGroup[consIndex[0]].getToIndexNode());
	cout<<"select: "<<consNodeGroup[consIndex[0]].selectFlow()<<endl;


	EdgeMatrix globalEdgeMatrix(networkInfo.getNumNode(),networkInfo.getNumNode());

	// vreify SAP
	cout<<"**************************************\n";
	vector<Flow* >& flowLib=*(consNodeGroup[consIndex[0]].getFlowLib());
	for (size_t i=0;i<flowLib.size();i++)
	{
		vector<pair<size_t,size_t>* > *path=flowLib[i]->getPath();

		cout<<i+1<<"("<<flowLib[i]->getRestFlow()<<"):";
		for(size_t j=0;j<path->size();j++)
		{
			pair<size_t,size_t>* tmpPair=(*path)[j];
			cout<<tmpPair->first<<"<"<<tmpPair->second<<"> ";
		}
		// cout<<flowLib[i]->getEnd()<<endl;
		cout<<endl;
	}
	cout<<"++++++++++++++++++++++++++++++++++++++"<<endl;
	

	//test regulate
	/*
	vector<pair<size_t, size_t> >overLoadEdge;
	vector<long int> overLoad;
	overLoadEdge.push_back(pair<size_t,size_t>(22,23));
	overLoadEdge.push_back(pair<size_t,size_t>(22,24));
	overLoadEdge.push_back(pair<size_t,size_t>(24,20));
	// overLoadEdge.push_back(pair<size_t,size_t>(31,3));
	overLoad.push_back(5);
	overLoad.push_back(2);
	overLoad.push_back(1);
	// overLoad.push_back(1);
	consNodeGroup[consIndex[0]].regulate(overLoadEdge,overLoad);
	overLoadEdge.clear();
	overLoad.clear();
	overLoadEdge.push_back(pair<size_t,size_t>(22,21));
	overLoad.push_back(3);
	consNodeGroup[consIndex[0]].regulate(overLoadEdge,overLoad);
	*/

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




	Route routeOutput;
	// cout<<"Flow number: "<<consNodeGroup[consIndex[0]].getNumFlow()<<endl;
	for(size_t i=0;i<consNodeGroup[consIndex[0]].getNumFlow();i++)
	{
		vector<size_t>* route=new vector<size_t>();
		consNodeGroup[consIndex[0]].popRoute(i,route);
		routeOutput.pushRoute(route);
	}

	for (size_t i=1;i<networkInfo.getNumCons();i++)
	{
		vector<size_t>* route=new vector<size_t>();
		
		route->push_back(consNodeGroup[consIndex[i]].getToIndexNode());
		route->push_back(consIndex[i]);
		route->push_back(consNodeGroup[consIndex[i]].getDemand());

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
	// cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<endl;

	//read cost of server
	str2Vec(topo[2],tmp);
	costServer=tmp[0];
	// cout<<tmp[0]<<endl;
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
		consNodeGroup[tmp[0]].editConsIndex(tmp[0]);
	}

	return 0;
}

//**************************************************Class NetworkNode**************************************************
NetworkNode::NetworkNode()
{
	toIndexCons=-1;
}
NetworkNode::NetworkNode(const NetworkNode& _networkNode)
{
	//int addEdge(size_t _toIndexNode, size_t _bandWidth, size_t _costUnit);
	toIndexCons=-1;
	for(size_t i=0;i<_networkNode.getEdgeSize();i++)
	{	
		addEdge(_networkNode.getToIndexNode(i),_networkNode.getBandWidth(i),_networkNode.getCostUnit(i));
		size_t lastIdx0=getEdgeSize()-1;
		size_t lastIdx1=_networkNode.getAntiEdgeIndex(i);
		updateAntiEdgeIndex(lastIdx0,lastIdx1);
	}

	toIndexCons=_networkNode.getToIndexCons();
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
	indexNode=0;
	indexMaxRestFlow=-1;
}

ConsNode::~ConsNode()
{}

int ConsNode::initCons(NetworkInfo networkInfo)
{
	initMapEdgeRoute(networkInfo.getNumNode());
	return 0;
}

int ConsNode::sap(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,size_t start, size_t end)
{
	//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

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

	//delete tmp networkNodeGroup

	//sort flowlib
	sortFlow();
	return 0;
}

int ConsNode::sapBfs(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,size_t end)
{
	queue<size_t> nodeQueue;

	vector<int> visitNode(networkInfo.getNumNode(),0);
	

	if(!sortIndexRestFlow.empty())
		sortIndexRestFlow.clear();
	restFlowGraph.resize(networkInfo.getNumNode());
	for(size_t i=0;i<networkInfo.getNumNode();i++)
	{
		restFlowGraph[i]=-9999;//default
	}
	restFlowGraph[toIndexNode]=9999;//end
	sortIndexRestFlow.push_back(toIndexNode);

	// nodeQueue.push(end);
	visitNode[end]=1;
	// cout<<"BFS: "<<endl;
	for(size_t i=0;i<_networkNodeGroup[end].getEdgeSize();i++)
	{		
		nodeQueue.push(_networkNodeGroup[end].getToIndexNode(i));
		visitNode[_networkNodeGroup[end].getToIndexNode(i)]=1;
	}
	

	while(!nodeQueue.empty())
	{
		size_t idx=nodeQueue.front();
		
		nodeQueue.pop();
		long int _restFlow=(long int)(sapV(_networkNodeGroup,networkInfo,idx,end)-demand);
		restFlowGraph[idx]=_restFlow;
		sortIndexRestFlow.push_back(idx);
		// cout<<idx<<"<"<<_restFlow<<"> ";
		for(size_t i=0;i<_networkNodeGroup[idx].getEdgeSize();i++)
		{
			if(visitNode[_networkNodeGroup[idx].getToIndexNode(i)]==0)
			{
				nodeQueue.push(_networkNodeGroup[idx].getToIndexNode(i));
				visitNode[_networkNodeGroup[idx].getToIndexNode(i)]=1;
			}
		}

	}

	//sort
	for(size_t i=0;i<sortIndexRestFlow.size()-1;i++)
		for(size_t j=i+1;j<sortIndexRestFlow.size();j++)
		{
			if(restFlowGraph[sortIndexRestFlow[i]]<restFlowGraph[sortIndexRestFlow[j]])
			{
				size_t tmpIndex=sortIndexRestFlow[i];
				sortIndexRestFlow[i]=sortIndexRestFlow[j];
				sortIndexRestFlow[j]=tmpIndex;
			}
		}

	// cout<<"Sorted rest flow: ";
	// for(size_t i=0;i<sortIndexRestFlow.size()-1;i++)
	// {
	// 	cout<<sortIndexRestFlow[i]<<"<"<<restFlowGraph[sortIndexRestFlow[i]]<<"> ";
	// }
	// cout<<endl;

	indexMaxRestFlow=sortIndexRestFlow[1];//not include the end
	maxRestFlow=restFlowGraph[indexMaxRestFlow];
	// cout<<"MaxRestFlow: "<<indexMaxRestFlow<<"<"<<maxRestFlow<<">"<<endl;

	return 0;
}



size_t ConsNode::sapV(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,size_t start, size_t end)
{
		//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

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

	// cout<<"augment route: "<<endl;
	while(h[start]<networkInfo.getNumNode())
	{
		if(u==end)
		{
			cur_flow=9999;


			static size_t routeIndex=0;
			routeIndex++;

			// Flow *tmp=new Flow(start,end);

			// cout<<routeIndex<<"(";
			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				if(cur_flow>networkNodeGroup[i].getBandWidth(curEdges[i]))
				{
					neck=i;
					cur_flow=networkNodeGroup[i].getBandWidth(curEdges[i]);
				}



			}

			// cout<<cur_flow<<"): ";
			// tmp->editFlow(cur_flow);

			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				networkNodeGroup[i].editBandWidth(curEdges[i],-cur_flow);
				size_t antiNode=networkNodeGroup[i].getToIndexNode(curEdges[i]);
				size_t antiEdge=networkNodeGroup[i].getAntiEdgeIndex(curEdges[i]);
				networkNodeGroup[antiNode].editBandWidth(antiEdge,cur_flow);
				
				//verify the route
				// cout<<i<<"<"<<curEdges[i]<<">"<<" ";

				// tmp->pushPair(i,curEdges[i]);
			}

			// cout<<end<<endl;

			// flowLib.push_back(tmp);
			// totalFlow+=cur_flow;

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


	return flow_ans;
}

int ConsNode::saps(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,vector<size_t>& _start, size_t end)
{
	//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

	//construct a total start, connecting to all start.
	NetworkNode tmpNode;
	cout<<"start: ";
	for(size_t i=0;i<_start.size();i++)
	{
		cout<<_start[i]<<" ";
	}
	cout<<endl;
	for(size_t i=0;i<_start.size();i++)
	{	
		//ToIndexNode BandWidth CostUnit
		tmpNode.addEdge(_start[i],9999,0);
		networkNodeGroup[_start[i]].addEdge(networkInfo.getNumNode(),9999,0);

		size_t lastIdx0=tmpNode.getEdgeSize()-1;
		size_t lastIdx1=networkNodeGroup[_start[i]].getEdgeSize()-1;

		tmpNode.updateAntiEdgeIndex(lastIdx0,lastIdx1);
		networkNodeGroup[_start[i]].updateAntiEdgeIndex(lastIdx1,lastIdx0);
	}
	
	networkNodeGroup.push_back(tmpNode);

	size_t start=networkInfo.getNumNode();

	size_t numNode=networkInfo.getNumNode()+1;

	size_t neck=-1;
	size_t u;
	size_t h[numNode];
	size_t cur_flow,flow_ans=0,tmp;

	int numh[numNode],curEdges[numNode],pre[numNode];
	for (size_t i=0;i<numNode;i++)
	{
		numh[i]=0;
		h[i]=0;
		pre[i]=-1;
		curEdges[i]=0;
	}

	numh[0]=numNode;
	u=start;

	cout<<"augment route: "<<endl;
	while(h[start]<numNode)
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
				
				if (i!=start)//first node is virtual
				{
					//verify the route
					cout<<i<<"<"<<curEdges[i]<<">"<<" ";

					tmp->pushPair(i,curEdges[i]);
				}
			}

			cout<<end<<"<"<<0<<">"<<" ";
			tmp->pushPair(end,0);
			cout<<endl;

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

			for(tmp=numNode,i=0;i<networkNodeGroup[u].getEdgeSize();i++)
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

	//delete tmp networkNodeGroup

	//sort flowlib according to distance
	sortFlow();

	//update map from Edge to path index
	if(mapEdgeRoute.size()==0)
	{
		cout<<"mapEdgeRoute not initialized"<<endl;
	}
	for(size_t indexFlow=0;indexFlow<flowLib.size();indexFlow++)
	{
		Flow *tmpflow=flowLib[indexFlow];
		vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
		for(size_t i=0;i<path.size()-1;i++)
		{
			mapEdgeRoute[path[i]->first][path[i+1]->first].push_back(indexFlow);
		}
	}

	//update rest flow
	if(flowLibRest.size()!=0)
		flowLibRest.clear();
	for(size_t i=0;i<flowLib.size();i++)
	{
		flowLibRest.push_back(flowLib[i]->getRestFlow());
	}

	return 0;
}

int ConsNode::initMapEdgeRoute(size_t _numNode)
{
	if (mapEdgeRoute.size()==0)
	{
		vector<vector<size_t> >tmp;
		tmp.resize(_numNode);
		mapEdgeRoute.resize(_numNode,tmp);
	}
	return 0;
}

int ConsNode::regulate(vector<pair<size_t, size_t> >& overLoadEdge, vector<long int>& overLoad)/////here
{
	cout<<"**************************************\n";

	vector<vector<int> >countUsed;
	vector<vector<int> >countLib;
	vector<int>tmp1;tmp1.resize(flowUsed.size(),0);
	vector<int>tmp2;tmp2.resize(flowLib.size(),0);

	countUsed.resize(overLoadEdge.size(),tmp1);
	countLib.resize(overLoadEdge.size(),tmp2);

	vector<long int>mapLibToUsed;	//map lib to index of use
	mapLibToUsed.resize(flowLib.size(),-1);

	vector<pair<size_t,size_t> >tmpFlowUsed;//tmp flowused
	for(size_t i=0;i<flowUsed.size();i++)
	{
		tmpFlowUsed.push_back(flowUsed[i]);
		mapLibToUsed[flowUsed[i].first]=i;
	}
	vector<long int> tmpRestFlow;//tmp restflow
	for(size_t i=0;i<flowLibRest.size();i++)
	{
		tmpRestFlow.push_back(flowLibRest[i]);
	}


	//show flowUsed
	cout<<"flowUsed: \n";
	for(size_t i=0;i<flowUsed.size();i++)
	{

		cout<<"["<<i<<"] ";
		Flow *tmpflow=flowLib[flowUsed[i].first];
		vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
		for(size_t j=0;j<path.size();j++)
		{
			cout<<path[j]->first<<" ";
		}
		cout<<"("<<flowUsed[i].second<<") <"<<flowLibRest[flowUsed[i].first]<<">"<<endl;
	}
	cout<<endl;
	// cout<<"("<<overLoadEdge[0].first<<" "<<overLoadEdge[0].second<<" "<<overLoad[0]<<")"<<endl;



	
	cout<<"edge over flow before remove: \n";
	for(size_t i=0;i<overLoadEdge.size();i++)
	{
		cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoad[i]<<endl;
	}
	cout<<endl;

	for(size_t i=0;i<overLoadEdge.size();i++)
	{
		vector<size_t>& flowIndex=mapEdgeRoute[overLoadEdge[i].first][overLoadEdge[i].second];
		for(size_t j=0;j<flowIndex.size();j++)
		{
			countLib[i][flowIndex[j]]=1;
		}

		for(size_t j=0;j<flowUsed.size();j++)
		{
			if(searchEdgeInFlow(overLoadEdge[i],flowUsed[j])==0)
			{
				countUsed[i][j]=1;
			}
		}
		
	}
	// cout<<"("<<countUsed[0][3]<<")"<<endl;


	//sort the used edge according to overload
	vector<size_t>usedIndex;
	vector<size_t>usedNum;
	for(size_t i=0;i<flowUsed.size();i++)
	{
		usedIndex.push_back(i);
		size_t sum=0;
		for(size_t j=0;j<overLoadEdge.size();j++)
			sum+=countUsed[j][i];
		usedNum.push_back(sum);
	}
	// cout<<"( ";
	// for(size_t i=0;i<usedNum.size();i++)
	// {
	// 	cout<<usedNum[i]<<" ";
	// }
	// cout<<")"<<endl;

	//sort (large -> small)
	for(size_t i=0;i<usedIndex.size()-1;i++)
	{
		for(size_t j=i+1;j<usedIndex.size();j++)
		{
			if(usedNum[usedIndex[i]]<usedNum[usedIndex[j]])
			{
				size_t tmp=usedIndex[i];
				usedIndex[i]=usedIndex[j];
				usedIndex[j]=tmp;
			}
		}
	}

	//sort the lib edge according to overload
	vector<size_t>libIndex;
	vector<size_t>libNum;
	for(size_t i=0;i<flowLib.size();i++)
	{
		libIndex.push_back(i);
		size_t sum=0;
		for(size_t j=0;j<overLoadEdge.size();j++)
			sum+=countLib[j][i];
		libNum.push_back(sum);
	}
	// cout<<"( ";
	// for(size_t i=0;i<libNum.size();i++)
	// {
	// 	cout<<libNum[i]<<" ";
	// }
	// cout<<")"<<endl;

	//sort (small -> large)
	for(size_t i=0;i<libIndex.size()-1;i++)
	{
		for(size_t j=i+1;j<libIndex.size();j++)
		{
			if(libNum[libIndex[i]]>libNum[libIndex[j]])
			{
				size_t tmp=libIndex[i];
				libIndex[i]=libIndex[j];
				libIndex[j]=tmp;
			}
		}
	}



	//remove the path including overload edge
	vector<long int> overLoadRest;
	for(size_t i=0;i<overLoad.size();i++)
	{
		overLoadRest.push_back(overLoad[i]);
	}

	int overloadOrNot=-1;//flag of overload or not
	long int minusFlow=0;
	for(size_t i=0;i<usedIndex.size();i++)	
	{
		size_t maxFlow=0;
		for(size_t j=0;j<overLoadEdge.size();j++)
		{
			size_t tmpFlow=0;
			if(countUsed[j][usedIndex[i]]==1)
			{
				if(overLoad[j]>=(long int)flowUsed[usedIndex[i]].second)
				{
					tmpFlow=flowUsed[usedIndex[i]].second;
				}
				else
				{
					tmpFlow=overLoad[j];
				}
			}
			if(tmpFlow>maxFlow)
				maxFlow=tmpFlow;
		}

		for(size_t j=0;j<overLoad.size();j++)
		{
			if(countUsed[j][usedIndex[i]]==1)
				overLoadRest[j]-=maxFlow;
		}

		minusFlow+=maxFlow;
		tmpFlowUsed[usedIndex[i]].second-=maxFlow;
		if(tmpFlowUsed[usedIndex[i]].second==0)
			tmpFlowUsed[usedIndex[i]];

		overloadOrNot=-1;
		for(size_t j=0;j<overLoad.size();j++)
		{
			if(overLoadRest[j]>0)
				overloadOrNot=1;
		}

		if(overloadOrNot==-1)
			break;

	}

	cout<<"edge over flow after remove: \n";
	for(size_t i=0;i<overLoadEdge.size();i++)
	{
		cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoadRest[i]<<endl;
	}
	cout<<endl;


	cout<<"overload fixed: ";
	if(overloadOrNot==-1)
		cout<<"Yes";
	else
		cout<<"No";
	cout<<endl;

	cout<<"flow need to compensate: ";
		cout<<minusFlow;
	cout<<endl<<endl;

	//add edge (can't guarantee the added edge is not the overload one, because the demand should be satisfied first)
	for(size_t i=0;i<flowLib.size();i++)
	{
		size_t iSorted=libIndex[i];
		// cout<<iSorted<<"-";
		if(tmpRestFlow[iSorted]>0)
		{

			long int deltaFlow=0;
			if(tmpRestFlow[iSorted]>minusFlow)
			{
				deltaFlow=minusFlow;
			}
			else
			{
				deltaFlow=tmpRestFlow[iSorted];
			}

			if(mapLibToUsed[iSorted]==-1)
			{
				pair<size_t,size_t>tmp(iSorted,deltaFlow);
				tmpFlowUsed.push_back(tmp);
			}
			else
			{
				tmpFlowUsed[mapLibToUsed[iSorted]].second+=deltaFlow;
			}

			// tmpRestFlow[iSorted]-=deltaFlow;
			minusFlow-=deltaFlow;

			//update the rest overload edge
			for(size_t j=0;j<overLoadEdge.size();j++)
			{
				if(countLib[j][iSorted]==1)
					overLoadRest[j]+=deltaFlow;
			}

			if(minusFlow==0)
				break;
			// }
		}
		
	}

	cout<<"edge over flow after add edge: \n";
	for(size_t i=0;i<overLoadEdge.size();i++)
	{
		cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoadRest[i]<<endl;
	}
	cout<<endl;

	//update tmpRestFlow
	for(size_t i=0;i<flowLib.size();i++)
	{
		tmpRestFlow[i]=0;
	}
	for(size_t i=0;i<tmpFlowUsed.size();i++)
	{
		tmpRestFlow[tmpFlowUsed[i].first]=flowLib[tmpFlowUsed[i].first]->getRestFlow()-tmpFlowUsed[i].second;
		// cout<<"#####"<<tmpFlowUsed[i].first<<" "<<flowLib[tmpFlowUsed[i].first]->getRestFlow()<<" "<<tmpFlowUsed[i].second<<"#####";
	}
	
	//remove the edge of zero (after that map of lib to used is change)
	vector<pair<size_t,size_t> >::iterator itr;
	
	for(itr=tmpFlowUsed.begin();itr!=tmpFlowUsed.end();)
	{
		if((*itr).second==0)
			tmpFlowUsed.erase(itr);
		else
			itr++;
	}

	//show tmpFlowUsed
	cout<<"tmpFlowUsed: \n";
	for(size_t i=0;i<tmpFlowUsed.size();i++)
	{

		cout<<"["<<i<<"] ";
		Flow *tmpflow=flowLib[tmpFlowUsed[i].first];
		vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
		for(size_t j=0;j<path.size();j++)
		{
			cout<<path[j]->first<<" ";
		}
		cout<<"("<<tmpFlowUsed[i].second<<") <"<<tmpRestFlow[tmpFlowUsed[i].first]<<">"<<endl;
	}

	//update flow used and rest
	flowLibRest.clear();
	flowUsed.clear();
	for(size_t i=0;i<tmpFlowUsed.size();i++)
	{
		pair<size_t,size_t>tmp(tmpFlowUsed[i]);
		flowUsed.push_back(tmp);
	}
	for(size_t i=0;i<tmpRestFlow.size();i++)
	{
		flowLibRest.push_back(tmpRestFlow[i]);
	}
	for(size_t i=0;i<overLoad.size();i++)
	{
		overLoad[i]=overLoadRest[i];
	}
	 
	//update the overload

	// flowLibRest=tmpRestFlow;



	cout<<"++++++++++++++++++++++++++++++++++++++\n";
	return 0;
}

int ConsNode::searchEdgeInFlow(pair<size_t, size_t> &Edge, pair<size_t,size_t> &flow)
{
	vector<size_t>& flowIndex=mapEdgeRoute[Edge.first][Edge.second];
	for(size_t i=0;i<flowIndex.size();i++)
	{
		if(flow.first==flowIndex[i])
			return 0;
	}

	return -1;
}




int ConsNode::sortFlow()
{
	for (size_t i=0;i<flowLib.size()-1;i++)
	{
		for(size_t j=i+1;j<flowLib.size();j++)
		{
			if(flowLib[i]->getLength()>flowLib[j]->getLength())
			{
				Flow* tmp=flowLib[i];
				flowLib[i]=flowLib[j];
				flowLib[j]=tmp;
			}
		}
	}
	return 0;
}

long int ConsNode::getBandWidthUsed(size_t i, size_t j)
{
	long int sumBandWidth=0;
	for (size_t k=0;k<mapEdgeRoute[i][j].size();k++)
		sumBandWidth+=mapEdgeRoute[i][j][k];

	return sumBandWidth;
}

int ConsNode::selectFlow()
{
	int satisfied=0;
	size_t bandWidth=0;


	for(size_t i=0;i<flowLib.size();i++)
	{
		if(demand-bandWidth>flowLib[i]->getRestFlow())//use out flow on the path
		{
			bandWidth+=flowLib[i]->getRestFlow();
			pair<size_t,size_t>tmp(i,flowLib[i]->getRestFlow());
			flowUsed.push_back(tmp);
			flowLibRest[i]=0;
		}
		else//use part of flow on the pat
		{
			pair<size_t,size_t>tmp(i,demand-bandWidth);
			flowUsed.push_back(tmp);
			flowLibRest[i]=flowLib[i]->getRestFlow()-(demand-bandWidth);
			
			bandWidth=demand;
			
			break;
		}
	}
	// cout<<"flowUsed size: "<<flowUsed.size()<<endl;
	// cout<<"demand "<<demand<<"bandWidth "<<bandWidth<<endl;
	if(bandWidth!=demand)
	{
		flowUsed.clear();
		satisfied=-1;
	}
	// else
	// {
	// 	if(mapEdgeRoute.size()==0)
	// 	{
	// 		cout<<"mapEdgeRoute not initialized"<<endl;
	// 	}
	// 	for(size_t indexFlow=0;indexFlow<flowUsed.size();indexFlow++)
	// 	{
	// 		Flow *tmpflow=flowLib[flowUsed[indexFlow].first];
	// 		vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
	// 		for(size_t i=0;i<path.size()-1;i++)
	// 		{
	// 			mapEdgeRoute[path[i]->first][path[i+1]->first].push_back(flowUsed[indexFlow].first);
	// 		}
	// 	}
	// }

	return satisfied;
}

int ConsNode::popRoute(size_t index, vector<size_t>* _route)
{
	if (index>=flowUsed.size())
		return -1;
	_route->clear();

	Flow *tmpflow=flowLib[flowUsed[index].first];
	vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
	for(size_t i=0;i<path.size();i++)
		_route->push_back(path[i]->first); 
	// _route->push_back(tmpflow->getEnd());//index connect to client
	_route->push_back(indexNode);//index of client
	_route->push_back(flowUsed[index].second);//flow

	// cout<<"flowUsed: ";
	// for(size_t i=0;i<_route->size();i++)
	// {
	// 	cout<<(*_route)[i]<<" ";
	// }
	// cout<<endl;
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
		for (size_t j=0;j<routeG[i]->size()-1;j++)
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

Flow::~Flow()
{
	for (size_t i=0;i<path.size();i++)
	{
		pair<size_t, size_t>* tmp=path.back();
		delete tmp;
		path.pop_back();
	}
}
//**************************************************Class EdgeMatrix**************************************************
EdgeMatrix::EdgeMatrix(size_t _numNode, size_t _numCons):numNode(_numNode),numCons(_numCons)
{
	vector<long int>tmp;
	tmp.resize(_numNode,0);
	matrix.resize(_numNode,tmp);
	matrixRef.resize(_numNode,tmp);

	xCIJ.resize(_numCons,matrixRef);
	// // verify matrix
	// matrix[3][4]=999;

	// for(size_t i=0;i<_numNode;i++)
	// {
	// 	for(size_t j=0;j<_numNode;j++)
	// 	{
	// 		cout<<matrix[i][j]<<" ";
	// 	}
	// 	cout<<endl;

	// }
}

EdgeMatrix::EdgeMatrix(const EdgeMatrix& _edgeMatrix)
{
	numNode=_edgeMatrix.getSize();

	vector<long int>tmp;
	for(size_t i=0;i<numNode;i++)
	{
		tmp.push_back(numNode);

	}

	for(size_t i=0;i<numNode;i++)
	{
		matrix.push_back(tmp);

	}

	for(size_t i=0;i<numNode;i++)
	{
		for(size_t j=0;j<numNode;j++)
		{
			matrix[i][j]=_edgeMatrix.getBandWidth(i,j);
		}
	}
}


EdgeMatrix::~EdgeMatrix()
{

}

int EdgeMatrix::editEdge(size_t i, size_t j, long int deltaBandWidth)
{
	matrix[i][j]+=deltaBandWidth; 
	pair<size_t, size_t> tmp(i,j);
	checkList.push_back(tmp);
	return 0;
}

int EdgeMatrix::loadEdge(size_t i, size_t j, long int deltaBandWidth)
{
	matrixRef[i][j]+=deltaBandWidth; 
	pair<size_t, size_t> tmp(i,j);
	checkList.push_back(tmp);
	return 0;
}

int EdgeMatrix::checkOverLoad(vector<pair<size_t, size_t> >& overLoad, vector<long int>& overLoadVal)
{
	int overLoadFlag=-1;
	if (overLoad.size()>0)
	{
		overLoad.clear();
	}
	if(overLoadVal.size()>0)
	{
		overLoadVal.clear();
	}

	for(size_t i=0;i<checkList.size();i++)
	{
		size_t x=checkList[i].first;
		size_t y=checkList[i].second;
		if(matrix[x][y]<0)
		{
			pair<size_t, size_t> tmp(x,y);
			overLoad.push_back(tmp);
			overLoadVal.push_back(-matrix[x][y]);
			overLoadFlag=0;
		}



	}
	return overLoadFlag;
}

int EdgeMatrix::insertFlow(vector<size_t >& flow, size_t consIndex)
{
	long int deltaBandWidth=flow.back();
	for (size_t i=0;i<flow.size()-3;i++)
	{
		editEdge(flow[i],flow[i+1],-deltaBandWidth);
		xCIJ[consIndex][flow[i]][flow[i+1]]+=deltaBandWidth;
	}
	return 0;
}

int EdgeMatrix::resetMatrix()
{
	for (size_t i=0;i<matrixRef.size();i++)
		for(size_t j=0;j<matrixRef.size();j++)
		{
			matrix[i][j]=matrixRef[i][j];
		}

	for(size_t c=0;c<numCons;c++)
		for(size_t i=0;i<numNode;i++)
			for(size_t j=0;j<numNode;j++)
				xCIJ[c][i][j]=0;
	return 0;
}

int EdgeMatrix::update(ConsNode consNodeGroup[],size_t numCons)
{
	resetMatrix();
	for(size_t i=0;i<numCons;i++)
	{
		for(size_t j=0;j<consNodeGroup[i].getNumFlow();j++)
		{
			vector<size_t>* route=new vector<size_t>();
			consNodeGroup[i].popRoute(j,route);
			insertFlow(*route,i);
			delete route;
		}
	}
		return 0;
}

