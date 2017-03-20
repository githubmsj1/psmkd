#ifndef __ROUTE_H__
#define __ROUTE_H__


#include "lib_io.h"
#include <stdlib.h> 
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>

using namespace std;
class NetworkNode;
class ConsNode;
class Flow;
void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

class NetworkInfo
{
public:
	NetworkInfo(size_t _numNode, size_t _numEdge, size_t _numCons): 
	numNode(_numNode), numEdge(_numEdge), numCons(_numCons)
	{}
	NetworkInfo(size_t _line_num, char** _topo):
	numLine(_line_num), topo(_topo)
	{}
	NetworkInfo();
	~NetworkInfo();

	int loadNetInfo();
	int loadData(NetworkNode networkNodeGroup[], ConsNode consNodeGroup[]);
	int str2Vec(char *str, vector<int>& vec);
	size_t getNumNode(){return numNode;}
	size_t getNumEdge(){return numEdge;}
	size_t getNumCons(){return numCons;}
	size_t getNumLine(){return numLine;}
	size_t getCostServer(){return costServer;}
private:
	size_t numNode;
	size_t numEdge;
	size_t numCons;
	size_t numLine;
	size_t costServer;
	char** topo;

};
	
class NetworkEdge
{
public:
	NetworkEdge(size_t _toIndexNode, size_t _bandWidth, size_t _costUnit):
	toIndexNode(_toIndexNode), bandWidth(_bandWidth), costUnit(_costUnit)
	{}
	NetworkEdge();
	NetworkEdge(NetworkEdge& _networkEdge);
	~NetworkEdge();
	size_t getToIndexNode(){return toIndexNode;}
	int getBandWidth(){return bandWidth;}
	size_t getCostUnit(){return costUnit;}
	int editBandWidth(int delta){bandWidth+=delta; return bandWidth;}
	size_t updateAntiEdgeIndex(size_t _antiEdgeIndex){antiEdgeIndex=_antiEdgeIndex; return antiEdgeIndex;}
	size_t getAntiEdgeIndex(){return antiEdgeIndex;}
private:
	size_t toIndexNode;
	int bandWidth;
	size_t costUnit;
	size_t antiEdgeIndex;
	NetworkNode* toPtrNode;
};

class NetworkNode
{
public:
	NetworkNode();
	NetworkNode(const NetworkNode& _networkNode);
	~NetworkNode();
	int addEdge(size_t _toIndexNode, size_t _bandWidth, size_t _costUnit);
	int connectCons(size_t _toIndexCons){toIndexCons=_toIndexCons; return 0;}
	vector<NetworkEdge*>* getAdjEdge(){return &adjEdge;}
	size_t getToIndexNode(size_t edgeIndex) const{return adjEdge[edgeIndex]->getToIndexNode();}
	size_t getBandWidth(size_t edgeIndex) const{return adjEdge[edgeIndex]->getBandWidth();}
	size_t getCostUnit(size_t edgeIndex) const{return adjEdge[edgeIndex]->getCostUnit();}
	int editBandWidth(size_t edgeIndex, int delta){return adjEdge[edgeIndex]->editBandWidth(delta);}
	size_t updateAntiEdgeIndex(size_t edgeIndex, size_t antiEdgeIndex){return adjEdge[edgeIndex]->updateAntiEdgeIndex(antiEdgeIndex);}
	size_t getEdgeSize() const{return adjEdge.size();}
	size_t getAntiEdgeIndex(size_t edgeIndex) const{return adjEdge[edgeIndex]->getAntiEdgeIndex();}
	long int getToIndexCons() const{return toIndexCons;}
	// int mergeDir();
private:
	vector<NetworkEdge*>adjEdge;
	long int toIndexCons;
};


class ConsNode
{
public:
	ConsNode();
	~ConsNode();
	int update(size_t _toIndexNode, size_t _demand)
	{
		 toIndexNode=_toIndexNode;
		 demand=_demand;
		return 0;
	}
	size_t getToIndexNode(){return toIndexNode;}
	size_t getDemand(){return demand;}	
	size_t getNumFlow(){return flowUsed.size();}
	int sap(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,size_t start, size_t end);
	vector<Flow* >* getFlowLib(){return &flowLib;}
	int initMapEdgeRoute(size_t _numNode);
	int selectFlow();
	int regulate(vector<pair<size_t, size_t> >& overLoadEdge);
	int popRoute(size_t index, vector<size_t>* route);
	// int editFlow(long int deltaFlow){totalFlow+=deltaFlow;}
	int sortFlow();
	int editConsIndex(size_t _index){indexNode=_index; return 0;}
private:
	size_t toIndexNode;
	size_t demand;
	size_t indexNode;

	vector<Flow* >flowLib;
	vector<pair<size_t,size_t> >flowUsed;//index of flow, flow
	vector<vector<vector<size_t> > >mapEdgeRoute;
	size_t totalFlow;
};



class Route
{
public:
	Route();
	~Route();
	int popRouteAll2Str(string& str);
	int pushRoute(vector<size_t>* route);

private:
	vector<vector<size_t>* >routeG ;

};



class Flow
{
public:
	Flow(size_t _start, size_t _end):start(_start),end(_end){restFlow=0;}
	~Flow();
	size_t getRestFlow(){return restFlow;}
	size_t editFlow(int deltaFlow){restFlow+=deltaFlow;return restFlow;}
	int pushPair(size_t nodeIndex, size_t edgeIndex);
	int getStart(){return start;}
	int getEnd(){return end;}
	vector<pair<size_t,size_t>* >* getPath(){return &path;}
private:
	size_t restFlow;
	size_t start;
	size_t end;
	vector<pair<size_t,size_t>* > path;
};


class EdgeMatrix
{
public:
	EdgeMatrix(size_t _numNode);
	EdgeMatrix(const EdgeMatrix& _edgeMatrix);
	~EdgeMatrix();
	int checkOverLoad(vector<pair<size_t, size_t> >& overLoad);
	int editEdge(size_t i, size_t j, long int deltaBandWidth);
	long int getBandWidth(size_t i, size_t j)const{return matrix[i][j];}
	size_t getSize() const{return numNode;}
private:
	size_t numNode;
	vector<vector<long int> >matrix;
	vector<pair<size_t, size_t> >checkList;
};
#endif