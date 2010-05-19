#include "HRICS_GridState.h"
#include "HRICS_Grid.h"

using namespace std;
using namespace HRICS;

State::State( Vector3i cell , Grid* grid) :
        _Grid(grid)
{
    _Cell = dynamic_cast<Cell*>(grid->getCell(cell));
}

State::State( Cell* cell , Grid* grid) :
        _Cell(cell),
        _Grid(grid)
{

}


vector<API::State*> State::getSuccessors()
{
    vector<API::State*> newStates;
//    newStates.reserve(26);

    for(int i=0;i<26;i++)
    {
        Cell* neigh = dynamic_cast<Cell*>(_Grid->getNeighbour( _Cell->getCoord(), i));
        if( neigh != NULL )
        {
            _Grid->isVirtualObjectPathValid(dynamic_cast<Cell*>(_Cell),neigh);
            newStates.push_back( new State(neigh,_Grid));
        }
    }

    return newStates;
}

bool State::isLeaf()
{
    return false;
}

bool State::equal(API::State* other)
{
    //bool equal(false);
    State* state = dynamic_cast<State*>(other);
    Vector3i pos = _Cell->getCoord();
    for(int i=0;i<3;i++)
    {
        if( pos[i] != state->_Cell->getCoord()[i])
        {
            //            cout << "State::equal false" << endl;
            return false;
        }
    }

    //    cout << "State::equal true" << endl;
    return true;
}

void State::setClosed(std::vector<State*>& closedStates,std::vector<State*>& openStates)
{
    //    cout << "State :: set Closed" <<endl;
    _Cell->setClosed();
}

bool State::isColsed(std::vector<State*>& closedStates)
{
    //    cout << "State :: get Closed" <<endl;
    return _Cell->getClosed();
}

void State::setOpen(std::vector<State*>& openStates)
{
    //     cout << "State :: set open" <<endl;
    _Cell->setOpen();
}


bool State::isOpen(std::vector<State*>& openStates)
{
    //    cout << "State :: get open" <<endl;
    return _Cell->getOpen();
}

void State::reset()
{
    _Cell->resetExplorationStatus();
}

void State::print()
{

}

double State::computeLength(API::State *parent)
{
    State* preced = dynamic_cast<State*>(parent);

    Vector3d pos1 = _Cell->getCenter();
    Vector3d pos2 = preced->_Cell->getCenter();

    double dist = ( pos1 - pos2 ).norm();

//    double cost1 = preced->_Cell->getCost();
    double cost2 = _Cell->getCost();
    double g = preced->g() + /*cost1 +*/ cost2 + dist;

//    cout << "dist = " << dist << endl;
//    cout << "g = " << g << endl;
    return g;
}

double State::computeHeuristic(API::State *parent,API::State* goal)
{
    State* state = dynamic_cast<State*>(goal);

    Vector3d pos1 = state->_Cell->getCenter();
    Vector3d pos2 = _Cell->getCenter();

    double dist=0;

    for(int i=0;i<3;i++)
    {
        dist += (pos1[i]-pos2[i])*(pos1[i]-pos2[i]);;
    }
    return dist;
}
