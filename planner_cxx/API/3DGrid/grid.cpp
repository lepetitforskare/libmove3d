#include "grid.h"

using namespace std;

#include <iostream>


/*!
 * \brief Constructor
 *
 * \param X number of cells
 * \param Y number of cells
 * \param Z number of cells
 */
Grid::Grid()
{

}

/*!
 * \brief Destructor
 */
Grid::~Grid()
{
    for(int i=0;i<_cells.size();i++)
    {
        delete _cells.at(i);
    }
}

/*!
 * \brief Initializes the grid with a pace
 *
 * \param vector int size (number of cells in X, Y, Z)
 * \param vector envSize XMin Xmax YMin YMax ZMin ZMax
 */
Grid::Grid( vector<int> size, vector<double> envSize )

{
    _nbCellsX = size[0];
    _nbCellsY = size[1];
    _nbCellsZ = size[2];

    _cellSize.push_back( (envSize.at(1) - envSize.at(0)) / _nbCellsX );
    _cellSize.push_back( (envSize.at(3) - envSize.at(2)) / _nbCellsY );
    _cellSize.push_back( (envSize.at(5) - envSize.at(4)) / _nbCellsZ );

    _originCorner.push_back(envSize.at(0));
    _originCorner.push_back(envSize.at(2));
    _originCorner.push_back(envSize.at(4));

//    cout << "_originCorner[0] = " << _originCorner.at(0) <<  endl;
//    cout << "_originCorner[1] = " << _originCorner.at(1) <<  endl;
//    cout << "_originCorner[2] = " << _originCorner.at(2) <<  endl;
}

/*!
 * \brief Initializes the grid with a certain pace
 *
 * \param double pace : sizes of the squared cells IMPORTANT Cells are squared
 * \param vector envSize XMin Xmax YMin YMax ZMin ZMax
 */
Grid::Grid( double pace, vector<double> envSize)
{
    for(int i= 0; i< envSize.size() ; i++)
    {
        cout << envSize.at(i) << " ";
    }
    cout << endl;

    if(((int)pace) != 0 )
    {
        if( ( ((int) (envSize.at(1) - envSize.at(0))) % (int)pace ) != 0 )
        {
            cout << "Grid Warning : not good X disctretization " << endl;
        }

        if( ( ((int) (envSize.at(3) - envSize.at(2))) % (int)pace ) != 0 )
        {
            cout << "Grid Warning : not good Y disctretization " << endl;
        }

        if( ( ((int) (envSize.at(5) - envSize.at(4))) % (int)pace ) != 0 )
        {
            cout << "Grid Warning : not good Z disctretization " << endl;
        }
    }

//    _cellSize.push_back( (envSize.at(1) - envSize.at(0))/pace );
//    _cellSize.push_back( (envSize.at(3) - envSize.at(2))/pace );
//    _cellSize.push_back( (envSize.at(5) - envSize.at(4))/pace );

    _cellSize.push_back( pace );
    _cellSize.push_back( pace );
    _cellSize.push_back( pace );

    _nbCellsX = (envSize.at(1) - envSize.at(0)) / pace;
    _nbCellsY = (envSize.at(3) - envSize.at(2)) / pace;
    _nbCellsZ = (envSize.at(5) - envSize.at(4)) / pace;

    cout << " _nbCellsX = " << _nbCellsX << endl;
    cout << " _nbCellsY = " << _nbCellsY << endl;
    cout << " _nbCellsZ = " << _nbCellsZ << endl;

    _originCorner.push_back(envSize.at(0));
    _originCorner.push_back(envSize.at(2));
    _originCorner.push_back(envSize.at(4));

    cout << "_originCorner[0] = " << _originCorner.at(0) <<  endl;
    cout << "_originCorner[1] = " << _originCorner.at(1) <<  endl;
    cout << "_originCorner[2] = " << _originCorner.at(2) <<  endl;

}

/*!
 * \brief Creates All Cells
 *
 * \param vector envSize XMin Xmax YMin YMax ZMin ZMax
 */
void Grid::createAllCells()
{

    int _nbCells = _nbCellsX * _nbCellsY * _nbCellsZ;

    _cells.resize(_nbCells);

    int x=0;
    int y=0;
    int z=0;

    for(int i = 0; i < _nbCells; i++)
    {
//        cout << "("<< x << "," << y << "," << z << ")" << endl;

        Cell* ptrCell = createNewCell(i,x,y,z);
        _cells[i] = ptrCell;

        x++;
        if( x >= _nbCellsX )
        {
            y++;
            x=0;
            if( y >= _nbCellsY )
            {
                z++;
                y=0;
                if( z > _nbCellsZ )
                {
                    cout << "Grid : Error Size of Grid " << endl;
                    return;
                }
            }
        }
    }
//    cout << "Finished"<< endl;
}

