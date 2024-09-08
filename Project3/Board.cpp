//
//  main.cpp
//  Project3
//
//  Created by Ava Bronkar on 5/5/22.
//

#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    const Game& m_game;
    
    char m_board[MAXROWS][MAXCOLS];
    bool hasBeenBlocked[MAXROWS][MAXCOLS]; //INITIALIZE
    bool hasBeenAttacked[MAXROWS][MAXCOLS];
    
    int timesBeenAttacked[5];
};

BoardImpl::BoardImpl(const Game& g):m_game(g)
{
    for (int rows = 0; rows < m_game.rows(); rows++){
        for (int cols = 0; cols < m_game.cols(); cols++)
            m_board[rows][cols] = '.';
        }
    
    for (int rows = 0; rows < m_game.rows(); rows++){
        for (int cols = 0; cols < m_game.cols(); cols++)
            hasBeenBlocked[rows][cols] = false;
        }
    for (int rows = 0; rows < m_game.rows(); rows++){
        for (int cols = 0; cols < m_game.cols(); cols++)
            hasBeenAttacked[rows][cols] = false;
        }
    
    for (int i = 0; i < 5; i++)
        timesBeenAttacked[i] = 0;
    
    
}    // Initialize a board. The parameter g refers to the game the board is being used in. Board member functions that need to know the number of rows or columns or the characteristics of ships will presumably ask the game object the board was constructed with.

void BoardImpl::clear()
{
    for (int rows = 0; rows < m_game.rows(); rows++){
        for (int cols = 0; cols < m_game.cols(); cols++)
            m_board[rows][cols] = '.';
        }
    // This function clears the board so it is empty, ready to be populated with ships.
}

void BoardImpl::block()
{
    int counter = 0;
    int randomRow = 0;
    int randomCol = 0;
    while (counter != (m_game.rows() * m_game.cols())/2){ //loop through half of elements in array
      // Block cells with 50% probability
        randomRow = randInt(m_game.rows());
        randomCol = randInt(m_game.cols());
        if (hasBeenBlocked[randomRow][randomCol] == false){
            hasBeenBlocked[randomRow][randomCol] = true;
            m_board[randomRow][randomCol] = '#';
            counter++;
        }
    }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++){
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == '#'){
                hasBeenBlocked[r][c] = false;
                m_board[r][c] = '.'; // TODO:  Replace this with code to unblock cell (r,c) if blocked
            }
        }
    }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId >=m_game.nShips() || shipId < 0){
        return false;
    }
    
    if (!m_game.isValid(topOrLeft)){
        return false; //checks to see if the point is inside the system
    }
    
    for (int i = 0; i < m_game.rows(); i++){
        for (int j = 0; j < m_game.cols(); j++){
            if (m_board[i][j] == m_game.shipSymbol(shipId)) {
                return false;//return false if The ship with that ship ID has previously been placed on this Board and not yet been unplaced since its most recent placement.
        }
    }
    }
    
    if (dir == HORIZONTAL){ //update Board to reflect that
        for (int m = 0; m < m_game.shipLength(shipId); m++){
            Point tryPlace(topOrLeft.r,topOrLeft.c+m);
            if (!m_game.isValid(tryPlace) || m_board[topOrLeft.r][topOrLeft.c+m] != '.'){
                return false;
            }
        }
        for (int m = 0; m < m_game.shipLength(shipId); m++){
            m_board[topOrLeft.r][topOrLeft.c+m] = m_game.shipSymbol(shipId);
        }
    }
    if (dir == VERTICAL){     //update Board to reflect that
        for (int m = 0; m < m_game.shipLength(shipId); m++){
            Point tryPlace(topOrLeft.r+m,topOrLeft.c);
            if (!m_game.isValid(tryPlace) || m_board[topOrLeft.r+m][topOrLeft.c] != '.'){
                return false;
            }
        }
        for (int m = 0; m < m_game.shipLength(shipId); m++){
            m_board[topOrLeft.r+m][topOrLeft.c] = m_game.shipSymbol(shipId);
        }
    }
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    int counter(0);
    for (int i = 0; i < m_game.rows(); i++){
        for (int j = 0; j < m_game.cols(); j++){
            if (m_board[i][j] == m_game.shipSymbol(shipId)) {
                counter++;//return false if The ship with that ship ID has previously been placed on this Board and not yet been unplaced since its most recent placement.
        }
    }
    }
    if (counter == 0)
        return false;
    
    if (dir == HORIZONTAL){
        for (int m = 0; m < m_game.shipLength(shipId); m++)
            if (m_board[topOrLeft.r][topOrLeft.c+m] != m_game.shipSymbol(shipId))
                return false;
    }
    if (dir == VERTICAL){    //return false if the board does not contain the entire ship at the indicated locations.
        for (int m = 0; m < m_game.shipLength(shipId); m++)
            if (m_board[topOrLeft.r+m][topOrLeft.c] != m_game.shipSymbol(shipId))
                return false;
    }
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == m_game.shipSymbol(shipId))
                m_board[r][c] = '.'; // update board to reflect unplaced ship
        }
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{

    cout << "  ";
    for (int c = 0; c < m_game.cols(); c++)
        cout << c;
    cout << endl;
    for (int r = 0; r < m_game.rows(); r++){
        if (!shotsOnly){
            cout << r << " ";
            for (int cols=0; cols < m_game.cols(); cols++){
                cout << m_board[r][cols];
            }
            cout << endl;
        }
        else{
            cout << r << " ";
            for (int cols=0; cols < m_game.cols(); cols++){
                if (m_board[r][cols] != 'X' && m_board[r][cols] != 'o' && m_board[r][cols] != '.'){
                    cout << '.';
                }else{
                    cout << m_board[r][cols];
                }
            }
            cout << endl;
        }
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    shipDestroyed = false;
    if (!m_game.isValid(p))
        return false; //function returns false if the attack point is outside of the board area
    if (hasBeenAttacked[p.r][p.c]) //function returns false if an attack is made on a previously attacked location
        return false; //UPDATE
    hasBeenAttacked[p.r][p.c] = true;
    if (m_board[p.r][p.c] != 'o' && m_board[p.r][p.c] != '.' && m_board[p.r][p.c] != 'X'){
        shotHit = true;
        int i (0);
        for (; i < m_game.nShips(); i++){
            if (m_board[p.r][p.c] == m_game.shipSymbol(i)){
                timesBeenAttacked[i] += 1;
                if (timesBeenAttacked[i] == m_game.shipLength(i)){
                    shipDestroyed = true;
                    shipId = i;
                }
                break;
            }
        }
        m_board[p.r][p.c] = 'X';
    }else {
        if (m_board[p.r][p.c] == '.'){
            m_board[p.r][p.c] = 'o';
        }
        shotHit = false;
    }
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    for (int rows = 0; rows < m_game.rows(); rows++){
        for (int cols = 0; cols < m_game.cols(); cols++){
            if (m_board[rows][cols] != '.' && m_board[rows][cols] != 'X' && m_board[rows][cols] != 'o')
                return false;
        }
    }
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
