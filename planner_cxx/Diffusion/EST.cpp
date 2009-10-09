/*
 * EST.cpp
 *
 *  Created on: Sep 16, 2009
 *      Author: jmainpri
 */

#include "EST.hpp"

using namespace std;
using namespace tr1;

EST::EST(WorkSpace* WS) :
	TreePlanner(WS)
{

}

EST::~EST()
{

}

/**
 * Ckecks Stop Conditions
 */
bool EST::checkStopConditions()
{
	if(_Graph->getNbNode() > 100)
	{
//		cout << "Number of node reached 100" << endl;
//		return true;
	}

	if(TreePlanner::checkStopConditions())
	{

		return true;
	}

 return false;

}

/**
 * Checks out the preconditions
 */
bool EST::preConditions()
{
	if(TreePlanner::preConditions())
	{
		if (ENV.getBool(Env::expandToGoal))
		{
			LocalPath direct(_Start->getConfiguration(), _Goal->getConfiguration());
			if (direct.getValid())
			{
				cout << "Direct connection" << endl;
				return false;
			}
		}

		if (ENV.getBool(Env::expandToGoal))
		{
			if( connectNodeToCompco(
					this->getStart(),
					this->getGoal()) )
				return false;


		}
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Initializes an RRT Planner
 */
int  EST::init()
{

	int added = TreePlanner::init();

	_SortedNodes = _Start->getSortedNodes();
	addNodeToSet(_Start);
	_Start->setSortedNodes(_SortedNodes);

//	if(ENV.getBool(Env::biDir))
//	{
		_SortedNodes = _Goal->getSortedNodes();
		addNodeToSet(_Goal);
		_Goal->setSortedNodes(_SortedNodes);
//	}

	_Expan = new TransitionExpansion(_Graph);

	setInit(true);

	return added;
}

class myNodeComparator {

public:
	EST* ptrNode;

	bool operator()(Node* ptrNode1,Node* ptrNode2) {

		return (
				ptrNode1->getSelectCost() <
				ptrNode2->getSelectCost() );
	}

} nodeCompObject;

/**
 * Adds a node to the sorted set
 */
void EST::addNodeToSet(Node* extentionNode)
{
//	cout << "addNodeToSet" << endl;

	_SortedNodes.push_back(extentionNode);

	sort(
			_SortedNodes.begin(),
			_SortedNodes.end(),
			nodeCompObject);

//	if(_SortedNodes.size()==0)
//	{
//		_SortedNodes.push_back(extentionNode);
//		return;
//	}
//
//	vector<Node*>::iterator it;
//
//	for( it=_SortedNodes.begin();
//		 it != _SortedNodes.end();
//		 it++ )
//	{
//		Node* node = *it;
//		if( extentionNode->getCost() < node->getSelectCost() )
//		{
//			_SortedNodes.insert(it,extentionNode);
//			return;
//		}
//	}
}

/**
 * Three phases One Step Expansion
 *  - Node
 *  - Direction
 *  - Process
 *
 *  @param fromComp the component which is expanded
 *  @param toComp the goal component
 */
int EST::expandOneStep(Node* fromComp, Node* toComp)
{
		Node* extentionNode;
		Node* expansionNode;
		shared_ptr<Configuration> directionConfig;

		_SortedNodes = fromComp->getSortedNodes();


		// get node for expansion
		expansionNode = _Expan->getExpansionNode(_SortedNodes);

		// get direction
		directionConfig = _Expan->getExpansionDirection(expansionNode,toComp);

		// expansion
		int nbOfNodesAdded=0;

		extentionNode = _Expan->expandProcessEST(expansionNode,directionConfig,
				nbOfNodesAdded);

		if( nbOfNodesAdded>0 )
		{
			addNodeToSet(extentionNode);
			fromComp->setSortedNodes(_SortedNodes);
		}
		else
		{
			expansionNode->setExpandFailed();
		}

		return nbOfNodesAdded;
//	}
}


/**
 * costConnectNodeToComp
 * Try to connect a node to a given component
 * taking into account the fact that the space
 * is a cost space
 * @return: TRUE if the node and the componant have
 * been connected.
 */
bool EST::connectNodeToCompco(Node* node, Node* compNode)
{
	int SavedIsMaxDis = FALSE;
	Node* node2(NULL);

	switch(p3d_GetNodeCompStrategy()) {
	case K_NEAREST_NODE_COMP:
		/*Connect randomly to one of the k nearest
      nodes of the componant */
		/*todo*/
		break;
	case NEAREST_NODE_COMP:
	default:
		SavedIsMaxDis =  p3d_GetIsMaxDistNeighbor();
		p3d_SetIsMaxDistNeighbor(FALSE);

		node2 = _Graph->nearestWeightNeighbour(compNode,
				node->getConfiguration(),
				false,
				p3d_GetDistConfigChoice());

		p3d_SetIsMaxDistNeighbor(SavedIsMaxDis);

		LocalPath path(node->getConfiguration(),node2->getConfiguration());

		if(!ENV.getBool(Env::CostBeforeColl))
		{
			if( path.getValid() )
			{
				if( path.length() <= _Expan->step() )
				{
					int nbCreatedNodes=0;

					_Expan->addNode(node,path,1.0,node2,nbCreatedNodes);
					cout << "Path Valid Connected" << endl;
					return true;
				}

				if( _Expan->expandToGoal(
						node,
						node2->getConfiguration()))
				{
					int nbCreatedNodes=0;

					_Expan->addNode(node,path,1.0,node2,nbCreatedNodes);
					cout << "attempting connect " << node->getConfiguration()->cost() << " to " << node2->getConfiguration()->cost() << endl;
					return true;
				}
			}
			return false;
		}
		else
		{
				if( path.length() <= _Expan->step() )
				{
					int nbCreatedNodes=0;

					if( path.getValid() )
					{
						_Expan->addNode(node,path,1.0,node2,nbCreatedNodes);
						cout << "Path Valid Connected" << endl;
						return true;
					}
					else
					{
						return false;
					}
				}
				if( _Expan->expandToGoal(
						node,
						node2->getConfiguration()))
				{
					if( path.getValid() )
					{
						int nbCreatedNodes=0;
						_Expan->addNode(node,path,1.0,node2,nbCreatedNodes);
						cout << "attempting connect " << node->getConfiguration()->cost() << " to " << node2->getConfiguration()->cost() << endl;
						return true;
					}
					else
					{
						return false;
				}
			}

		}
	}
}