/*!
 * \brief Get Cell
 *
 * \param index
 */
Cell* Grid::getCell(int i)
{
    return _cells[i];
}

/*!
 * \brief Retruns the Cell at (x,y,z)
 *
 * \param integers x, y, z
 */
Cell* Grid::getCell(int x, int y, int z)
{
    if(x<0 || x >= _nbCellsX)
    {
        cout << "Grid Error : out of bands"<< endl;
    }
    if(y<0 || y >= _nbCellsY)
    {
        cout << "Grid Error : out of bands"<< endl;
    }
    if(z<0 || z >= _nbCellsZ)
    {
        cout << "Grid Error : out of bands"<< endl;
    }

    return _cells[ x + y*_nbCellsX + z*_nbCellsX*_nbCellsY ];
}

/*!
 * \brief Get Cell
 *
 * \param index
 */
Cell* Grid::getCell(vector<int> cell)
{
    return getCell(cell[0],cell[1],cell[2]);
}

/*!
 * \brief Get Cell in 3D Grid
 *
 * \param index
 */
Cell* Grid::getCell(vector<double> pos)
{
    double x = (int)((pos[0]-_originCorner[0])/_cellSize[0]);
    double y = (int)((pos[1]-_originCorner[1])/_cellSize[1]);
    double z = (int)((pos[2]-_originCorner[2])/_cellSize[2]);

//    cout << "( "<<x<<" , "<<y<<" , "<<z<<" ) "<< endl;

    if( x>=_nbCellsX ||  y>=_nbCellsY || z>=_nbCellsZ || x<0 || y<0 || z<0 )
    {
       cout << "Grid:: OutBands " << endl;
       return 0x0;
     }

    return getCell(x,y,z);
}

/*!
 * \brief Get place in grid
 *
 * \param index
 */
vector<int> Grid::getCellCoord(Cell* ptrCell)
{
   vector<int> coord;

   int i = ptrCell->getIndex();

   coord[0] = (i/1) % 3 - 1 ; // x
   coord[1] = (i/3) % 3 - 1 ; // y
   coord[2] = (i/9) % 3 - 1 ; // z

   return coord;
}


/*!
 * \brief Virtual function that creates a new Cell
 *
 * \param integer index
 * \param integer x
 * \param integer y
 * \param integer z
 */
Cell* Grid::createNewCell(int index, int x, int y, int z )
{
    if (index == 0)
    {
        return new Cell( 0, _originCorner , this );
    }
    Cell* newCell = new Cell( index, computeCellCorner(x,y,z) , this );
    vector<double> corner = newCell->getCorner();
//    cout << " = (" << corner[0] <<"," << corner[1] << "," << corner[2] << ")" << endl;
    return newCell;
}

/*!
 * \brief Computes the corner of a cell
 *
 * \param integer index
 */
vector<double> Grid::computeCellCorner(int x, int y, int z)
{
    vector<double> corner(3);

    corner[0] = _originCorner[0] + x*_cellSize[0];
    corner[1] = _originCorner[1] + y*_cellSize[1];
    corner[2] = _originCorner[2] + z*_cellSize[2];

//    cout << " = (" << x <<"," << y << "," << z << ")" << endl;
//    cout << " = (" << corner[0] <<"," << corner[1] << "," << corner[2] << ")" << endl;

    return corner;
}


/*!
 * \brief Get Number Of Cells
 */
int Grid::getNumberOfCells()
{
    return _cells.size();
}


/*!
 * \brief Get Neighboor Cell
 */
Cell* Grid::getNeighbour( const vector<int>& pos, int i)
{
    if( i<0 || i>26 )
    {
        return 0x0;
    }
    else
    {
        if(i>=13) i++;

        int dx =  (i/1) % 3 - 1 ;
        int dy =  (i/3) % 3 - 1 ;
        int dz =  (i/9) % 3 - 1 ;

//        cout << "( "<<dx<<" , "<<dy<<" , "<<dz<<" ) "<< endl;

        int x = pos[0] + dx ;
        int y = pos[1] + dy ;
        int z = pos[2] + dz ;

        if( x>=_nbCellsX ||  y>=_nbCellsY || z>=_nbCellsZ || x<0 || y<0 || z<0 )
        {
//            cout << "( "<<x<<" , "<<y<<" , "<<z<<" ) "<< endl;
//            cout << "OutBands" << endl;
            return 0x0;
        }
        else
        {
//            cout << "( "<<x<<" , "<<y<<" , "<<z<<" ) "<< endl;
            return getCell(x,y,z);
        }
    }
}

