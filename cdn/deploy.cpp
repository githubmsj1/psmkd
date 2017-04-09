#include "deploy.h"
#include <stdio.h>
#include <iostream>


// #define NUM_SERVER 30
using namespace std;


//You need to complete the function 
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	clock_t startTime = clock();
	clock_t startTimeTotal=clock();
	// for(size_t i=0;i<MAX_EDGE_NUM;i++)
	// {
	// 	cout<<topo[i];
	// }

	//load network data
	NetworkInfo networkInfo(line_num,topo);
	networkInfo.loadNetInfo();

	//network node and client node 
	NetworkNode networkNodeGroup[networkInfo.getNumNode()];
	ConsNode consNodeGroup[networkInfo.getNumCons()];

	//solution group
	vector<Route*> routeSolution;

	//record the bandwidth limit
	EdgeMatrix globalEdgeMatrix(networkInfo.getNumNode(),networkInfo.getNumNode());

	//load edge and node data
	networkInfo.loadData(networkNodeGroup, consNodeGroup, globalEdgeMatrix);


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

	cout<<"**************************************\n";
	globalEdgeMatrix.resetMatrix();
	// globalEdgeMatrix.printMatrix();
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

	clock_t endTime = clock();
	cout<<"Time Cost[read data]-------------->"<<(double)(endTime - startTime)/CLOCKS_PER_SEC<<endl;
	startTime=endTime;

	vector<size_t>consIndex;
	for(size_t i=0;i<networkInfo.getNumCons();i++)
		consIndex.push_back(i);



	// //bfs sap map
	// for(size_t i=0;i<networkInfo.getNumCons();i++)
	// {
	// 	consNodeGroup[i].sapBfs(networkNodeGroup,networkInfo,consNodeGroup[i].getToIndexNode());
	// }

	//construct serverPool
	networkInfo.constructServerPool(networkNodeGroup,consNodeGroup);
	networkInfo.initServerPool(consNodeGroup);

	// endTime = clock();
	// cout<<"Time Cost[BFS sap map]-------------->"<<(double)(endTime - startTime)/CLOCKS_PER_SEC<<endl;
	// startTime=endTime;

//-----------------------------------------------------------------------------------------------
	// //identify the number of server
	// size_t NUM_SERVER=(size_t)((double)networkInfo.getNumCons()*0.07);
	// cout<<"NUM_SERVER: "<<NUM_SERVER<<endl;

	// vector<size_t>oldServer;
	// for(;NUM_SERVER<networkInfo.getNumCons();NUM_SERVER++)
	// {
		// sort client according to demand
	// for(size_t i=0;i<networkInfo.getNumCons()-1;i++)
	// {
	// 	for(size_t j=i+1;j<networkInfo.getNumCons();j++)
	// 	{
	// 		if (consNodeGroup[consIndex[i]].getDemand()<consNodeGroup[consIndex[j]].getDemand())
	// 		{
	// 			size_t tmp=consIndex[i];
	// 			consIndex[i]=consIndex[j];
	// 			consIndex[j]=tmp;
	// 		}


	// 	}
	// }

	// cout<<"index:"<<endl;
	// vector<size_t>serverPosSorted;
	// for(size_t i=0;i<networkInfo.getNumCons();i++)
	// {
	// 	serverPosSorted.push_back(consNodeGroup[consIndex[i]].getToIndexNode());
	// 	cout<<consNodeGroup[consIndex[i]].getToIndexNode()<<":"<<consNodeGroup[consIndex[i]].getDemand()<<" ";
	// }


	//initialize inherit
	networkInfo.initInherit(1);

	vector<size_t>serverPos;
	size_t lastCost=9999999;
	size_t indexInherit=0;
	while(1)
	{
		//algorithm of serverPool
		// networkInfo.deployServer(consNodeGroup,networkNodeGroup,serverPos,lastCost);
		
		if(indexInherit==networkInfo.getNumInherit())
			networkInfo.deployServerInheritGroupCross(serverPos);
		else
			networkInfo.deployServerInherit(consNodeGroup,networkNodeGroup,serverPos,indexInherit);



		// for(size_t i=0;i<serverPos.size();i++)
		// {
		// 	cout<<serverPos[i]<<" ";
		// }
		// cout<<endl;

		// cout<<serverPos.size()<<"haha"<<networkInfo.getNumCons()<<endl;;

		while(1)
		{
			Route* routeOutput=NULL;
			// long int indexConsOverLoad=-1;
			set<long int>indexConsOverLoad;
			if(networkInfo.solveSpfa(networkNodeGroup,serverPos,consNodeGroup,routeOutput,indexConsOverLoad)==0)
			// if(networkInfo.solve(networkNodeGroup,consNodeGroup,serverPos,routeOutput,globalEdgeMatrix,indexConsOverLoad)==0)
			{
				routeSolution.push_back(routeOutput);
				cout<<"**************************************\n";
				cout<<"Solution cost: "<<routeSolution.back()->getCost()<<endl;
				cout<<"Server used: "<<routeSolution.back()->getServerNum()<<endl;
				// cout<<"Server flow";
				// cout<<"Server Choose: ";

				// for(size_t i=0;i<serverPos.size();i++)
				// {
				// 	cout<<serverPos[i]<<" ";
				// }
				// cout<<endl;
				// cout<<serverPos.size()<<"haha"<<networkInfo.getNumCons()<<endl;
				cout<<"++++++++++++++++++++++++++++++++++++++\n";

				lastCost=routeOutput->getCost();
				routeOutput->serverUsedCal(serverPos);
				// cout<<serverPos.size()<<endl;
				// serverPos.pop_back();
				// cout<<serverPos.size()<<endl;
				// string pp;
				// cin>>pp;
				break;
			}
			else
			{
				set<long int>::iterator iter0=indexConsOverLoad.begin();

				cout<<"No solution"<<endl;
				cout<<"indexServerOverLoad "<<consNodeGroup[*iter0].getToIndexNode()<<endl;
				cout<<"indexServerOverLoad: "<<endl;

				
				for(;iter0!=indexConsOverLoad.end();iter0++)
					cout<<consNodeGroup[*iter0].getToIndexNode()<<" ";
				cout<<endl;

				iter0=indexConsOverLoad.begin();
				// break;
				networkInfo.reDeploy(indexConsOverLoad,serverPos,consNodeGroup);
				// vector<size_t>::iterator itr1=find(serverPos.begin(),serverPos.end(),consNodeGroup[*iter0].getToIndexNode());
				// if(itr1==serverPos.end())
				// {
				// 	// cout<<"check:"<<serverPos.size()<<" "<<(indexSwap-NUM_SERVER)%serverPos.size()<<endl;
	
				// 	serverPos.push_back(consNodeGroup[*iter0].getToIndexNode());
					
				// }
			}	
			
			endTime = clock();
			double totalTime=(double)(endTime - startTimeTotal)/CLOCKS_PER_SEC;
			startTime=endTime;
			if(totalTime>85)
				break;
		}

		if(indexInherit==networkInfo.getNumInherit())
			networkInfo.saveServerInheritGroupCross(serverPos,lastCost);
		else
			networkInfo.saveServerInherit(serverPos,lastCost,indexInherit);

		indexInherit++;
		indexInherit=indexInherit%(networkInfo.getNumInherit()+1);

		endTime = clock();
		double totalTime=(double)(endTime - startTimeTotal)/CLOCKS_PER_SEC;
		startTime=endTime;
		if(totalTime>85)
			break;
				
		
	}





		// //show cons idx sorted by demand
		// cout<<"**************************************\n";
		// cout<<"sort cons node by demand: \n";
		// for(size_t i=0;i<consIndex.size();i++)
		// {
		// 	cout<<consIndex[i]<<"<"<<consNodeGroup[consIndex[i]].getDemand()<<">("<<consNodeGroup[consIndex[i]].getToIndexNode()<<") ";
		// }
		// cout<<endl;
		// cout<<"++++++++++++++++++++++++++++++++++++++\n";


		




		// ///test whether the node of low demand is available get from other server; 
		// for(size_t k=0;k<NUM_SERVER;k++)
		// {
		// 	for(size_t i=k;i<networkInfo.getNumCons();i++)
		// 	{
		// 		if(consNodeGroup[consIndex[i]].getRestFlow(5)>(long int)consNodeGroup[consIndex[i]].getDemand())
		// 		{
		// 			size_t tmp=consIndex[0];
		// 			consIndex[0]=consIndex[i];
		// 			consIndex[i]=tmp;
		// 			break;
		// 		}
		// 	}
		// }



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

		// //take out NUM_SERVER server (deploy server)
		// vector<size_t>serverPos;

		// if(oldServer.size()!=0)
		// {
		// 	for(size_t i=NUM_SERVER;i<consIndex.size();i++)
		// 	{
		// 		serverPos.push_back(oldServer[i-NUM_SERVER]);
		// 	}
		// 	cout<<"high efficient"<<endl;
		// }
		// else
		// {
		// 	for(size_t i=NUM_SERVER;i<consIndex.size();i++)
		// 	{
		// 		serverPos.push_back(consNodeGroup[consIndex[i]].getToIndexNode());
		// 	}
		// }


		// for(size_t i=0;i<serverPos.size();i++)
		// {
		// 	cout<<serverPos[i]<<" ";
		// }
		// cout<<endl;

		//test whether the node of low demand is available get from other server; 
			
		// for(size_t k=0;k<NUM_SERVER;k++)
		// {	
		// 	size_t indexK=consIndex[k];
		// 	size_t maxFlow=consNodeGroup[indexK].sapsV(networkNodeGroup,networkInfo,serverPos,consNodeGroup[indexK].getToIndexNode());
		// 	if(maxFlow<consNodeGroup[indexK].getDemand())
		// 	{
		// 		size_t tmp=consIndex[k];
		// 		consIndex[k]=consIndex[indexSwap];
		// 		consIndex[indexSwap]=tmp;

		// 		serverPos[indexSwap-NUM_SERVER]=consNodeGroup[consIndex[indexSwap]].getToIndexNode();
		// 		indexSwap++;
		// 	}

		// }
		


		// //solve the case
		// int state=0;
		// int quit=-1;
		// size_t indexSwap=NUM_SERVER;
		// cout<<"INFO:  "<<NUM_SERVER<<" "<<networkInfo.getNumCons()<<endl;
		// while(1)
		// {
				
			
		// 	Route* routeOutput=NULL;
		// 	long int indexConsOverLoad=-1;
		// 	if(networkInfo.solve(networkNodeGroup,consNodeGroup,serverPos,routeOutput,globalEdgeMatrix,indexConsOverLoad)==0)
		// 	{

		// 		routeSolution.push_back(routeOutput);
		// 		state=0;
		// 	}
		// 	else
		// 	{
				
		// 		// cout<<networkInfo.getNumCons()<<" "<<NUM_SERVER<<endl;
		// 		vector<size_t>::iterator itr1=find(serverPos.begin(),serverPos.end(),consNodeGroup[indexConsOverLoad].getToIndexNode());
		// 		if(itr1==serverPos.end())
		// 		{
		// 			// cout<<"check:"<<serverPos.size()<<" "<<(indexSwap-NUM_SERVER)%serverPos.size()<<endl;
		// 			size_t indexSever=(indexSwap-NUM_SERVER)%serverPos.size();
		
		// 			serverPos[indexSever]=consNodeGroup[indexConsOverLoad].getToIndexNode();
		// 			indexSwap++;
		// 			state=1;

		// 			cout<<"indexServerOverLoad "<<consNodeGroup[indexConsOverLoad].getToIndexNode()<<endl;
		// 		}
		// 		else
		// 		{
		// 			cout<<"unknow error"<<endl;
		// 			// quit=0;
		// 			 break;
		// 		}

		// 		// int pause;
		// 		// cin>>pause;	
				
		// 	}	

		// 	// int pause;
		// 	// cin>>pause;	
		// 	// cout<<"**************************************\n";
		// 	// cout<<"Solution cost: "<<routeSolution.back()->getCost()<<endl;
		// 	// cout<<"Server used: "<<routeSolution.back()->getServerNum()<<endl;
		// 	// cout<<"++++++++++++++++++++++++++++++++++++++\n";

		// 	if(state==0)
		// 		break;

		// 	endTime = clock();
		// 	double totalTime=(double)(endTime - startTimeTotal)/CLOCKS_PER_SEC;
		// 	startTime=endTime;
		// 	if(totalTime>70)
		// 		break;
				
		// }


		// endTime = clock();
		// double totalTime=(double)(endTime - startTimeTotal)/CLOCKS_PER_SEC;
		// startTime=endTime;
		// if(totalTime>70)
		// 	quit=0;

		// if(quit==0)break;

		
		// oldServer.clear();
		// oldServer=serverPos;

