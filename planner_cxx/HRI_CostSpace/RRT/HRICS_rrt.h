#ifndef HRICS_RRT_H
#define HRICS_RRT_H

#include "../../Diffusion/RRT.hpp"
#include "../Grid/HRICS_Grid.h"

class HRICS_RRT : public RRT
{
public:
    HRICS_RRT(Robot* R, Graph* G);

    /**
      * Sets the grid pointer
      */
    void setGrid(HRICS::Grid* G);

    /**
      * Sets the cell path
      */
    void setCellPath(std::vector<API::Cell*> cellPath);

   /**
     * Initialzation of the plannificator
     * @return the number of node added during the init phase
     */
    virtual int init();

    /**
      * Intents to connect a node to the compco
      */
    bool connectNodeToCompco(Node* node, Node* compNode);

    /**
      * Finds the nearest neighbour in the cell
      * @return Other wise returns NULL
      */
    Node* nearestNeighbourInCell(Node* node, std::vector<Node*> neigbour);

    /**
      * @return the cell in which is the given node
      */
    API::Cell* getCellFromNode(Node* node);

 private:
    HRICS::Grid* _Grid;

};

#endif // HRICS_RRT_H
