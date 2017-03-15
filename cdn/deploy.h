#ifndef __ROUTE_H__
#define __ROUTE_H__


#include "lib_io.h"
#include <stdlib.h> 
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
class NetworkNode;
class ConsNode;
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
	~NetworkEdge();
	size_t getToIndexNode(){return toIndexNode;}
	size_t getBandWidth(){return bandWidth;}
	size_t getCostUnit(){return costUnit;}

private:
	size_t toIndexNode;
	size_t bandWidth;
	size_t costUnit;
	NetworkNode* toPtrNode;
};

class NetworkNode
{
public:
	NetworkNode();
	~NetworkNode();
	int addEdge(size_t _toIndexNode, size_t _bandWidth, size_t _costUnit);
	int connectCons(size_t _toIndexCons){toIndexCons=_toIndexCons; return 0;}
	vector<NetworkEdge*>* getAdjEdge(){return &adjEdge;}
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
private:
	size_t toIndexNode;
	size_t demand;
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

#endif