//old solution

		// //collect flow only for the node of front NUM_SERVER
		// // consNodeGroup[consIndex[0]].sap(networkNodeGroup,networkInfo,consNodeGroup[consIndex[1]].getToIndexNode(),consNodeGroup[consIndex[0]].getToIndexNode());
		// for(size_t k=0;k<networkInfo.getNumCons();k++)
		// {
		// 	consNodeGroup[consIndex[k]].saps(networkNodeGroup,networkInfo,serverPos,consNodeGroup[consIndex[k]].getToIndexNode());
		// 	cout<<"select: "<<consNodeGroup[consIndex[k]].selectFlow()<<endl;
		// }

		// // endTime = clock();
		// // cout<<"Time Cost[Search flow path by sap]-------------->"<<(double)(endTime - startTime)/CLOCKS_PER_SEC<<endl;
		// // startTime=endTime;

		// //check overload
		// // vector<pair<size_t, size_t> > overLoadEdge;
		// // vector<long int> overLoad;
		// // cout<<"**************************************\n";
		// // if(globalEdgeMatrix.checkCons(consNodeGroup,networkInfo,overLoadEdge,overLoad)!=0)//don't need to reset;
		// // {
		// // 	cout<<"edge overload!: "<<endl;
		// // 	for(size_t i=0;i<overLoadEdge.size();i++)
		// // 	{
		// // 		cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoad[i]<<endl;
		// // 	} 	
		// // }
		// // else
		// // {
		// // 	cout<<"edge bandWidth satisfied "<<endl;
		// // }
		// // cout<<"++++++++++++++++++++++++++++++++++++++\n";


		// //test regulate
		// int bandWidthNormal=-1;
		// cout<<"**************************************\n";
		// for(size_t i=0;i<10;i++)
		// {
		// 	vector<pair<size_t, size_t> > overLoadEdge;
		// 	vector<long int> overLoad;
		// 	if(globalEdgeMatrix.checkCons(consNodeGroup,networkInfo,overLoadEdge,overLoad)==0)
		// 	{
		// 		cout<<"No flow overload."<<endl;
		// 		bandWidthNormal=0;
		// 		break;
		// 	}
		// 	else
		// 	{
		// 		for(size_t j=0;j<networkInfo.getNumCons();j++)
		// 		{
		// 				if(consNodeGroup[j].regulate(overLoadEdge,overLoad)==0)
		// 				{	
		// 					cout<<"Regulation Done."<<endl;
		// 					break;
		// 				}
		// 		}
		// 	}
		// 	cout<<"Regulate: "<<i<<endl;
		// 	// cout<<"after regulation\n";
		// 	// for(size_t j=0;j<overLoadEdge.size();j++)
		// 	// {
		// 	// 	cout<<"("<<overLoadEdge[j].first<<","<<overLoadEdge[j].second<<"): "<<overLoad[j]<<endl;
		// 	// }
		// }
		// cout<<"++++++++++++++++++++++++++++++++++++++\n";

		// // endTime = clock();
		// // cout<<"Time Cost[Regulate over load edge]-------------->"<<(double)(endTime - startTime)/CLOCKS_PER_SEC<<endl;
		// // startTime=endTime;

		// // cout<<"**************************************\n";
		// // cout<<"overload edge used for client"<<endl;
		// // cout<<"ConsNode["<<consIndex[4]<<"]->"; 
		// // cout<<"("<<overLoadEdge[0].first<<","<<overLoadEdge[0].second<<"):"<<consNodeGroup[consIndex[4]].getEdgeLoad(overLoadEdge[0])<<endl;
		// // cout<<"++++++++++++++++++++++++++++++++++++++\n";

		


		// // vreify SAP
		// // cout<<"**************************************\n";
		// // vector<Flow* >& flowLib=*(consNodeGroup[consIndex[0]].getFlowLib());
		// // for (size_t i=0;i<flowLib.size();i++)
		// // {
		// // 	vector<pair<size_t,size_t>* > *path=flowLib[i]->getPath();

		// // 	cout<<i+1<<"("<<flowLib[i]->getRestFlow()<<"):";
		// // 	for(size_t j=0;j<path->size();j++)
		// // 	{
		// // 		pair<size_t,size_t>* tmpPair=(*path)[j];
		// // 		cout<<tmpPair->first<<"<"<<tmpPair->second<<"> ";
		// // 	}
		// // 	// cout<<flowLib[i]->getEnd()<<endl;
		// // 	cout<<" {"<<flowLib[i]->getFlowCostUnit()<<"}";
		// // 	cout<<endl;
		// // }
		// // cout<<"++++++++++++++++++++++++++++++++++++++"<<endl;
		

		// //test regulate	
		// // vector<pair<size_t, size_t> >overLoadEdge;
		// // vector<long int> overLoad;
		// // overLoadEdge.push_back(pair<size_t,size_t>(22,23));
		// // overLoadEdge.push_back(pair<size_t,size_t>(22,24));
		// // overLoadEdge.push_back(pair<size_t,size_t>(24,20));
		// // // overLoadEdge.push_back(pair<size_t,size_t>(31,3));
		// // overLoad.push_back(5);
		// // overLoad.push_back(2);
		// // overLoad.push_back(1);
		// // // overLoad.push_back(1);
		// // consNodeGroup[consIndex[5]].regulate(overLoadEdge,overLoad);
		// // overLoadEdge.clear();
		// // overLoad.clear();
		// // overLoadEdge.push_back(pair<size_t,size_t>(22,21));
		// // overLoad.push_back(3);
		// // consNodeGroup[consIndex[5]].regulate(overLoadEdge,overLoad);
		

		// //SAP
		// /*
		// size_t start=38;
		// size_t end=16;
		// size_t neck;
		// size_t u;
		// size_t h[networkInfo.getNumNode()];
		// size_t cur_flow,flow_ans=0,tmp;

		// int numh[networkInfo.getNumNode()],curEdges[networkInfo.getNumNode()],pre[networkInfo.getNumNode()];
		// for (size_t i=0;i<networkInfo.getNumNode();i++)
		// {
		// 	numh[i]=0;
		// 	h[i]=0;
		// 	pre[i]=-1;
		// 	curEdges[i]=0;
		// }

		// numh[0]=networkInfo.getNumNode();
		// u=start;

		// cout<<"augment route: "<<endl;
		// while(h[start]<networkInfo.getNumNode())
		// {
		// 	if(u==end)
		// 	{
		// 		cur_flow=9999;


		// 		static size_t routeIndex=0;
		// 		routeIndex++;
		// 		cout<<routeIndex<<"(";
		// 		for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
		// 		{
		// 			if(cur_flow>networkNodeGroup[i].getBandWidth(curEdges[i]))
		// 			{
		// 				neck=i;
		// 				cur_flow=networkNodeGroup[i].getBandWidth(curEdges[i]);
		// 			}



		// 		}

		// 		cout<<cur_flow<<"): ";

		// 		for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
		// 		{
		// 			networkNodeGroup[i].editBandWidth(curEdges[i],-cur_flow);
		// 			size_t antiNode=networkNodeGroup[i].getToIndexNode(curEdges[i]);
		// 			size_t antiEdge=networkNodeGroup[i].getAntiEdgeIndex(curEdges[i]);
		// 			networkNodeGroup[antiNode].editBandWidth(antiEdge,cur_flow);
					
		// 			//verify the route
		// 			cout<<i<<"<"<<curEdges[i]<<">"<<" ";
		// 		}

		// 		cout<<end<<endl;

		// 		flow_ans+=cur_flow;
		// 		u=neck;
		// 	}
		// 	// vector<NetworkEdge*>* adj=networkNodeGroup[u].getAdjEdge();

		// 	// vector<NetworkEdge*>::iterator itr;
		// 	// for (vector<NetworkEdge*>::iterator itr=adj->begin()+curEdges[i];itr!=adj->end();itr++)
		// 	// {
		// 	// 	if((*itr)->getBandWidth()&&h[u]==h[(*itr)->getToIndexNode()]+1)
		// 	// 	{
		// 	// 		break;
		// 	// 	}
		// 	// }

		// 	size_t i;
		// 	for (i=curEdges[u];i<networkNodeGroup[u].getEdgeSize();i++)
		// 	{
		// 		if(networkNodeGroup[u].getBandWidth(i)&&h[u]==h[networkNodeGroup[u].getToIndexNode(i)]+1)
		// 		{
		// 			break;
		// 		}
		// 	}

		// 	if(i!=networkNodeGroup[u].getEdgeSize())
		// 	{
		// 		curEdges[u]=i;
		// 		pre[networkNodeGroup[u].getToIndexNode(i)]=u;
		// 		u=networkNodeGroup[u].getToIndexNode(i);
		// 	}
		// 	else
		// 	{
		// 		if(0==--numh[h[u]])break;
		// 		curEdges[u]=0;

		// 		for(tmp=networkInfo.getNumNode(),i=0;i<networkNodeGroup[u].getEdgeSize();i++)
		// 		{
		// 			if(networkNodeGroup[u].getBandWidth(i))
		// 				tmp=(tmp<h[networkNodeGroup[u].getToIndexNode(i)]?tmp:h[networkNodeGroup[u].getToIndexNode(i)]);
		// 		}
		// 		h[u]=tmp+1;
		// 		++numh[h[u]];
		// 		if(u!=start)
		// 			u=pre[u];
		// 	}

		// }*/


		// if(bandWidthNormal==0)
		// {

		// 	Route* routeOutput=new Route();

		// 	//output the route for those apply flow from other servers
		// 	for(size_t k=0;k<networkInfo.getNumCons();k++)
		// 	{
		// 		for(size_t i=0;i<consNodeGroup[consIndex[k]].getNumFlow();i++)
		// 		{
		// 			vector<size_t>* route=new vector<size_t>();
		// 			consNodeGroup[consIndex[k]].popRoute(i,route);
		// 			routeOutput->pushRoute(route);
		// 		}

		// 	}




		// 	// //the rest server direct connect to consume n-2
		// 	// for (size_t i=NUM_SERVER;i<networkInfo.getNumCons();i++)
		// 	// {
		// 	// 	vector<size_t>* route=new vector<size_t>();
				
		// 	// 	route->push_back(consNodeGroup[consIndex[i]].getToIndexNode());
		// 	// 	route->push_back(consIndex[i]);
		// 	// 	route->push_back(consNodeGroup[consIndex[i]].getDemand());

		// 	// 	routeOutput->pushRoute(route);
		// 	// }	

		// 	//calculate cost
		// 	cout<<"**************************************\n";
		// 	cout<<"bandwidth is normal."<<endl;
		// 	cout<<"total Cost: "<<routeOutput->costCal(networkInfo)<<endl;	
		// 	cout<<"++++++++++++++++++++++++++++++++++++++\n";

		// 	routeSolution.push_back(routeOutput);
			
		// }
		// else
		// {	
		// 	cout<<"**************************************\n";
		// 	cout<<"bandwidth is overused!"<<endl;
		// 	cout<<"++++++++++++++++++++++++++++++++++++++\n";
		// 	break;
		// }

		// //clean sap route associated with server position and number
		// for(size_t i=0;i<networkInfo.getNumCons();i++)
		// {
		// 	consNodeGroup[i].clearRoute();
		// }
//old solution 

	// }
//-----------------------------------------------------------------------------------------------
	endTime = clock();
	cout<<"Time Cost[Find solution]-------------->"<<(double)(endTime - startTime)/CLOCKS_PER_SEC<<endl;
	startTime=endTime;


	endTime = clock();
	cout<<"Time Cost[Total run time]-------------->"<<(double)(endTime - startTimeTotal)/CLOCKS_PER_SEC<<endl;
	startTime=endTime;



	// //if no solution 
	// if(routeSolution.size()==0)
	// {
	// 	Route* routeOutput=new Route();
	// 	//the rest server direct connect to consume n-2
	// 	for (size_t i=0;i<networkInfo.getNumCons();i++)
	// 	{
	// 		vector<size_t>* route=new vector<size_t>();
			
	// 		route->push_back(consNodeGroup[consIndex[i]].getToIndexNode());
	// 		route->push_back(consIndex[i]);
	// 		route->push_back(consNodeGroup[consIndex[i]].getDemand());

	// 		routeOutput->pushRoute(route);
	// 	}	
	// 	routeSolution.push_back(routeOutput);
	// }

	cout<<"**************************************\n";
	cout<<"Total solution is "<<routeSolution.size()<<endl;
	cout<<"Cost are ";
	for(size_t i=0;i<routeSolution.size();i++)
	{
		cout<<routeSolution[i]->getCost()<<" ";
	}
	cout<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++\n";

	//take the route of lowest cost
	size_t lowestCost=routeSolution[0]->getCost();
	size_t IndexLowestCost=0;
	for(size_t i=1;i<routeSolution.size();i++)
	{
		if(routeSolution[i]->getCost()<lowestCost)
		{
			lowestCost=routeSolution[i]->getCost();
			IndexLowestCost=i;
		}
	}


	string strTopo;
	// routeOutput.popRouteAll2Str(strTopo);
	cout<<"Final solution cost: "<<routeSolution[IndexLowestCost]->getCost()<<endl;
	cout<<"Server used: "<<routeSolution[IndexLowestCost]->getServerNum()<<endl;
	routeSolution[IndexLowestCost]->popRouteAll2Str(strTopo);
	write_result(strTopo.c_str(), filename);

	//delete solution
	while(routeSolution.size())
	{
		Route *tmp=routeSolution.back();
		routeSolution.pop_back();
		delete tmp;
	}	

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

	vector<size_t>tmpV;
	tmpV.resize(numNode,0);//default is only used to initialize the added memory
	costMatrix.resize(numNode,tmpV);
	return 0;
}

// int NetworkInfo::loadData(NetworkNode networkNodeGroup[], ConsNode consNodeGroup[])
// {
// 	vector<int> tmp;	

// 	//read network node info 0:headNode 1:toNode 2:bandWidth 3:costUnit
// 	size_t lineHead=4;
// 	size_t lineEnd=lineHead+numEdge-1;
// 	for(size_t line=lineHead;line<=lineEnd;line++)
// 	{
// 		str2Vec(topo[line],tmp);
// 		networkNodeGroup[tmp[0]].addEdge(tmp[1], tmp[2], tmp[3]);
// 		networkNodeGroup[tmp[1]].addEdge(tmp[0], tmp[2], tmp[3]);
		
// 		size_t lastIdx0=networkNodeGroup[tmp[0]].getEdgeSize()-1;
// 		size_t lastIdx1=networkNodeGroup[tmp[1]].getEdgeSize()-1;

// 		networkNodeGroup[tmp[0]].updateAntiEdgeIndex(lastIdx0,lastIdx1);
// 		networkNodeGroup[tmp[1]].updateAntiEdgeIndex(lastIdx1,lastIdx0);


// 		// cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<" "<<tmp[3]<<endl;
// 	}



