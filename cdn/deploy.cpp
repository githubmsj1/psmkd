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



	//verify network
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
	//sort client according to demand
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
	
	//show cons idx sorted
	cout<<"demand: ";
	for(size_t i=0;i<consIndex.size();i++)
	{
		cout<<consNodeGroup[consIndex[i]].getDemand()<<" ";
	}
	cout<<endl;

	//collect flow only for the node of 0 and 1
	consNodeGroup[consIndex[0]].sap(networkNodeGroup,networkInfo,consNodeGroup[consIndex[2]].getToIndexNode(),consNodeGroup[consIndex[0]].getToIndexNode());
	cout<<"select: "<<consNodeGroup[consIndex[0]].selectFlow()<<endl;
	// consNodeGroup[consIndex[1]].sap(networkNodeGroup,networkInfo,consNodeGroup[consIndex[1]].getToIndexNode(),consNodeGroup[consIndex[0]].getToIndexNode());
	// cout<<"select: "<<consNodeGroup[consIndex[1]].selectFlow()<<endl;

	EdgeMatrix globalEdgeMatrix(networkInfo.getNumNode());

	// vreify SAP
	cout<<"--------------------"<<endl;
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



	Route routeOutput;
	cout<<"Flow number: "<<consNodeGroup[consIndex[0]].getNumFlow()<<endl;
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
}

ConsNode::~ConsNode()
{}

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

int ConsNode::regulate(vector<pair<size_t, size_t> >& overLoadEdge)
{
	return 0;
}

int ConsNode::sortFlow()
{
	for (size_t i=0;i<flowLib.size()-1;i++)
	{
		for(size_t j=i+1;j<flowLib.size();j++)
		{
			if(flowLib[i]->getRestFlow()>flowLib[j]->getRestFlow())
			{
				Flow* tmp=flowLib[i];
				flowLib[i]=flowLib[j];
				flowLib[j]=tmp;
			}
		}
	}
	return 0;
}

int ConsNode::selectFlow()
{
	int satisfied=0;
	size_t bandWidth=0;

	for(size_t i=0;i<flowLib.size();i++)
	{
		if(demand-bandWidth>flowLib[i]->getRestFlow())
		{
			bandWidth+=flowLib[i]->getRestFlow();
			pair<size_t,size_t>tmp(i,flowLib[i]->getRestFlow());
			flowUsed.push_back(tmp);
		}
		else
		{
			pair<size_t,size_t>tmp(i,demand-bandWidth);
			flowUsed.push_back(tmp);
			bandWidth=demand;
			break;
		}
	}
	// cout<<"flowUsed size: "<<flowUsed.size()<<endl;
	// cout<<"demand "<<demand<<"bandWidth "<<bandWidth<<endl;
	if(bandWidth!=demand)
	{
		satisfied=-1;
	}
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
	_route->push_back(tmpflow->getEnd());//index connect to client
	_route->push_back(indexNode);//index of client
	_route->push_back(flowUsed[index].second);//flow

	cout<<"flowUsed: ";
	for(size_t i=0;i<_route->size();i++)
	{
		cout<<(*_route)[i]<<" ";
	}
	cout<<endl;
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
EdgeMatrix::EdgeMatrix(size_t _numNode):numNode(_numNode)
{
	vector<long int>tmp;
	tmp.resize(_numNode,0);
	matrix.resize(_numNode,tmp);


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

int EdgeMatrix::checkOverLoad(vector<pair<size_t, size_t> >& overLoad)
{
	int overLoadFlag=0;
	if (overLoad.size()>0)
	{
		overLoad.clear();
	}

	for(size_t i=0;i<checkList.size();i++)
	{
		size_t x=checkList[i].first;
		size_t y=checkList[i].second;
		if(matrix[x][y]<0)
		{
			pair<size_t, size_t> tmp(x,y);
			overLoad.push_back(tmp);
			overLoadFlag=-1;
		}
	}
	return overLoadFlag;
}