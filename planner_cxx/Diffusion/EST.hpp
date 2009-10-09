/*
 * EST.hpp
 *
 *  Created on: Sep 16, 2009
 *      Author: jmainpri
 */

#ifndef EST_HPP_
#define EST_HPP_

#include "Expansion/TransitionExpansion.hpp"
#include "TreePlanner.hpp"

/**
 * Expansive State Space Tree Planner
 */
class EST: public TreePlanner
{

public:
	/**
	 * Constructor from a WorkSpace object
	 * @param WS the WorkSpace
	 */
	EST(WorkSpace* WS);

	/**
	 * Destructor
	 */
	~EST();

	/**
	 * Initialzation of the plannificator
	 * @return the number of node added during the init phase
	 */
	virtual int init();

	/**
	 * Checks out the Stop condition
	 */
	bool checkStopConditions();

	/**
	 * Checks out the preconditions
	 */
	bool preConditions();

	/**
	 * Add node to sorted set
	 */
	void addNodeToSet(Node* extentionNode);

	/**
	 * Three phases One Step Expansion
	 *  - Direction
	 *  - Node
	 *  - Process
	 *
	 *  @param fromComp the component which is expanded
	 *  @param toComp the goal component
	 */
	virtual int expandOneStep(Node* fromComp, Node* toComp);

	/**
	 * EST Connect node using Cost
	 */
	bool connectNodeToCompco(Node* node, Node* compNode);


	/**
	* Returns number of consecutive failure
	* during plannification
	*/

	TransitionExpansion* getExpansion()
		{
			return _Expan;
		};

protected:
	TransitionExpansion* _Expan;
	std::vector<Node*> _SortedNodes;


};

#endif /* EST_HPP_ */