// 	//read consumer node info 0:consNode 1:toNode 2:demand
// 	lineHead=lineEnd+2;
// 	lineEnd=lineHead+numCons-1;
// 	for(size_t line=lineHead;line<=lineEnd;line++)
// 	{
// 		str2Vec(topo[line],tmp);
// 		consNodeGroup[tmp[0]].update(tmp[1],tmp[2]);
// 		networkNodeGroup[tmp[1]].connectCons(tmp[0]);
// 		// cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<endl;
// 		consNodeGroup[tmp[0]].editConsIndex(tmp[0]);
// 	}

// 	return 0;
// }

int NetworkInfo::loadData(NetworkNode networkNodeGroup[], ConsNode consNodeGroup[], EdgeMatrix& edgeMatrix)
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

		edgeMatrix.loadEdge(tmp[0],tmp[1],tmp[2]);//load edge to matrix
		edgeMatrix.loadEdge(tmp[1],tmp[0],tmp[2]);
		// cout<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<" "<<tmp[3]<<endl;

		costMatrix[tmp[0]][tmp[1]]=tmp[3];//load edge cost
		costMatrix[tmp[1]][tmp[0]]=tmp[3];
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

		directLink.push_back(tmp[1]);
	}

	return 0;
}

int NetworkInfo::sapss(NetworkNode _networkNodeGroup[],vector<size_t>& _start, ConsNode consNodeGroup[])
{
	//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

	vector<size_t>_end;
	for(size_t i=0;i<numCons;i++)
	{
		_end.push_back(consNodeGroup[i].getToIndexNode());
	}

	//construct a total start, connecting to all start.
	NetworkNode tmpNode;
	size_t startIndex=getNumNode();
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
		networkNodeGroup[_start[i]].addEdge(startIndex,9999,0);

		size_t lastIdx0=tmpNode.getEdgeSize()-1;
		size_t lastIdx1=networkNodeGroup[_start[i]].getEdgeSize()-1;

		tmpNode.updateAntiEdgeIndex(lastIdx0,lastIdx1);
		networkNodeGroup[_start[i]].updateAntiEdgeIndex(lastIdx1,lastIdx0);
	}
	
	//construct a total end, connecting to all client. 
	NetworkNode tmpNode1;
	size_t endIndex=getNumNode()+1;
	cout<<"end: ";
	for(size_t i=0;i<_end.size();i++)
	{
		cout<<_end[i]<<" ";
	}
	cout<<endl;
	for(size_t i=0;i<_end.size();i++)
	{	
		//ToIndexNode BandWidth CostUnit
		tmpNode1.addEdge(_end[i],9999,0);
		networkNodeGroup[_end[i]].addEdge(endIndex,9999,0);

		size_t lastIdx0=tmpNode1.getEdgeSize()-1;
		size_t lastIdx1=networkNodeGroup[_end[i]].getEdgeSize()-1;

		tmpNode1.updateAntiEdgeIndex(lastIdx0,lastIdx1);
		networkNodeGroup[_end[i]].updateAntiEdgeIndex(lastIdx1,lastIdx0);
	}


	networkNodeGroup.push_back(tmpNode);//push total start
	networkNodeGroup.push_back(tmpNode1);//push total end

	size_t start=startIndex;
	size_t end=endIndex;

	size_t numNode=getNumNode()+2;

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

			// Flow *tmp=new Flow(start,end);

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
			// tmp->editFlow(cur_flow);

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

					// tmp->pushPair(i,curEdges[i]);
					// tmp->editFlowCostUnit(networkNodeGroup[i].getCostUnit(curEdges[i]));
				}

			}

			// cout<<end<<"<"<<0<<">"<<" ";
			// tmp->pushPair(end,0);
			cout<<endl;

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
	// sortFlow();

	//update map from Edge to path index
	// if(mapEdgeRoute.size()==0)
	// {
	// 	cout<<"mapEdgeRoute not initialized"<<endl;
	// }
	// for(size_t indexFlow=0;indexFlow<flowLib.size();indexFlow++)
	// {
	// 	Flow *tmpflow=flowLib[indexFlow];
	// 	vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
	// 	for(size_t i=0;i<path.size()-1;i++)
	// 	{
	// 		mapEdgeRoute[make_pair(path[i]->first,path[i+1]->first)].push_back(indexFlow);
	// 		// mapEdgeRoute[path[i]->first][path[i+1]->first].push_back(indexFlow);
	// 	}
	// }

	//update rest flow
	// if(flowLibRest.size()!=0)
	// 	flowLibRest.clear();
	// for(size_t i=0;i<flowLib.size();i++)
	// {
	// 	flowLibRest.push_back(flowLib[i]->getRestFlow());
	// }

	return 0;
}
int NetworkInfo::solveSpfa(NetworkNode _networkNodeGroup[],vector<size_t>& _start, ConsNode consNodeGroup[],Route* &routeOutput,set<long int> &indexConsOverLoad)
{
	//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<(*this).getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

	//construct end
	vector<size_t>_end;
	for(size_t i=0;i<numCons;i++)
	{
		_end.push_back(consNodeGroup[i].getToIndexNode());
	}

	//clear total flow
	vector<long int>demandRest;

	//load demand 
	for(size_t i=0;i<numCons;i++)
	{
		demandRest.push_back(consNodeGroup[i].getDemand());
	}
		
	size_t start=getNumNode();
	size_t end=getNumNode()+1;
	size_t endIndex=end;

	//construct a total start, connecting to all start.
	NetworkNode tmpNode;
	// cout<<"start: ";
	// for(size_t i=0;i<_start.size();i++)
	// {
	// 	cout<<_start[i]<<" ";
	// }
	cout<<endl;
	for(size_t i=0;i<_start.size();i++)
	{	
		//ToIndexNode BandWidth CostUnit
		tmpNode.addEdge(_start[i],9999,0);
		networkNodeGroup[_start[i]].addEdge((*this).getNumNode(),9999,0);

		size_t lastIdx0=tmpNode.getEdgeSize()-1;
		size_t lastIdx1=networkNodeGroup[_start[i]].getEdgeSize()-1;

		tmpNode.updateAntiEdgeIndex(lastIdx0,lastIdx1);
		networkNodeGroup[_start[i]].updateAntiEdgeIndex(lastIdx1,lastIdx0);
	}
	

	//construct a total end, connecting to all client. 
	NetworkNode tmpNode1;
	
	// cout<<"end: ";
	// for(size_t i=0;i<_end.size();i++)
	// {
	// 	cout<<_end[i]<<" ";
	// }
	// cout<<endl;
	for(size_t i=0;i<_end.size();i++)
	{	
		//ToIndexNode BandWidth CostUnit
		size_t tmpBandwith=demandRest[i];
		tmpNode1.addEdge(_end[i],tmpBandwith,0);
		networkNodeGroup[_end[i]].addEdge(endIndex,tmpBandwith,0);

		size_t lastIdx0=tmpNode1.getEdgeSize()-1;
		size_t lastIdx1=networkNodeGroup[_end[i]].getEdgeSize()-1;

		tmpNode1.updateAntiEdgeIndex(lastIdx0,lastIdx1);
		networkNodeGroup[_end[i]].updateAntiEdgeIndex(lastIdx1,lastIdx0);
	}



	networkNodeGroup.push_back(tmpNode);
	networkNodeGroup.push_back(tmpNode1);



	size_t numNode=getNumNode()+2;


	//copy the new networknode
	vector<NetworkNode> networkNodeGroup1;
	for (size_t i=0;i<(*this).getNumNode()+2;i++)
	{
		NetworkNode tmp(networkNodeGroup[i]);
		networkNodeGroup1.push_back(tmp);
	}

	// size_t neck=-1;
	// size_t u;
	// size_t h[numNode];
	// size_t cur_flow,flow_ans=0,tmp;

	// int numh[numNode],curEdges[numNode],pre[numNode];
	// for (size_t i=0;i<numNode;i++)
	// {
	// 	numh[i]=0;
	// 	h[i]=0;
	// 	pre[i]=-1;
	// 	curEdges[i]=0;
	// }

	// numh[0]=numNode;
	// u=start;

	size_t pre[numNode];
	size_t curEdge[numNode];
	int vis[numNode];
	size_t dis[numNode];
	size_t flow_ans=0;
	queue<size_t>que;
	//spfa
	
	while(1)
	{
		for(size_t i=0;i<numNode;i++)
		{
			dis[i]=9999999;
			vis[i]=0;
		}

		dis[start]=0;
		que.push(start);

		while(!que.empty())
		{
			size_t u=que.front();
			que.pop();
			vis[u]=1;
			for(size_t j=0;j<networkNodeGroup[u].getEdgeSize();j++)
			{
				size_t IndexNext=networkNodeGroup[u].getToIndexNode(j);
				if(networkNodeGroup[u].getBandWidth(j)&&dis[IndexNext]>dis[u]+networkNodeGroup[u].getCostUnit(j))
				{  
					dis[IndexNext]=dis[u]+networkNodeGroup[u].getCostUnit(j);
					pre[IndexNext]=u;
					curEdge[IndexNext]=j;
					if(vis[IndexNext]==0)
					{
						vis[IndexNext]=1;
						que.push(IndexNext);
					}
				}
			}
			vis[u]=0;
		}

		if(dis[end]==9999999)
		{
			break;
		}
		else
		{	
			
			vector<size_t>tmpPath;
			vector<size_t>tmpEdge;
			size_t sum=9999999;

			// tmpPath.push_back(end);
			// tmpEdge.push_back(0);
			for(size_t i=end;i!=start;i=pre[i])
			{	
				
				size_t restBandWidth=networkNodeGroup[pre[i]].getBandWidth(curEdge[i]);

				sum=sum<restBandWidth?sum:restBandWidth;
				if(pre[i]!=start)
				{
					tmpPath.push_back(pre[i]);
					tmpEdge.push_back(curEdge[i]);
				}

			}
			
			size_t indexCons=networkNodeGroup[tmpPath.front()].getToIndexCons();
			demandRest[indexCons]-=sum;
			// cout<<"demandRest: "<<demandRest[indexCons]<<endl;
			// if(sum>demandRest[indexCons])//rest>need
			// {
			// 	sum=demandRest[indexCons];
			// 	demandRest[indexCons]=0;
			// }
			// else//rest<=need
			// {
			// 	demandRest[indexCons]-=sum;
			// }



			for(size_t i=end;i!=start;i=pre[i])
			{
				size_t indexPre=pre[i];
				networkNodeGroup[indexPre].editBandWidth(curEdge[i],-sum);
				size_t antiEdge=networkNodeGroup[indexPre].getAntiEdgeIndex(curEdge[i]);
				networkNodeGroup[i].editBandWidth(antiEdge,sum);
				flow_ans+=sum;
			}
			
			// size_t start1=tmpPath.back();
			// size_t end1=tmpPath.front();

			// Flow *tmp=new Flow(start1,end1);
			// tmp->editFlow(sum);

			// for(long int i=tmpPath.size()-1;i>=0;i--)
			// {
			// 	tmp->pushPair(tmpPath[i],tmpEdge[i]);
			// 	tmp->editFlowCostUnit(networkNodeGroup[tmpPath[i]].getCostUnit(tmpEdge[i]));
			// }

			// vector<Flow* >&flowLib=*(consNodeGroup[indexCons].getFlowLib());
			// flowLib.push_back(tmp);

			//verify whether satisfaction

		}

	}

	int satisfied=0;
	indexConsOverLoad.clear();
	for(size_t i=0;i<demandRest.size();i++)
	{
		if(demandRest[i]!=0)
		{
			satisfied=-1;
			indexConsOverLoad.insert(i);
		}
	}


	
	
	if(satisfied==0)
	{
		//get the real route
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		demandRest.clear();
		for(size_t i=0;i<numCons;i++)
		{
			demandRest.push_back(consNodeGroup[i].getDemand());
		}

		//DFS
		vector<size_t>nodeStack;
		
		for(size_t i=0;i<numNode;i++)
		{
			vis[i]=0;
		}
		nodeStack.push_back(start);
		vis[start]=1;
		while(nodeStack.size())
		{
			size_t u=nodeStack.back();

			//find one edge
			int pathOrNot=-1;
			for(size_t j=0;j<networkNodeGroup[u].getEdgeSize();j++)
			{
				size_t IndexNext=networkNodeGroup[u].getToIndexNode(j);

				size_t fullBandWidth=networkNodeGroup1[u].getBandWidth(j);

				// cout<<networkNodeGroup[u].getBandWidth(j)<<" "<<fullBandWidth<<endl;


				if(networkNodeGroup[u].getBandWidth(j)<fullBandWidth&&vis[IndexNext]==0)
				{
					// cout<<u<<" "<<IndexNext<<endl;

					pre[IndexNext]=u;
					curEdge[IndexNext]=j;
					vis[IndexNext]=1;
					nodeStack.push_back(IndexNext);
					pathOrNot=0;
					break;
				}
			}
	// cout<<"---------------------------------p1"<<endl;
			//sign of go throuh all the route and the stack it empty
			if(pathOrNot==-1)
			{
				// cout<<u<<"here"<<start<<endl;
				vis[u]=1;
				nodeStack.pop_back();
				if(nodeStack.size()==0)
					break;
			}


			if(nodeStack.back()==end)
			{

// cout<<"---------------------------------p2"<<endl;
				//recall

				//record edge
				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				vector<size_t>tmpPath;
				vector<size_t>tmpEdge;
				size_t sum=9999999;

				// tmpPath.push_back(end);
				// tmpEdge.push_back(0);

				//recall point
				size_t recallPoint=start;
				for(size_t i=end;i!=start;i=pre[i])
				{	
					
					size_t restBandWidth=networkNodeGroup1[pre[i]].getBandWidth(curEdge[i])-networkNodeGroup[pre[i]].getBandWidth(curEdge[i]);

					// sum=sum<restBandWidth?sum:restBandWidth;
					if(sum>=restBandWidth)
					{
						sum=restBandWidth;
						recallPoint=pre[i];

					}

					// cout<<networkNodeGroup1[pre[i]].getBandWidth(curEdge[i])<<" "<<networkNodeGroup[pre[i]].getBandWidth(curEdge[i])<<endl;
					if(pre[i]!=start)
					{
						tmpPath.push_back(pre[i]);
						tmpEdge.push_back(curEdge[i]);
					}

				}
				
				//recall
				while(1)
				{
					if(nodeStack.back()==recallPoint)
						break;
					vis[nodeStack.back()]=0;
					nodeStack.pop_back();
				}



				size_t indexCons=networkNodeGroup[tmpPath.front()].getToIndexCons();
				// demandRest[indexCons]-=sum;
				// cout<<"demandRest: "<<demandRest[indexCons]<<endl;
				// if(sum>demandRest[indexCons])//rest>need
				// {
				// 	sum=demandRest[indexCons];
				// 	demandRest[indexCons]=0;
				// }
				// else//rest<=need
				// {
				// 	demandRest[indexCons]-=sum;
				// }



				for(size_t i=end;i!=start;i=pre[i])
				{

					size_t indexPre=pre[i];
					networkNodeGroup[indexPre].editBandWidth(curEdge[i],sum);
					// flow_ans+=sum;
				}
				
				size_t start1=tmpPath.back();
				size_t end1=tmpPath.front();

				Flow *tmp=new Flow(start1,end1);
				tmp->editFlow(sum);

				for(long int i=tmpPath.size()-1;i>=0;i--)
				{
					tmp->pushPair(tmpPath[i],tmpEdge[i]);
					tmp->editFlowCostUnit(networkNodeGroup[tmpPath[i]].getCostUnit(tmpEdge[i]));
				}

				vector<Flow* >&flowLib=*(consNodeGroup[indexCons].getFlowLib());
				flowLib.push_back(tmp);

				//~~~~~~~~~~~~~~~~~~~~~

			}

		}

		//set flowused
		for(size_t i=0;i<numCons;i++)
		{
			vector<Flow* >&flowLib=*(consNodeGroup[i].getFlowLib());
			vector<pair<size_t,size_t> >&flowUsed=*(consNodeGroup[i].getFlowUsed());
			for(size_t j=0;j<flowLib.size();j++)
			{
				pair<size_t,size_t>tmp(j,flowLib[j]->getRestFlow());
				flowUsed.push_back(tmp);
			}
		}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		routeOutput=new Route();

		//output the route for those apply flow from other servers
		for(size_t k=0;k<this->getNumCons();k++)
		{
			for(size_t i=0;i<consNodeGroup[k].getNumFlow();i++)
			{
				vector<size_t>* route=new vector<size_t>();
				consNodeGroup[k].popRoute(i,route);
				routeOutput->pushRoute(route);
			}

		}

		//calculate cost
		cout<<"**************************************\n";
		cout<<"bandwidth is normal."<<endl;
		cout<<"total Cost: "<<routeOutput->costCal(*this)<<endl;	
		cout<<"++++++++++++++++++++++++++++++++++++++\n";

		
	}
	else
	{	
		cout<<"**************************************\n";
		cout<<"bandwidth is overused!"<<endl;
		cout<<"++++++++++++++++++++++++++++++++++++++\n";
	}

	//clean sap route associated with server position and number
	for(size_t i=0;i<this->getNumCons();i++)
	{
		consNodeGroup[i].clearRoute();
	}

	return satisfied;



	//sort flowlib according to distance
	// sortFlow();

	//update map from Edge to path index
	// if(mapEdgeRoute.size()==0)
	// {
	// 	cout<<"mapEdgeRoute not initialized"<<endl;
	// }
	// for(size_t indexFlow=0;indexFlow<flowLib.size();indexFlow++)
	// {
	// 	Flow *tmpflow=flowLib[indexFlow];
	// 	vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
	// 	for(size_t i=0;i<path.size()-1;i++)
	// 	{
	// 		mapEdgeRoute[make_pair(path[i]->first,path[i+1]->first)].push_back(indexFlow);
	// 		// mapEdgeRoute[path[i]->first][path[i+1]->first].push_back(indexFlow);
	// 	}
	// }

	// // cout<<"P2----------------"<<endl;
	// //update rest flow
	// if(flowLibRest.size()!=0)
	// 	flowLibRest.clear();
	// for(size_t i=0;i<flowLib.size();i++)
	// {
	// 	flowLibRest.push_back(flowLib[i]->getRestFlow());
	// }

	return 0;
}

