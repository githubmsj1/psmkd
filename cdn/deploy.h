#ifndef __ROUTE_H__
#define __ROUTE_H__


#include "lib_io.h"
#include <stdlib.h> 
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <queue>
#include <algorithm>
#include <map>
#include <time.h>

using namespace std;
class NetworkNode;
class ConsNode;
class Flow;
class EdgeMatrix;
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
	// int loadData(NetworkNode networkNodeGroup[], ConsNode consNodeGroup[]);
	int loadData(NetworkNode networkNodeGroup[], ConsNode consNodeGroup[], EdgeMatrix& edgeMatrix);
	int str2Vec(char *str, vector<int>& vec);
	size_t getNumNode(){return numNode;}
	size_t getNumEdge(){return numEdge;}
	size_t getNumCons(){return numCons;}
	size_t getNumLine(){return numLine;}
	size_t getCostServer(){return costServer;}
	size_t getMatrixCost(size_t i, size_t j){return costMatrix[i][j];}
	int sapss(NetworkNode _networkNodeGroup[],vector<size_t>& _start, ConsNode consNodeGroup[]);

private:
	size_t numNode;
	size_t numEdge;
	size_t numCons;
	size_t numLine;
	size_t costServer;
	char** topo;
	vector<vector<size_t> >costMatrix;

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
	int saps(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,vector<size_t>& start, size_t end);
	// int sapss(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,vector<size_t>& start, vector<size_t>& end);
	size_t sapV(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,size_t start, size_t end);
	vector<Flow* >* getFlowLib(){return &flowLib;}
	vector<pair<size_t,size_t> >* getFlowUsed(){return &flowUsed;}
	int initMapEdgeRoute(size_t _numNode);
	int initCons(NetworkInfo networkInfo);
	int selectFlow();
	int regulate(vector<pair<size_t, size_t> >& overLoadEdge, vector<long int>& overLoad);
	int popRoute(size_t index, vector<size_t>* route);
	// int editFlow(long int deltaFlow){totalFlow+=deltaFlow;}
	int sortFlow();
	int editConsIndex(size_t _index){indexNode=_index; return 0;}
	int sapBfs(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,size_t end);
	long int getMaxRestFlow(){return restFlowGraph[sortIndexRestFlow[0]];}
	long int getRestFlow(size_t index){return restFlowGraph[sortIndexRestFlow[index]];}
	size_t getIndexMaxRestFlow(){return sortIndexRestFlow[0];}
	// long int getBandWidthUsed(size_t i, size_t j);
	int searchEdgeInFlow(pair<size_t, size_t> &Edge, pair<size_t,size_t> &flow);
	long int getEdgeLoad(pair<size_t, size_t>);
	int clearRoute();
	size_t getTotalRestFlow(){return totalFlow-demand;};
private:
	size_t toIndexNode;
	size_t demand;
	size_t indexNode;
	size_t numNetworkNode;


	vector<Flow* >flowLib;
	vector<long int>flowLibRest;
	vector<pair<size_t,size_t> >flowUsed;//index of flow, flow
	// vector<vector<vector<size_t> > >mapEdgeRoute;
	vector<long int> restFlowGraph;
	vector<size_t> sortIndexRestFlow;
	map<pair<size_t, size_t>,vector<size_t>>mapEdgeRoute;
	size_t totalFlow;
	// size_t indexMaxRestFlow;
	// long int maxRestFlow;
};



class Route
{
public:
	Route();
	~Route();
	int popRouteAll2Str(string& str);
	int pushRoute(vector<size_t>* route);
	int editCost(size_t _cost);
	size_t getCost(){return cost;}
	size_t costCal(NetworkInfo networkInfo);
private:
	vector<vector<size_t>* >routeG ;
	size_t cost;
	size_t serverNum;

};



class Flow
{
public:
	Flow(size_t _start, size_t _end):start(_start),end(_end){restFlow=0;flowCostUnit=0;}
	~Flow();
	size_t getRestFlow(){return restFlow;}
	size_t getLength(){return path.size();}
	size_t editFlow(int deltaFlow){restFlow+=deltaFlow;return restFlow;}
	int pushPair(size_t nodeIndex, size_t edgeIndex);
	int getStart(){return start;}
	int getEnd(){return end;}
	int editFlowCostUnit(size_t costUnit){flowCostUnit+=costUnit; return 0;}
	int getFlowCostUnit(){return flowCostUnit;}
	vector<pair<size_t,size_t>* >* getPath(){return &path;}
private:
	size_t restFlow;
	size_t start;
	size_t end;
	size_t flowCostUnit;
	vector<pair<size_t,size_t>* > path;
};


class EdgeMatrix
{
public:
	EdgeMatrix(size_t _numNode, size_t _numCons);
	EdgeMatrix(const EdgeMatrix& _edgeMatrix);
	~EdgeMatrix();
	int checkOverLoad(vector<pair<size_t, size_t> >& overLoadEdge, vector<long int>& overLoad);
	int loadEdge(size_t i, size_t j, long int deltaBandWidth);
	int editEdge(size_t i, size_t j, long int deltaBandWidth);
	long int getBandWidth(size_t i, size_t j)const{return matrix[i][j];}
	size_t getSize() const{return numNode;}
	// int insertFlow(vector<size_t>& flow,size_t consIndex);
	int resetMatrix();
	// int update(ConsNode consNodeGroup[],size_t numCons);//memory leak
	int checkCons(ConsNode consNodeGroup[], NetworkInfo networkInfo,vector<pair<size_t, size_t> >& overLoad, vector<long int>& overLoadVal);
	void printMatrix();
private:
	size_t numNode;
	size_t numCons;
	vector<vector<long int> >matrix;
	vector<vector<long int> >matrixRef;
	vector<pair<size_t, size_t> >checkList;
	// vector<vector<vector<long int> > >xCIJ;
};
#endif