int NetworkInfo::solve(NetworkNode networkNodeGroup[],ConsNode consNodeGroup[],vector<size_t>&serverPos,Route* &routeOutput,EdgeMatrix& globalEdgeMatrix,set<long int> &indexConsOverLoad)
{
			
	//generate and select the flow
	int demandSatisfied=0;
	for(size_t k=0;k<this->getNumCons();k++)
	{
		// cout<<"Q1 "<<k<<endl;spfa
		// consNodeGroup[k].saps(networkNodeGroup,*this,serverPos,consNodeGroup[k].getToIndexNode());
		consNodeGroup[k].spfa(networkNodeGroup,*this,serverPos,consNodeGroup[k].getToIndexNode());
		if(consNodeGroup[k].selectFlow()!=0)
		{
			 cout<<"select: "<<-1<<endl;//can't satifiy the demand
			demandSatisfied=-1;
			indexConsOverLoad.insert(k);
		}
		// cout<<"Q2"<<endl;

	}

	if(demandSatisfied==-1)
	{
		for(size_t i=0;i<this->getNumCons();i++)
		{
			consNodeGroup[i].clearRoute();
		}
		// cout<<"demand not satisied!!!"<<endl;
		return -1;

	}
	
	//test regulate
	int bandWidthNormal=-1;
	// cout<<"**************************************\n";
	for(size_t i=0;i<3;i++)
	{
		vector<pair<size_t, size_t> > overLoadEdge;
		vector<long int> overLoad;
		

		if(globalEdgeMatrix.checkCons(consNodeGroup,*this,overLoadEdge,overLoad)==0)
		{
			// cout<<"No flow overload."<<endl;
			bandWidthNormal=0;
			// indexConsOverLoad=-1;
			indexConsOverLoad.clear();
			break;
		}
		else
		{
			// indexConsOverLoad=this->getNumCons()-1;	
			for(size_t j=0;j<this->getNumCons();j++)
			{
					int regulated;
					if(consNodeGroup[j].regulate(overLoadEdge,overLoad,regulated)==0)
					{	
						// cout<<"Regulation Done."<<endl;
						// indexConsOverLoad=j;
						indexConsOverLoad.insert(j);
						break;
					}
					else
					{
						if(regulated==0)
							indexConsOverLoad.insert(j);
							// indexConsOverLoad=j;
					}

			}
			
		}
		// cout<<"Regulate: "<<i<<endl;
		// cout<<"after regulation\n";
		// for(size_t j=0;j<overLoadEdge.size();j++)
		// {
		// 	cout<<"("<<overLoadEdge[j].first<<","<<overLoadEdge[j].second<<"): "<<overLoad[j]<<endl;
		// }

	}

	//output the solution
	if(bandWidthNormal==0)
	{
		routeOutput=new Route();

		//output the route for those apply flow from other servers
		for(size_t k=0;k<this->getNumCons();k++)
		{
			for(size_t i=0;i<consNodeGroup[k].getNumFlow();i++)
			{
				vector<size_t>* route=new vector<size_t>();
				consNodeGroup[k].popRoute(i,route);
				routeOutput->pushRoute(route);
			}

		}

		//calculate cost
		cout<<"**************************************\n";
		cout<<"bandwidth is normal."<<endl;
		cout<<"total Cost: "<<routeOutput->costCal(*this)<<endl;	
		cout<<"++++++++++++++++++++++++++++++++++++++\n";

		
	}
	else
	{	
		cout<<"**************************************\n";
		cout<<"bandwidth is overused!"<<endl;
		cout<<"++++++++++++++++++++++++++++++++++++++\n";
	}

	//clean sap route associated with server position and number
	for(size_t i=0;i<this->getNumCons();i++)
	{
		consNodeGroup[i].clearRoute();
	}
	

	return bandWidthNormal;
}

int NetworkInfo::solve(NetworkNode networkNodeGroup[],ConsNode consNodeGroup[],vector<size_t>&serverPos,Route* &routeOutput,EdgeMatrix& globalEdgeMatrix,long int &indexConsOverLoad)
{
			
	//generate and select the flow
	int demandSatisfied=0;
	for(size_t k=0;k<this->getNumCons();k++)
	{
		// cout<<"Q1 "<<k<<endl;spfa
		// consNodeGroup[k].saps(networkNodeGroup,*this,serverPos,consNodeGroup[k].getToIndexNode());
		consNodeGroup[k].spfa(networkNodeGroup,*this,serverPos,consNodeGroup[k].getToIndexNode());
		if(consNodeGroup[k].selectFlow()!=0)
		{
			 cout<<"select: "<<-1<<endl;//can't satifiy the demand
			demandSatisfied=-1;
			indexConsOverLoad=k;
		}
		// cout<<"Q2"<<endl;

	}

	if(demandSatisfied==-1)
	{
		for(size_t i=0;i<this->getNumCons();i++)
		{
			consNodeGroup[i].clearRoute();
		}
		// cout<<"demand not satisied!!!"<<endl;
		return -1;

	}
	
	//test regulate
	int bandWidthNormal=-1;
	// cout<<"**************************************\n";
	for(size_t i=0;i<3;i++)
	{
		vector<pair<size_t, size_t> > overLoadEdge;
		vector<long int> overLoad;
		

		if(globalEdgeMatrix.checkCons(consNodeGroup,*this,overLoadEdge,overLoad)==0)
		{
			// cout<<"No flow overload."<<endl;
			bandWidthNormal=0;
			indexConsOverLoad=-1;
			break;
		}
		else
		{
			indexConsOverLoad=this->getNumCons()-1;	
			for(size_t j=0;j<this->getNumCons();j++)
			{
					int regulated;
					if(consNodeGroup[j].regulate(overLoadEdge,overLoad,regulated)==0)
					{	
						// cout<<"Regulation Done."<<endl;
						indexConsOverLoad=j;
						break;
					}
					else
					{
						if(regulated==0)
							indexConsOverLoad=j;
					}

			}
			
		}
		// cout<<"Regulate: "<<i<<endl;
		// cout<<"after regulation\n";
		// for(size_t j=0;j<overLoadEdge.size();j++)
		// {
		// 	cout<<"("<<overLoadEdge[j].first<<","<<overLoadEdge[j].second<<"): "<<overLoad[j]<<endl;
		// }

	}

	//output the solution
	if(bandWidthNormal==0)
	{
		routeOutput=new Route();

		//output the route for those apply flow from other servers
		for(size_t k=0;k<this->getNumCons();k++)
		{
			for(size_t i=0;i<consNodeGroup[k].getNumFlow();i++)
			{
				vector<size_t>* route=new vector<size_t>();
				consNodeGroup[k].popRoute(i,route);
				routeOutput->pushRoute(route);
			}

		}

		//calculate cost
		cout<<"**************************************\n";
		cout<<"bandwidth is normal."<<endl;
		cout<<"total Cost: "<<routeOutput->costCal(*this)<<endl;	
		cout<<"++++++++++++++++++++++++++++++++++++++\n";

		
	}
	else
	{	
		cout<<"**************************************\n";
		cout<<"bandwidth is overused!"<<endl;
		cout<<"++++++++++++++++++++++++++++++++++++++\n";
	}

	//clean sap route associated with server position and number
	for(size_t i=0;i<this->getNumCons();i++)
	{
		consNodeGroup[i].clearRoute();
	}
	

	return bandWidthNormal;
}
int NetworkInfo::initServerPool(ConsNode consNodeGroup[])
{

	vector<size_t>consIndex;
	for(size_t i=0;i<getNumCons();i++)
		consIndex.push_back(i);

	for(size_t i=0;i<getNumCons()-1;i++)
	{
		for(size_t j=i+1;j<getNumCons();j++)
		{
			if (consNodeGroup[consIndex[i]].getDemand()<consNodeGroup[consIndex[j]].getDemand())
			{
				size_t tmp=consIndex[i];
				consIndex[i]=consIndex[j];
				consIndex[j]=tmp;
			}


		}
	}

	// cout<<"index:"<<endl;
	
	for(size_t i=0;i<getNumCons();i++)
	{
		serverPosSorted.push_back(consNodeGroup[consIndex[i]].getToIndexNode());
		// cout<<consNodeGroup[consIndex[i]].getToIndexNode()<<":"<<consNodeGroup[consIndex[i]].getDemand()<<" ";
	}


	return 0;
}
int NetworkInfo::constructServerPool(NetworkNode networkNodeGroup[],ConsNode consNodeGroup[])
{

	for(size_t i=0;i<numCons;i++)
	{
		vector<size_t>extraServerPos;
		for(size_t j=0;j<numCons;j++)
		{
			
			if(j!=i)
			{
				extraServerPos.push_back(consNodeGroup[j].getToIndexNode());
			}
		}
		
		consNodeGroup[i].spfa(networkNodeGroup,*this,extraServerPos,consNodeGroup[i].getToIndexNode());
		consNodeGroup[i].constructServerPool();
		consNodeGroup[i].clearRoute();

	}
	// cout<<"here"<<endl;

	//map server to client
	for(size_t i=0;i<numCons;i++)
	{
		for(size_t j=0;j<consNodeGroup[i].getServerPoolSize();j++)
		{
			serverToCons[consNodeGroup[i].getServerPoolServer(j)].push_back(i);
		}

	}

	//map direct link
	for(size_t i=0;i<directLink.size();i++)
	{
		directLinkMap[directLink[i]]=1;
	}

	return 0;
}
int NetworkInfo::born(vector<size_t>&p1,vector<size_t>&p2,vector<size_t>&output)
{
	if(output.size())
		output.clear();

	vector<unsigned int> gene(numNode,0);
	for(size_t i=0;i<p1.size();i++)
	{
		gene[p1[i]]++;
	}

	for(size_t i=0;i<p2.size();i++)
	{
		gene[p2[i]]++;
	}

	for(size_t i=0;i<gene.size();i++)
	{
		if(gene[i]==2)
			output.push_back(i);

		if(gene[i]==1)
			if(rand()%2==0)
				output.push_back(i);
		// if(gene[i]>=1)
		// 	output.push_back(i);

	}
	return 0;
}


int NetworkInfo::born3(vector<size_t>&p1,vector<size_t>&p2,vector<size_t>&p3,vector<size_t>&output)
{
	if(output.size())
		output.clear();

	vector<unsigned int> gene(numNode,0);
	for(size_t i=0;i<p1.size();i++)
	{
		gene[p1[i]]++;
	}

	for(size_t i=0;i<p2.size();i++)
	{
		gene[p2[i]]++;
	}

	for(size_t i=0;i<p3.size();i++)
	{
		gene[p3[i]]++;
	}

	for(size_t i=0;i<gene.size();i++)
	{
		if(gene[i]==3)
			output.push_back(i);

		if(gene[i]==2)
			if(rand()%3>0)
				output.push_back(i);

		if(gene[i]==1)
			if(rand()%3==0)
				output.push_back(i);
		// if(gene[i]>=1)
		// 	output.push_back(i);

	}
	return 0;
}


int NetworkInfo::reDeploy(set<long int> &indexConsOverLoad,vector<size_t>&serverPos, ConsNode consNodeGroup[])
{	//reset server
	if(!consDeployFlag.size())
	{
		consDeployFlag.resize(numCons,0);
	}
	else
	{
		for(size_t i=0;i<numCons;i++)
		{
			consDeployFlag[i]=0;
		}
	}

	// cout<<"indexConsOverLoad:[]"<<*indexConsOverLoad.begin()<<endl;
	// cout<<"demand:[]"<<consNodeGroup[*indexConsOverLoad.begin()].getDemand()<<endl;
	// cout<<"restflow:[]"<<consNodeGroup[*indexConsOverLoad.begin()].getTotalRestFlow()<<endl;
	while(indexConsOverLoad.size())
	{
		if(consDeployFlag[*(indexConsOverLoad.begin())]!=1)
		{
			size_t i=*(indexConsOverLoad.begin());
			size_t serverChoose=consNodeGroup[i].getToIndexNode();
			if(rand()%2==0)
			{
				size_t serverPosIndex=(size_t)(rand()%consNodeGroup[i].getServerPoolSize()*0.2);
				serverChoose=consNodeGroup[i].getServerPoolServer(serverPosIndex);
			}
			// size_t serverPosIndex=(size_t)(rand()%consNodeGroup[i].getServerPoolSize()*0.02);
			// size_t serverChoose=consNodeGroup[i].getServerPoolServer(serverPosIndex);
			serverPos.push_back(serverChoose);

			indexConsOverLoad.erase(indexConsOverLoad.begin());

			for(size_t j=0;j<serverToCons[serverChoose].size();j++)
			{
					consDeployFlag[serverToCons[serverChoose][j]]++;
					set<long int>::iterator iter=indexConsOverLoad.find(serverToCons[serverChoose][j]);
					if(iter!=indexConsOverLoad.end())
						indexConsOverLoad.erase(iter);
			}

		}
	}
	return 0;
}
int NetworkInfo::initInherit(size_t numGroup)
{
	for(size_t i=0;i<numGroup;i++)
	{
		list<vector<size_t>* >tmpSolution;
		list<size_t>tmpCost;

		solutionGroup.push_back(tmpSolution);
		costGroup.push_back(tmpCost);
	}
	return 0;
}
int NetworkInfo::saveServerInherit(vector<size_t>&serverPos,size_t lastCost,size_t indexGroup)
{
	list<size_t>& subCost=costGroup[indexGroup];
	list<vector<size_t>* >& subSolution=solutionGroup[indexGroup];
	// cout<<"-------------------"<<indexGroup<<endl;
	//solution queue
	if(serverPos.size()>0)
	{
		list<size_t>::iterator iterCost=subCost.begin();
		list<vector<size_t>* >::iterator iterSolution=subSolution.begin();
		for(;iterCost!=subCost.end();iterCost++,iterSolution++)
		{
			if(lastCost<=*iterCost)
				break;

		}
		
		//don't need same
		if(lastCost!=*iterCost)
		{
			subCost.insert(iterCost,lastCost);
			subSolution.insert(iterSolution,new vector<size_t>(serverPos));
			if(subCost.size()>80)
			{
				subCost.pop_back();
				vector<size_t>*tmp=subSolution.back();
				subSolution.pop_back();
				delete tmp;
			}
			//verify
			cout<<"cost queue "<<indexGroup<<": ";
			list<size_t>::iterator iter=subCost.begin();
			for(;iter!=subCost.end();iter++)
				cout<<*iter<<" ";
			cout<<endl<<endl;
		}


	}
	return 0;
}

int NetworkInfo::deployServerInherit(ConsNode consNodeGroup[], NetworkNode networkNodeGroup[] ,vector<size_t>&serverPos,size_t indexGroup)
{
	//reset server
	if(!consDeployFlag.size())
	{
		consDeployFlag.resize(numCons,0);
	}
	else
	{
		for(size_t i=0;i<numCons;i++)
		{
			consDeployFlag[i]=0;
		}
	}

	if(!serverPos.size())
	{
		serverPos.clear();
	}



	list<size_t>& subCost=costGroup[indexGroup];
	// list<vector<size_t>* >& subSolution=solutionGroup[indexGroup];

	// //solution queue
	// if(serverPos.size()>0)
	// {
	// 	list<size_t>::iterator iterCost=subCost.begin();
	// 	list<vector<size_t>* >::iterator iterSolution=subSolution.begin();
	// 	for(;iterCost!=subCost.end();iterCost++,iterSolution++)
	// 	{
	// 		if(lastCost<=*iterCost)
	// 			break;

	// 	}
		
	// 	//don't need same
	// 	if(lastCost!=*iterCost)
	// 	{
	// 		subCost.insert(iterCost,lastCost);
	// 		subSolution.insert(iterSolution,new vector<size_t>(serverPos));
	// 		if(subCost.size()>50)
	// 		{
	// 			subCost.pop_back();
	// 			vector<size_t>*tmp=subSolution.back();
	// 			subSolution.pop_back();
	// 			delete tmp;
	// 		}
	// 		//verify
	// 		cout<<"cost queue "<<indexGroup<<": ";
	// 		list<size_t>::iterator iter=subCost.begin();
	// 		for(;iter!=subCost.end();iter++)
	// 			cout<<*iter<<" ";
	// 		cout<<endl<<endl;
	// 	}


	// }
	
	srand(clock());

	if(subCost.size()>79&&rand()%10>1)
	{
		serverPos.clear();


		randomBorn(serverPos,indexGroup);
		// size_t p1=rand()%(subCost.size());
		// size_t p2=rand()%(subCost.size());
		// size_t p3=rand()%(subCost.size());

		// list<vector<size_t>* >::iterator p1Itr=subSolution.begin();
		// list<vector<size_t>* >::iterator p2Itr=subSolution.begin();
		// list<vector<size_t>* >::iterator p3Itr=subSolution.begin();
		// for(size_t i=0;i<p1;i++)
		// {
		// 	p1Itr++;
		// }
		// for(size_t i=0;i<p2;i++)
		// {
		// 	p2Itr++;
		// }
		// for(size_t i=0;i<p3;i++)
		// {
		// 	p3Itr++;
		// }

		// born3(**p1Itr,**p2Itr,**p3Itr,serverPos);


		// born(**p1Itr,**p2Itr,serverPos);
		// cout<<"Born: ";
		// for(size_t i=0;i<serverPos.size();i++)
		// {
		// 	cout<<serverPos[i]<<" ";
		// }
		// cout<<endl;
		// cout<<serverPos.size()<<"haha"<<getNumCons()<<endl;
	}
	else
	{
		serverPos.clear();
		// map<size_t,size_t>serverCount;
		// vector<size_t>serverSort;

		// for(size_t i=0;i<numCons;i++)
		// {
		// 	for(size_t j=0;j<consNodeGroup[i].getServerPoolSize();j++)
		// 	{
		// 		size_t serverChoose=consNodeGroup[i].getServerPoolServer(j);
		// 		if(serverCount.find(serverChoose)!=serverCount.end())
		// 			serverCount[serverChoose]++;
		// 		else
		// 		{
		// 			serverCount[serverChoose]=1;
		// 			serverSort.push_back(serverChoose);
		// 		}
		// 	}
		// }

		// //sort
		// for(size_t i=0;i<serverSort.size()-1;i++)
		// 	for(size_t j=i+1;j<serverSort.size();j++)
		// 		if(serverCount[serverSort[i]]<serverCount[serverSort[j]])
		// 		{
		// 			size_t tmp=serverSort[i];
		// 			serverSort[i]=serverSort[j];
		// 			serverSort[j]=tmp;
		// 		}

		// map<size_t,size_t>::iterator iter=serverCount.begin();
		// cout<<"candidate"<<endl;
		// // for(;iter!=serverCount.end();iter++)
		// // 	cout<<iter->first<<":"<<iter->second<<" ";
		// for(size_t i=0;i<serverSort.size();i++)
		// 	cout<<serverSort[i]<<":"<<serverCount[serverSort[i]]<<" ";
		// cout<<endl;

		// for(size_t i=0;i<serverSort.size()*0.6;i++)
		// 	if(rand()%(i+1)<=2)
		// 		serverPos.push_back(serverSort[i]);
		// for(size_t i=0;i<numCons;i++)
		// {
		// 	size_t ii=(size_t)(rand()%serverSort.size());
		// 	serverPos.push_back(serverSort[ii]);
		// }

		// int o;
		// cin>>o;



		// size_t randomOffset=rand()%numCons;
		// size_t iOffset=0;

		//random search
		// set<size_t>sequence;
		// while(sequence.size()!=numCons)
		// {
		// 	size_t i=rand()%numCons;
		// 	size_t oldSize=sequence.size();
		// 	sequence.insert(i);
		// 	if(sequence.size()==oldSize)
		// 		continue;
		// 	else
		// 	{
		// 		if(consDeployFlag[i]!=1)
		// 		{	
		// 			// cout<<"-------------------"<<consNodeGroup[i].getServerPoolSize()<<endl;
		// 			size_t poolSize=consNodeGroup[i].getServerPoolSize()<1?consNodeGroup[i].getServerPoolSize():1;
		// 			// cout<<"---------------------"<<poolSize<<endl;
		// 			size_t serverPosIndex=(size_t)(rand()%poolSize);//consNodeGroup[i].getServerPoolSize()*0.05);
		// 			size_t serverChoose=consNodeGroup[i].getServerPoolServer(serverPosIndex);
		// 			serverPos.push_back(serverChoose);
					
		// 			// set the client including same server
		// 			for(size_t j=0;j<serverToCons[serverChoose].size();j++)
		// 			{
		// 				// if(rand()%2==0)
		// 				// {
		// 					consDeployFlag[serverToCons[serverChoose][j]]++;
		// 				// }
		// 			}
		// 		}
		// 	}

		// }

		for(size_t i=0;i<numCons;i++)
		{
			if(rand()%numCons>=i)
				serverPos.push_back(serverPosSorted[i]);
		}


	}
	
	
	return 0;

}
int NetworkInfo::randomBorn(vector<size_t>& output,size_t indexGroup)
{
	list<size_t>& subCost=costGroup[indexGroup];
	list<vector<size_t>* >& subSolution=solutionGroup[indexGroup];

	if(output.size())
		output.clear();
	size_t crossNum=rand()%10;
	 crossNum=69;

	vector<size_t>pN;
	vector<list<vector<size_t>* >::iterator >pNItr;
	vector<unsigned int> gene(numNode,0);

	for(size_t i=0;i<crossNum;i++)
	{
		pN.push_back(rand()%subCost.size());
		list<vector<size_t>* >::iterator p1Itr=subSolution.begin();
		
		for(size_t j=0;j<pN[i];j++)
		{
			p1Itr++;
		}
		pNItr.push_back(p1Itr);

		vector<size_t>& p1Vec=**(pNItr[i]);
		for(size_t j=0;j<p1Vec.size();j++)
		{
			gene[p1Vec[j]]++;
		}
	}

	for(size_t i=0;i<gene.size();i++)
	{
		if(gene[i]!=0)
			if(rand()%crossNum<gene[i])
				output.push_back(i);
	}

	






	// for(size_t i=0;i<p1.size();i++)
	// {
	// 	gene[p1[i]]++;
	// }

	// for(size_t i=0;i<p2.size();i++)
	// {
	// 	gene[p2[i]]++;
	// }

	// for(size_t i=0;i<p3.size();i++)
	// {
	// 	gene[p3[i]]++;
	// }



	return 0;
}
int NetworkInfo::deployServerInheritGroupCross(vector<size_t>&serverPos)
{

	if(costCross.size()>2&&rand()%2==0)
	{
		
		size_t p1=rand()%costCross.size();
		size_t p2=rand()%costCross.size();
		list<vector<size_t>* >::iterator p1Itr=solutionCross.begin();
		list<vector<size_t>* >::iterator p2Itr=solutionCross.begin();
		for(size_t i=0;i<p1;i++)
		{
			p1Itr++;
		}
		for(size_t i=0;i<p2;i++)
		{
			p2Itr++;
		}

		born(**p1Itr,**p2Itr,serverPos);
	}
	else
	{
		size_t indexGroup1=rand()%costGroup.size();
		size_t indexGroup2=rand()%costGroup.size();

		if(indexGroup1==indexGroup2)
			indexGroup2=(indexGroup1+1)%costGroup.size();

		list<size_t>& subCost1=costGroup[indexGroup1];
		list<vector<size_t>* >& subSolution1=solutionGroup[indexGroup1];

		list<size_t>& subCost2=costGroup[indexGroup2];
		list<vector<size_t>* >& subSolution2=solutionGroup[indexGroup2];


		serverPos.clear();

		size_t c1=subCost1.size()/5+1;//(size_t)((double)subCost1.size()*0.5);
		size_t c2=subCost2.size()/5+1;//(size_t)((double)subCost2.size()*0.5);
		size_t p1=rand()%c1;
		size_t p2=rand()%c2;

		list<vector<size_t>* >::iterator p1Itr=subSolution1.begin();
		list<vector<size_t>* >::iterator p2Itr=subSolution2.begin();
		for(size_t i=0;i<p1;i++)
		{
			p1Itr++;
		}
		for(size_t i=0;i<p2;i++)
		{
			p2Itr++;
		}

		born(**p1Itr,**p2Itr,serverPos);
	}

	// cout<<"Born: ";
	// for(size_t i=0;i<serverPos.size();i++)
	// {
	// 	cout<<serverPos[i]<<" ";
	// }
	// cout<<endl;
	// cout<<serverPos.size()<<"haha"<<getNumCons()<<endl;
	

	return 0;
}

int NetworkInfo::saveServerInheritGroupCross(vector<size_t>&serverPos, size_t lastCost)
{
	//solution queue
	if(serverPos.size()>0)
	{
		list<size_t>::iterator iterCost=costCross.begin();
		list<vector<size_t>* >::iterator iterSolution=solutionCross.begin();
		for(;iterCost!=costCross.end();iterCost++,iterSolution++)
		{
			if(lastCost<=*iterCost)
				break;

		}
		
		//don't need same
		if(lastCost!=*iterCost)
		{
			costCross.insert(iterCost,lastCost);
			solutionCross.insert(iterSolution,new vector<size_t>(serverPos));
			if(costCross.size()>10)
			{
				costCross.pop_back();
				vector<size_t>*tmp=solutionCross.back();
				solutionCross.pop_back();
				delete tmp;
			}
			//verify
			cout<<"cross cost queue: ";
			list<size_t>::iterator iter=costCross.begin();
			for(;iter!=costCross.end();iter++)
				cout<<*iter<<" ";
			cout<<endl<<endl;
		}


	}
	return 0;
}
// int NetworkInfo::deployServer(ConsNode consNodeGroup[], NetworkNode networkNodeGroup[] ,vector<size_t>&serverPos,size_t lastCost)
// {
// 	//reset server
// 	if(!consDeployFlag.size())
// 	{
// 		consDeployFlag.resize(numCons,0);
// 	}
// 	else
// 	{
// 		for(size_t i=0;i<numCons;i++)
// 		{
// 			consDeployFlag[i]=0;
// 		}
// 	}

// 	// if(!serverPos.size())
// 	// {
// 	// 	serverPos.clear();
// 	// }

// 	//solution queue
// 	if(serverPos.size()>0)
// 	{
// 		list<size_t>::iterator iterCost=cost.begin();
// 		list<vector<size_t>* >::iterator iterSolution=solution.begin();
// 		for(;iterCost!=cost.end();iterCost++,iterSolution++)
// 		{
// 			if(lastCost<=*iterCost)
// 				break;

// 		}
		
// 		//don't need same
// 		if(lastCost!=*iterCost)
// 		{
// 			cost.insert(iterCost,lastCost);
// 			solution.insert(iterSolution,new vector<size_t>(serverPos));
// 			if(cost.size()>50)
// 			{
// 				cost.pop_back();
// 				vector<size_t>*tmp=solution.back();
// 				solution.pop_back();
// 				delete tmp;
// 			}
// 			//verify
// 			cout<<"cost queue: ";
// 			list<size_t>::iterator iter=cost.begin();
// 			for(;iter!=cost.end();iter++)
// 				cout<<*iter<<" ";
// 			cout<<endl<<endl;
// 		}


// 	}
	
// 	srand(clock());

// 	if(cost.size()>40&&rand()%10>1)
// 	{
// 		serverPos.clear();

// 		size_t p1=rand()%cost.size();
// 		size_t p2=rand()%cost.size();

// 		list<vector<size_t>* >::iterator p1Itr=solution.begin();
// 		list<vector<size_t>* >::iterator p2Itr=solution.begin();
// 		for(size_t i=0;i<p1;i++)
// 		{
// 			p1Itr++;
// 		}
// 		for(size_t i=0;i<p2;i++)
// 		{
// 			p2Itr++;
// 		}

// 		born(**p1Itr,**p2Itr,serverPos);
// 		cout<<"Born: ";
// 		for(size_t i=0;i<serverPos.size();i++)
// 		{
// 			cout<<serverPos[i]<<" ";
// 		}
// 		cout<<endl;
// 		cout<<serverPos.size()<<"haha"<<getNumCons()<<endl;
// 	}
// 	else
// 	{
// 		serverPos.clear();
// 		map<size_t,size_t>serverCount;
// 		vector<size_t>serverSort;

// 		for(size_t i=0;i<numCons;i++)
// 		{
// 			for(size_t j=0;j<consNodeGroup[i].getServerPoolSize();j++)
// 			{
// 				size_t serverChoose=consNodeGroup[i].getServerPoolServer(j);
// 				if(serverCount.find(serverChoose)!=serverCount.end())
// 					serverCount[serverChoose]++;
// 				else
// 				{
// 					serverCount[serverChoose]=1;
// 					serverSort.push_back(serverChoose);
// 				}
// 			}
// 		}

// 		//sort
// 		for(size_t i=0;i<serverSort.size()-1;i++)
// 			for(size_t j=i+1;j<serverSort.size();j++)
// 				if(serverCount[serverSort[i]]<serverCount[serverSort[j]])
// 				{
// 					size_t tmp=serverSort[i];
// 					serverSort[i]=serverSort[j];
// 					serverSort[j]=tmp;
// 				}

// 		// map<size_t,size_t>::iterator iter=serverCount.begin();
// 		// cout<<"candidate"<<endl;
// 		// // for(;iter!=serverCount.end();iter++)
// 		// // 	cout<<iter->first<<":"<<iter->second<<" ";
// 		// for(size_t i=0;i<serverSort.size();i++)
// 		// 	cout<<serverSort[i]<<":"<<serverCount[serverSort[i]]<<" ";
// 		// cout<<endl;

// 		// for(size_t i=0;i<serverSort.size()*0.6;i++)
// 		// 	if(rand()%(i+1)<=2)
// 		// 		serverPos.push_back(serverSort[i]);
// 		// for(size_t i=0;i<numCons;i++)
// 		// {
// 		// 	size_t ii=(size_t)(rand()%serverSort.size());
// 		// 	serverPos.push_back(serverSort[ii]);
// 		// }

// 		// int o;
// 		// cin>>o;



// 		// size_t randomOffset=rand()%numCons;
// 		// size_t iOffset=0;

// 		//random search
// 		set<size_t>sequence;
// 		while(sequence.size()!=numCons)
// 		{
// 			size_t i=rand()%numCons;
// 			size_t oldSize=sequence.size();
// 			sequence.insert(i);
// 			if(sequence.size()==oldSize)
// 				continue;
// 			else
// 			{
// 				if(consDeployFlag[i]!=1)
// 				{	
// 					// cout<<"-------------------"<<consNodeGroup[i].getServerPoolSize()<<endl;
// 					size_t serverPosIndex=(size_t)(rand()%consNodeGroup[i].getServerPoolSize()*0.01);
// 					size_t serverChoose=consNodeGroup[i].getServerPoolServer(serverPosIndex);
// 					serverPos.push_back(serverChoose);
					
// 					// set the client including same server
// 					for(size_t j=0;j<serverToCons[serverChoose].size();j++)
// 					{
// 						// if(rand()%2==0)
// 						// {
// 							consDeployFlag[serverToCons[serverChoose][j]]++;
// 						// }
// 					}
// 				}
// 			}

// 		}

// 		// for(size_t i=0;i<numCons;i++)
// 		// {
// 		// 	iOffset=(i+randomOffset)%numCons;
// 		// 	if(consDeployFlag[iOffset]!=1)
// 		// 	{	
// 		// 		// cout<<"-------------------"<<consNodeGroup[i].getServerPoolSize()<<endl;
// 		// 		size_t serverPosIndex=(size_t)(rand()%consNodeGroup[iOffset].getServerPoolSize()*0.02);
// 		// 		size_t serverChoose=consNodeGroup[iOffset].getServerPoolServer(serverPosIndex);
// 		// 		serverPos.push_back(serverChoose);
				
// 		// 		// set the client including same server
// 		// 		for(size_t j=0;j<serverToCons[serverChoose].size();j++)
// 		// 		{
// 		// 			// if(rand()%2==0)
// 		// 			// {
// 		// 				consDeployFlag[serverToCons[serverChoose][j]]++;
// 		// 			// }
// 		// 		}
// 		// 	}
// 		// }
// 	}
	
	
// 	return 0;

// }

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
		adjEdge.pop_back();
		delete tmp;
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
	// indexMaxRestFlow=-1;
}

ConsNode::~ConsNode()
{
	while(flowLib.size())//clear flow lib
	{
		Flow* tmp=flowLib.back();
		flowLib.pop_back();
		delete tmp;
		
	}
}

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

int ConsNode::spfa(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,vector<size_t>& _start, size_t end)
{
	//clear total flow
	totalFlow=0;

	//if the start point include end
	for(size_t i=0;i<_start.size();i++)
	{
		if(_start[i]==end)
		{
			Flow *tmp=new Flow(end,end);
			tmp->editFlow(99999);
			tmp->pushPair(end,0);
			flowLib.push_back(tmp);

			if(flowLibRest.size()!=0)
				flowLibRest.clear();
			for(size_t i=0;i<flowLib.size();i++)
			{
				flowLibRest.push_back(flowLib[i]->getRestFlow());
			}

			return 0;
		}

	}
		// cout<<"P1----------------"<<endl;
		
	//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

	//construct a total start, connecting to all start.
	NetworkNode tmpNode;
	// cout<<"start: ";
	// for(size_t i=0;i<_start.size();i++)
	// {
	// 	cout<<_start[i]<<" ";
	// }
	// cout<<endl;
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

	// size_t neck=-1;
	// size_t u;
	// size_t h[numNode];
	// size_t cur_flow,flow_ans=0,tmp;

	// int numh[numNode],curEdges[numNode],pre[numNode];
	// for (size_t i=0;i<numNode;i++)
	// {
	// 	numh[i]=0;
	// 	h[i]=0;
	// 	pre[i]=-1;
	// 	curEdges[i]=0;
	// }

	// numh[0]=numNode;
	// u=start;

	size_t pre[numNode];
	size_t curEdge[numNode];
	int vis[numNode];
	size_t dis[numNode];
	size_t flow_ans=0;
	queue<size_t>que;
	//spfa
	
	while(1)
	{
		for(size_t i=0;i<numNode;i++)
		{
			dis[i]=9999999;
			vis[i]=0;
		}

		dis[start]=0;
		que.push(start);

		while(!que.empty())
		{
			size_t u=que.front();
			que.pop();
			vis[u]=1;
			for(size_t j=0;j<networkNodeGroup[u].getEdgeSize();j++)
			{
				size_t IndexNext=networkNodeGroup[u].getToIndexNode(j);
				if(networkNodeGroup[u].getBandWidth(j)&&dis[IndexNext]>dis[u]+networkNodeGroup[u].getCostUnit(j))
				{  
					dis[IndexNext]=dis[u]+networkNodeGroup[u].getCostUnit(j);
					pre[IndexNext]=u;
					curEdge[IndexNext]=j;
					if(vis[IndexNext]==0)
					{
						vis[IndexNext]=1;
						que.push(IndexNext);
					}
				}
			}
			vis[u]=0;
		}

		if(dis[end]==9999999)
		{
			break;
		}
		else
		{	
			
			vector<size_t>tmpPath;
			vector<size_t>tmpEdge;
			size_t sum=9999999;

			tmpPath.push_back(end);
			tmpEdge.push_back(0);
			for(size_t i=end;i!=start;i=pre[i])
			{	
				
				size_t restBandWidth=networkNodeGroup[pre[i]].getBandWidth(curEdge[i]);

				sum=sum<restBandWidth?sum:restBandWidth;
				if(pre[i]!=start)
				{
					tmpPath.push_back(pre[i]);
					tmpEdge.push_back(curEdge[i]);
				}

			}
			

			for(size_t i=end;i!=start;i=pre[i])
			{
				size_t indexPre=pre[i];
				networkNodeGroup[indexPre].editBandWidth(curEdge[i],-sum);
				size_t antiEdge=networkNodeGroup[indexPre].getAntiEdgeIndex(curEdge[i]);
				networkNodeGroup[i].editBandWidth(antiEdge,sum);
				totalFlow+=sum;
				flow_ans+=sum;
			}
			
			Flow *tmp=new Flow(start,end);
			tmp->editFlow(sum);

			for(long int i=tmpPath.size()-1;i>=0;i--)
			{
				tmp->pushPair(tmpPath[i],tmpEdge[i]);
				tmp->editFlowCostUnit(networkNodeGroup[tmpPath[i]].getCostUnit(tmpEdge[i]));
			}

			flowLib.push_back(tmp);

		}

	}


	
	// // cout<<"augment route: "<<endl;
	// while(h[start]<numNode)
	// {
	// 	if(u==end)
	// 	{
	// 		cur_flow=9999;


	// 		static size_t routeIndex=0;
	// 		routeIndex++;

	// 		Flow *tmp=new Flow(start,end);

	// 		// cout<<routeIndex<<"(";
	// 		for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
	// 		{
	// 			if(cur_flow>networkNodeGroup[i].getBandWidth(curEdges[i]))
	// 			{
	// 				neck=i;
	// 				cur_flow=networkNodeGroup[i].getBandWidth(curEdges[i]);
	// 			}



	// 		}

	// 		// cout<<cur_flow<<"): ";
	// 		tmp->editFlow(cur_flow);

	// 		for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
	// 		{
	// 			networkNodeGroup[i].editBandWidth(curEdges[i],-cur_flow);
	// 			size_t antiNode=networkNodeGroup[i].getToIndexNode(curEdges[i]);
	// 			size_t antiEdge=networkNodeGroup[i].getAntiEdgeIndex(curEdges[i]);
	// 			networkNodeGroup[antiNode].editBandWidth(antiEdge,cur_flow);
				
	// 			if (i!=start)//first node is virtual
	// 			{
	// 				//verify the route
	// 				// cout<<i<<"<"<<curEdges[i]<<">"<<" ";

	// 				tmp->pushPair(i,curEdges[i]);
	// 				tmp->editFlowCostUnit(networkNodeGroup[i].getCostUnit(curEdges[i]));
	// 			}

	// 		}

	// 		// cout<<end<<"<"<<0<<">"<<" ";
	// 		tmp->pushPair(end,0);
	// 		// cout<<endl;

	// 		flowLib.push_back(tmp);
			

	// 		totalFlow+=cur_flow;

	// 		flow_ans+=cur_flow;
	// 		u=neck;
	// 	}
	// 	// vector<NetworkEdge*>* adj=networkNodeGroup[u].getAdjEdge();

	// 	// vector<NetworkEdge*>::iterator itr;
	// 	// for (vector<NetworkEdge*>::iterator itr=adj->begin()+curEdges[i];itr!=adj->end();itr++)
	// 	// {
	// 	// 	if((*itr)->getBandWidth()&&h[u]==h[(*itr)->getToIndexNode()]+1)
	// 	// 	{
	// 	// 		break;
	// 	// 	}
	// 	// }

	// 	size_t i;
	// 	for (i=curEdges[u];i<networkNodeGroup[u].getEdgeSize();i++)
	// 	{
	// 		if(networkNodeGroup[u].getBandWidth(i)&&h[u]==h[networkNodeGroup[u].getToIndexNode(i)]+1)
	// 		{
	// 			break;
	// 		}
	// 	}



	// 	if(i!=networkNodeGroup[u].getEdgeSize())
	// 	{
	// 		curEdges[u]=i;
	// 		pre[networkNodeGroup[u].getToIndexNode(i)]=u;
	// 		u=networkNodeGroup[u].getToIndexNode(i);
	// 	}
	// 	else
	// 	{
	// 		if(0==--numh[h[u]])break;
	// 		curEdges[u]=0;

	// 		for(tmp=numNode,i=0;i<networkNodeGroup[u].getEdgeSize();i++)
	// 		{
	// 			if(networkNodeGroup[u].getBandWidth(i))
	// 				tmp=(tmp<h[networkNodeGroup[u].getToIndexNode(i)]?tmp:h[networkNodeGroup[u].getToIndexNode(i)]);
	// 		}
	// 		h[u]=tmp+1;
	// 		++numh[h[u]];
	// 		if(u!=start)
	// 			u=pre[u];
	// 	}

	// }
// cout<<"P12----------------"<<endl;
	//delete tmp networkNodeGroup

	//sort flowlib according to distance
	sortFlow();

	//update map from Edge to path index
	// if(mapEdgeRoute.size()==0)
	// {
	// 	cout<<"mapEdgeRoute not initialized"<<endl;
	// }
	for(size_t indexFlow=0;indexFlow<flowLib.size();indexFlow++)
	{
		Flow *tmpflow=flowLib[indexFlow];
		vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
		for(size_t i=0;i<path.size()-1;i++)
		{
			mapEdgeRoute[make_pair(path[i]->first,path[i+1]->first)].push_back(indexFlow);
			// mapEdgeRoute[path[i]->first][path[i+1]->first].push_back(indexFlow);
		}
	}

	// cout<<"P2----------------"<<endl;
	//update rest flow
	if(flowLibRest.size()!=0)
		flowLibRest.clear();
	for(size_t i=0;i<flowLib.size();i++)
	{
		flowLibRest.push_back(flowLib[i]->getRestFlow());
	}

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
	cout<<"BFS: "<<endl;
	for(size_t i=0;i<_networkNodeGroup[end].getEdgeSize();i++)
	{		
		nodeQueue.push(_networkNodeGroup[end].getToIndexNode(i));
		visitNode[_networkNodeGroup[end].getToIndexNode(i)]=1;
	}
	

	size_t loopTime=0;
	while(!nodeQueue.empty())
	{
		size_t idx=nodeQueue.front();
		
		nodeQueue.pop();
		long int _restFlow=(long int)(sapV(_networkNodeGroup,networkInfo,idx,end)-demand);
		// long int _restFlow=1;
		restFlowGraph[idx]=_restFlow;
		sortIndexRestFlow.push_back(idx);
		cout<<idx<<"<"<<_restFlow<<"> ";
		if(_restFlow>0)
		{
			for(size_t i=0;i<_networkNodeGroup[idx].getEdgeSize();i++)
			{
				if(visitNode[_networkNodeGroup[idx].getToIndexNode(i)]==0)
				{
					nodeQueue.push(_networkNodeGroup[idx].getToIndexNode(i));
					visitNode[_networkNodeGroup[idx].getToIndexNode(i)]=1;
				}
			}
		}
		loopTime++;
		if(loopTime>20)
			break;

	}
	cout<<endl;
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

	// indexMaxRestFlow=sortIndexRestFlow[1];//not include the end
	// maxRestFlow=restFlowGraph[indexMaxRestFlow];
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


	//if the start point include end
	for(size_t i=0;i<_start.size();i++)
	{
		if(_start[i]==end)
		{
			Flow *tmp=new Flow(end,end);
			tmp->editFlow(99999);
			tmp->pushPair(end,0);
			flowLib.push_back(tmp);

			if(flowLibRest.size()!=0)
				flowLibRest.clear();
			for(size_t i=0;i<flowLib.size();i++)
			{
				flowLibRest.push_back(flowLib[i]->getRestFlow());
			}

			return 0;
		}

	}
		// cout<<"P1----------------"<<endl;
		
	//copy tmp networkNodeGroup
	vector<NetworkNode> networkNodeGroup;
	for (size_t i=0;i<networkInfo.getNumNode();i++)
	{
		NetworkNode tmp(_networkNodeGroup[i]);
		networkNodeGroup.push_back(tmp);
	}

	//construct a total start, connecting to all start.
	NetworkNode tmpNode;
	// cout<<"start: ";
	// for(size_t i=0;i<_start.size();i++)
	// {
	// 	cout<<_start[i]<<" ";
	// }
	// cout<<endl;
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

	
	// cout<<"augment route: "<<endl;
	while(h[start]<numNode)
	{
		if(u==end)
		{
			cur_flow=9999;


			static size_t routeIndex=0;
			routeIndex++;

			Flow *tmp=new Flow(start,end);

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
					// cout<<i<<"<"<<curEdges[i]<<">"<<" ";

					tmp->pushPair(i,curEdges[i]);
					tmp->editFlowCostUnit(networkNodeGroup[i].getCostUnit(curEdges[i]));
				}

			}

			// cout<<end<<"<"<<0<<">"<<" ";
			tmp->pushPair(end,0);
			// cout<<endl;

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
// cout<<"P12----------------"<<endl;
	//delete tmp networkNodeGroup

	//sort flowlib according to distance
	sortFlow();

	//update map from Edge to path index
	// if(mapEdgeRoute.size()==0)
	// {
	// 	cout<<"mapEdgeRoute not initialized"<<endl;
	// }
	for(size_t indexFlow=0;indexFlow<flowLib.size();indexFlow++)
	{
		Flow *tmpflow=flowLib[indexFlow];
		vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
		for(size_t i=0;i<path.size()-1;i++)
		{
			mapEdgeRoute[make_pair(path[i]->first,path[i+1]->first)].push_back(indexFlow);
			// mapEdgeRoute[path[i]->first][path[i+1]->first].push_back(indexFlow);
		}
	}

	// cout<<"P2----------------"<<endl;
	//update rest flow
	if(flowLibRest.size()!=0)
		flowLibRest.clear();
	for(size_t i=0;i<flowLib.size();i++)
	{
		flowLibRest.push_back(flowLib[i]->getRestFlow());
	}

	return 0;
}

size_t ConsNode::sapsV(NetworkNode _networkNodeGroup[],NetworkInfo networkInfo,vector<size_t>& _start, size_t end)
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
	// cout<<"start: ";
	// for(size_t i=0;i<_start.size();i++)
	// {
	// 	cout<<_start[i]<<" ";
	// }
	// cout<<endl;
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

	// cout<<"augment route: "<<endl;
	while(h[start]<numNode)
	{
		if(u==end)
		{
			cur_flow=9999;


			static size_t routeIndex=0;
			routeIndex++;


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


			for(size_t i=start;i!=end;i=networkNodeGroup[i].getToIndexNode(curEdges[i]))
			{
				networkNodeGroup[i].editBandWidth(curEdges[i],-cur_flow);
				size_t antiNode=networkNodeGroup[i].getToIndexNode(curEdges[i]);
				size_t antiEdge=networkNodeGroup[i].getAntiEdgeIndex(curEdges[i]);
				networkNodeGroup[antiNode].editBandWidth(antiEdge,cur_flow);

					//verify the route
					// cout<<i<<"<"<<curEdges[i]<<">"<<" ";


			}

			// cout<<end<<"<"<<0<<">"<<" ";

			// cout<<endl;


			


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
	

	return flow_ans;


}

int ConsNode::initMapEdgeRoute(size_t _numNode)
{
	// if (mapEdgeRoute.size()==0)
	// {
	// 	vector<vector<size_t> >tmp;
	// 	tmp.resize(_numNode);
	// 	mapEdgeRoute.resize(_numNode,tmp);
	// }
	return 0;
}

int ConsNode::regulate(vector<pair<size_t, size_t> >& overLoadEdge, vector<long int>& overLoad,int& regulated)/////here
{
	regulated=-1;
	//test whether the overload edge is in use
	if(flowUsed.size()==0)
	{
		// cout<<"No route is sat!"<<endl;
		return -1;
	}

	int edgeInUsed=-1;
	for(size_t i=0;i<overLoadEdge.size();i++)
	{
		for(size_t j=0;j<flowUsed.size();j++)
		{
			if(searchEdgeInFlow(overLoadEdge[i],flowUsed[j])==0)
			{
				edgeInUsed=0;
			}
		}
	}
	if(edgeInUsed==-1)
	{
		// cout<<"Edge overload not in use"<<endl;
		return -1;
	}

	// cout<<"**************************************\n";


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


	// //show flowUsed
	// cout<<"flowUsed: \n";
	// for(size_t i=0;i<flowUsed.size();i++)
	// {

	// 	cout<<"["<<i<<"] ";
	// 	Flow *tmpflow=flowLib[flowUsed[i].first];
	// 	vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
	// 	for(size_t j=0;j<path.size();j++)
	// 	{
	// 		cout<<path[j]->first<<" ";
	// 	}
	// 	cout<<"("<<flowUsed[i].second<<") <"<<flowLibRest[flowUsed[i].first]<<">"<<endl;
	// }
	// cout<<endl;
	// cout<<"("<<overLoadEdge[0].first<<" "<<overLoadEdge[0].second<<" "<<overLoad[0]<<")"<<endl;



	
	// cout<<"edge over flow before remove: \n";
	// for(size_t i=0;i<overLoadEdge.size();i++)
	// {
	// 	cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoad[i]<<endl;
	// }
	// cout<<endl;

	for(size_t i=0;i<overLoadEdge.size();i++)
	{
		// vector<size_t>& flowIndex=mapEdgeRoute[overLoadEdge[i].first][overLoadEdge[i].second];
		vector<size_t>& flowIndex=mapEdgeRoute[make_pair(overLoadEdge[i].first,overLoadEdge[i].second)];
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

	if(usedNum[usedIndex[0]]>0)//if include overload edge, it will regulate 
		regulated=0;

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
		long int maxFlow=0;
		// size_t minFlow=99999;
		for(size_t j=0;j<overLoadEdge.size();j++)
		{
			long int tmpFlow=0;
			if(countUsed[j][usedIndex[i]]==1)
			{
				if(overLoadRest[j]>=(long int)flowUsed[usedIndex[i]].second)
				{
					tmpFlow=flowUsed[usedIndex[i]].second;
				}
				else
				{
					tmpFlow=overLoadRest[j];
				}
			}
			if(tmpFlow>maxFlow)
				maxFlow=tmpFlow;
			// if(tmpFlow<minFlow)
			// 	minFlow=tmpFlow;
		}

		// cout<<"maxFlow: "<<maxFlow<<endl;
		for(size_t j=0;j<overLoad.size();j++)
		{
			if(countUsed[j][usedIndex[i]]==1)
				overLoadRest[j]-=maxFlow;
				// overLoadRest[j]-=minFlow;
		}

		minusFlow+=maxFlow;
		// minusFlow+=minFlow;
		tmpFlowUsed[usedIndex[i]].second-=maxFlow;
		tmpRestFlow[usedIndex[i]]+=maxFlow;
		// tmpFlowUsed[usedIndex[i]].second-=minFlow;
		// if(tmpFlowUsed[usedIndex[i]].second==0)
		// 	tmpFlowUsed[usedIndex[i]];

		//if all overload edge is fixed, quit
		overloadOrNot=-1;
		for(size_t j=0;j<overLoad.size();j++)
		{
			if(overLoadRest[j]>0)
				overloadOrNot=1;
		}

		if(overloadOrNot==-1)
			break;

	}

	// cout<<"edge over flow after remove: \n";
	// for(size_t i=0;i<overLoadEdge.size();i++)
	// {
	// 	cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoadRest[i]<<endl;
	// }
	// cout<<endl;


	// cout<<"overload fixed: ";
	// if(overloadOrNot==-1)
	// 	cout<<"Yes";
	// else
	// 	cout<<"No";
	// cout<<endl;

	// cout<<"flow need to compensate: ";
	// 	cout<<minusFlow;
	// cout<<endl<<endl;

	//add edge (can't guarantee the added edge is not the overload one, because the demand should be satisfied first)
	for(size_t i=0;i<flowLib.size();i++)//tmpRest is not updated
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

	// cout<<"edge over flow after add edge: \n";
	// for(size_t i=0;i<overLoadEdge.size();i++)
	// {
	// 	cout<<"("<<overLoadEdge[i].first<<","<<overLoadEdge[i].second<<"): "<<overLoadRest[i]<<endl;
	// }
	// cout<<endl;

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

	// //show tmpFlowUsed
	// cout<<"tmpFlowUsed: \n";
	// for(size_t i=0;i<tmpFlowUsed.size();i++)
	// {

	// 	cout<<"["<<i<<"] ";
	// 	Flow *tmpflow=flowLib[tmpFlowUsed[i].first];
	// 	vector<pair<size_t,size_t>* > &path=*(tmpflow->getPath());
	// 	for(size_t j=0;j<path.size();j++)
	// 	{
	// 		cout<<path[j]->first<<" ";
	// 	}
	// 	cout<<"("<<tmpFlowUsed[i].second<<") <"<<tmpRestFlow[tmpFlowUsed[i].first]<<">"<<endl;
	// }

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



	// cout<<"++++++++++++++++++++++++++++++++++++++\n";
	
	//for those overload is negative, they don't regulation! Because the bandwidth is not used fully.
	for(size_t i=0;i<overLoad.size();i++)
	{
		if(overLoad[i]<0)
			overLoad[i]=0;
	}

	for(size_t i=0;i<overLoad.size();i++)
	{
		if(overLoad[i]>0)
			return -1;
	}
	return 0;
}

int ConsNode::searchEdgeInFlow(pair<size_t, size_t> &Edge, pair<size_t,size_t> &flow)
{
	vector<size_t>& flowIndex=mapEdgeRoute[Edge];//mapEdgeRoute[Edge.first][Edge.second];
	for(size_t i=0;i<flowIndex.size();i++)
	{
		if(flow.first==flowIndex[i])
			return 0;
	}

	return -1;
}

long int ConsNode::getEdgeLoad(pair<size_t, size_t> indexEdge)
{
	long int _bandWidth=0;
	// vector<size_t>&indexFlowLib=mapEdgeRoute[indexEdge.first][indexEdge.second];
	vector<size_t>&indexFlowLib=mapEdgeRoute[make_pair(indexEdge.first,indexEdge.second)];
	for(size_t i=0;i<indexFlowLib.size();i++)
	{
		if((long int)flowLib[indexFlowLib[i]]->getRestFlow()!=flowLibRest[indexFlowLib[i]])
		{
			_bandWidth+=(flowLib[indexFlowLib[i]]->getRestFlow()-flowLibRest[indexFlowLib[i]]);
		}
	}
	return _bandWidth;
}


int ConsNode::sortFlow()
{
	// //sort by path length
	// for (size_t i=0;i<flowLib.size()-1;i++)
	// {
	// 	for(size_t j=i+1;j<flowLib.size();j++)
	// 	{
	// 		if(flowLib[i]->getLength()>flowLib[j]->getLength())
	// 		{
	// 			Flow* tmp=flowLib[i];
	// 			flowLib[i]=flowLib[j];
	// 			flowLib[j]=tmp;
	// 		}
	// 	}
	// }

	//sort by path cost
	for (size_t i=0;i<flowLib.size()-1;i++)
	{
		for(size_t j=i+1;j<flowLib.size();j++)
		{
			if(flowLib[i]->getFlowCostUnit()>flowLib[j]->getFlowCostUnit())
			{
				Flow* tmp=flowLib[i];
				flowLib[i]=flowLib[j];
				flowLib[j]=tmp;
			}
		}
	}
	return 0;
}

// long int ConsNode::getBandWidthUsed(size_t i, size_t j)
// {
// 	long int sumBandWidth=0;
// 	for (size_t k=0;k<mapEdgeRoute[i][j].size();k++)
// 		sumBandWidth+=mapEdgeRoute[i][j][k];

// 	return sumBandWidth;
// }

int ConsNode::selectFlow()
{
	int satisfied=0;
	size_t bandWidth=0;

	// cout<<"fuck"<<endl;

	for(size_t i=0;i<flowLib.size();i++)
	{
		if(demand-bandWidth>flowLib[i]->getRestFlow())//use out flow on the path
		{
			bandWidth+=flowLib[i]->getRestFlow();
			pair<size_t,size_t>tmp(i,flowLib[i]->getRestFlow());
			flowUsed.push_back(tmp);
			flowLibRest[i]=0;
		}
		else//use part of flow on the path
		{

			pair<size_t,size_t>tmp(i,demand-bandWidth);
			flowUsed.push_back(tmp);			
			flowLibRest[i]=flowLib[i]->getRestFlow()-(demand-bandWidth);
			
			bandWidth=demand;
			
			break;
		}
	}
	// cout<<"fuck1"<<endl;
	// cout<<"flowUsed size: "<<flowUsed.size()<<endl;
	// cout<<"demand "<<demand<<"bandWidth "<<bandWidth<<endl;
	if(bandWidth!=demand)
	{
		if(flowUsed.size()!=0)
			flowUsed.clear();
		for(size_t i=0;i<flowLib.size();i++)
		{
			flowLibRest[i]=flowLib[i]->getRestFlow();
		}
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

int ConsNode::clearRoute()
{
	while(flowLib.size())//clear flow lib
	{
		Flow* tmp=flowLib.back();
		flowLib.pop_back();
		delete tmp;
	}
	flowLibRest.clear();//clear flow rest
	flowUsed.clear();//clear flow used
	size_t _nodeNum=mapEdgeRoute.size();//clear flow map
	mapEdgeRoute.clear();
	initMapEdgeRoute(_nodeNum);

	return 0;
}

int ConsNode::constructServerPool()
{
	//map push node in the path to serverPool
	for(size_t i=0;i<flowLib.size();i++)
	{
		vector<pair<size_t,size_t>* > &path=*(flowLib[i]->getPath());
		for(size_t j=0;j<path.size();j++)
		{
			if(serverFlow.find(path[j]->first)==serverFlow.end())
			{
				serverFlow[path[j]->first]=flowLib[i]->getRestFlow();
				serverPool.push_back(path[j]->first);
			}
			else
				serverFlow[path[j]->first]+=flowLib[i]->getRestFlow();
		}
	}

	//sort server pool according to flow
	for(size_t i=0;i<serverPool.size()-1;i++)
	{
		for(size_t j=i+1;j<serverPool.size();j++)
		{
			if(serverFlow[serverPool[i]]<serverFlow[serverPool[j]])
			{
				size_t tmp=serverPool[i];
				serverPool[i]=serverPool[j];
				serverPool[j]=tmp;
			}
		}
	} 

	// //verify
	// for(size_t i=0;i<serverPool.size();i++)
	// 	cout<<serverPool[i]<<","<<serverFlow[serverPool[i]]<<" ";
	// cout<<endl;

	
	return 0;
}
//**************************************************Class Route**************************************************
Route::Route()
{
	cost=0;
}

Route::~Route()
{
	while(routeG.size())
	{
		vector<size_t>* tmp=routeG.back();
		routeG.pop_back();
		delete tmp;
	}

}

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

size_t Route::costCal(NetworkInfo networkInfo)
{
	cost=0;
	map<size_t,int>serverUsed;

	for(size_t i=0;i<routeG.size();i++)
	{
		vector<size_t>& _route=*(routeG[i]);
		size_t costUnitSum=0;
		for (size_t j=0;j<_route.size()-3;j++)
		{
			costUnitSum+=networkInfo.getMatrixCost(_route[j],_route[j+1]);
		}
		cost+=costUnitSum*_route.back();
		serverUsed[_route.front()]=1;


	}

	cost+=networkInfo.getCostServer()*serverUsed.size();
	serverNum=serverUsed.size();
	cout<<"num of server: "<<serverNum<<endl;

	return cost;
}

int Route::serverUsedCal(vector<size_t>& serverPos)
{
	if(serverInUse.size()==0)
	{
		for(size_t i=0;i<routeG.size();i++)
		{
			vector<size_t>& _route=*(routeG[i]);
			
			if(serverInUse.find(_route[_route.size()-3])==serverInUse.end())
				serverInUse[_route.front()]=_route.back();
			else
				serverInUse[_route.front()]+=_route.back();

		}

	}

	if(serverPos.size())
		serverPos.clear();

	map<size_t,size_t>::iterator iter=serverInUse.begin();
	for(;iter!=serverInUse.end();iter++)
	{
		// cout<<iter->first<<":"<<iter->second<<endl;
		serverPos.push_back(iter->first);
	}

	//sort
	for(size_t i=0;i<serverPos.size()-1;i++)
	{
		for(size_t j=0;j<serverPos.size()-1;j++)
		{
			if(serverInUse[serverPos[i]]>serverInUse[serverPos[j]])
			{
				size_t tmp=serverPos[i];
				serverPos[i]=serverPos[j];
				serverPos[j]=tmp;
			}
		}
	}

	// //verify
	// cout<<"server in used"<<endl;
	// for(size_t i=0;i<serverPos.size()-1;i++)
	// {
	// 	cout<<serverPos[i]<<" "<<serverInUse[serverPos[i]]<<endl;
	// } 
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
	while(path.size())
	{
		pair<size_t, size_t>* tmp=path.back();
		path.pop_back();
		delete tmp;
	}
}
//**************************************************Class EdgeMatrix**************************************************
EdgeMatrix::EdgeMatrix(size_t _numNode, size_t _numCons):numNode(_numNode),numCons(_numCons)
{
	vector<long int>tmp;
	tmp.resize(_numNode,0);
	matrix.resize(_numNode,tmp);
	matrixRef.resize(_numNode,tmp);

	// xCIJ.resize(_numCons,matrixRef);
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

void EdgeMatrix::printMatrix()//////////////////////////////////here
{
	for(size_t i=0;i<matrix.size();i++)
	{
		cout<<i<<": ";
		for(size_t j=0;j<matrix.size();j++)
			cout<<j<<"<"<<matrix[i][j]<<"> ";
		cout<<endl;
	}
}

int EdgeMatrix::checkCons(ConsNode consNodeGroup[], NetworkInfo networkInfo,vector<pair<size_t, size_t> >& overLoadEdge, vector<long int>& overLoad)
{
	//path: first: nodeIndex second: edgeIndex
	resetMatrix();
	for (size_t i=0;i<networkInfo.getNumCons();i++)
	{
		vector<Flow* >& _flowLib=*(consNodeGroup[i].getFlowLib());
		vector<pair<size_t,size_t> >& _flowUsed=*(consNodeGroup[i].getFlowUsed());
		for(size_t j=0;j<_flowUsed.size();j++)
		{
			vector<pair<size_t,size_t>* >&path=*(_flowLib[_flowUsed[j].first]->getPath());

			for(size_t k=0;k<path.size()-1;k++)
			{
				editEdge(path[k]->first,path[k+1]->first,-_flowUsed[j].second);
			}
		}
		
	}

	if (checkOverLoad(overLoadEdge,overLoad)==0)
			return 0;
	else
			return -1;
}

int EdgeMatrix::editEdge(size_t i, size_t j, long int deltaBandWidth)
{
	matrix[i][j]+=deltaBandWidth; 
	return 0;
}

int EdgeMatrix::loadEdge(size_t i, size_t j, long int deltaBandWidth)
{
	matrixRef[i][j]+=deltaBandWidth; 
	pair<size_t, size_t> tmp(i,j);
	checkList.push_back(tmp);
	return 0;
}

int EdgeMatrix::checkOverLoad(vector<pair<size_t, size_t> >& overLoadEdge, vector<long int>& overLoad)
{
	int overLoadFlag=0;
	if (overLoadEdge.size()>0)
	{
		overLoadEdge.clear();
	}
	if(overLoad.size()>0)
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
			overLoadEdge.push_back(tmp);
			overLoad.push_back(-matrix[x][y]);
			overLoadFlag=-1;
		}



	}
	return overLoadFlag;
}

// int EdgeMatrix::insertFlow(vector<size_t >& flow, size_t consIndex)
// {
// 	long int deltaBandWidth=flow.back();
// 	for (size_t i=0;i<flow.size()-3;i++)
// 	{
// 		editEdge(flow[i],flow[i+1],-deltaBandWidth);
// 		xCIJ[consIndex][flow[i]][flow[i+1]]+=deltaBandWidth;
// 	}
// 	return 0;
// }

int EdgeMatrix::resetMatrix()
{
	for (size_t i=0;i<matrixRef.size();i++)
		for(size_t j=0;j<matrixRef.size();j++)
		{
			matrix[i][j]=matrixRef[i][j];
		}

	// for(size_t c=0;c<numCons;c++)
	// 	for(size_t i=0;i<numNode;i++)
	// 		for(size_t j=0;j<numNode;j++)
	// 			xCIJ[c][i][j]=0;
	return 0;
}

// int EdgeMatrix::update(ConsNode consNodeGroup[],size_t numCons)
// {
// 	resetMatrix();
// 	for(size_t i=0;i<numCons;i++)
// 	{
// 		for(size_t j=0;j<consNodeGroup[i].getNumFlow();j++)
// 		{
// 			vector<size_t>* route=new vector<size_t>();
// 			consNodeGroup[i].popRoute(j,route);
// 			insertFlow(*route,i);
// 			delete route;
// 		}
// 	}
// 		return 0;
// }

