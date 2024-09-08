//
//  Game.cpp
//  Project3
//
//  Created by Ava Bronkar on 5/5/22.
//

#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;




class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    
    
private:
    int m_rows;
    int m_cols;

    
    struct Ship{
        int len = 0;
        char sym = ' ';
        string nm = "";
        int ID = 0; //assign to certain ships specifically
    };

    
    vector<Ship>vs;
        
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols): m_rows(nRows), m_cols(nCols)
{
    // Construct a game with the indicated number of rows and columns, which must not exceed MAXROWS and MAXCOLS (defined in globals.h), respectively.
}

int GameImpl::rows() const
{
    return m_rows; //Return the number of rows in the game board.
}

int GameImpl::cols() const
{
    return m_cols; // Return the number of columns in the game board.
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols(); //Return true if and only if the point denotes a position on the game board.
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols())); //Return a random point on the game board.
}

bool GameImpl::addShip(int length, char symbol, string name) //Add a new type of ship to the game
{
    
    if (symbol == 'X' || symbol == 'o' || symbol == '.' || isprint(symbol) == 0) //if symbol is invalid return false
        return false;
    if (length < 0 || length > rows() || length > cols()) //if length is negative or too big return false
        return false;
    for(int i = 0; i < vs.size(); i++)//checks to see if symbol is the same as other symbols
        if (symbol == vs[i].sym)
            return false;
    Ship newShip;
    newShip.sym = symbol;
    newShip.len = length;
    newShip.nm = name;
    newShip.ID = (int) vs.size();
    vs.push_back(newShip);
    // ID should be based on when it was added to the array
    return true;
}


int GameImpl::nShips() const //Return the number of ship types in the game, which will be the number of times addShip was called returning true. The integers from 0 to nShips()−1 will be the ship IDs of the ships in the game, each one corresponding to the ship type added by one of the successful calls to addShip. Although each ship type's ship ID will be a distinct integer in the range 0 to nShips()-1, it is not required that the first ship added have ship ID 0, the next 1, the next 2, etc. (although that may be what naturally happens with your implementation).

{
    return (int) vs.size();
}

int GameImpl::shipLength(int shipId) const // Return the number of positions occupied by the ship whose ID is shipId.
{
    for (int i = 0; i < vs.size(); i++){
        if (vs[i].ID == shipId)
            return vs[i].len; //loop thru vector until u find matching ID, return length of ship with ID; if no such ID return -1
    }
    return -1;
}

char GameImpl::shipSymbol(int shipId) const
{
    for (int i = 0; i < vs.size(); i++){
        if (vs[i].ID == shipId)
            return vs[i].sym;//loop thru vector until u find matching ID, return symbol of ship with ID; if no such ID, return ?
    }
    return '?';
}

string GameImpl::shipName(int shipId) const
{
    for (int i = 0; i < vs.size(); i++){
        if (vs[i].ID == shipId)
            return vs[i].nm;//loop thru vector until u find matching ID, return symbol of ship with ID; if no such ID, return empty string
    }
    return "";
    
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if (!p1->placeShips(b1)){
        return nullptr;
    }
    if (!p2->placeShips(b2)){
        return nullptr;
    }
    Player* winner;
    while (true){
        cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
        if (p1->isHuman()){
            b2.display(true);
        }
        else{
            b2.display(false);
            }
        bool b2shotHit;
        bool b2shipDestroyed;
        int b2shipID;
        Point p1att = p1->recommendAttack();
        bool validShot1 = b2.attack(p1att, b2shotHit, b2shipDestroyed, b2shipID);
        p1->recordAttackResult(p1att, validShot1, b2shotHit, b2shipDestroyed, b2shipID);
        if(validShot1){
            string result = b2shotHit ? "hit something" : "missed";
            cout << p1->name() << " attacked (" << p1att.r << "," << p1att.c << ") and " <<  result << ", resulting in:" << endl;
            if (p1->isHuman()){
                b2.display(true);
            }
            else{
                b2.display(false);
            }
            if(b2.allShipsDestroyed()){
                winner = p1;
                //output string message, break
                break;
            }
        }else{
            cout << p1->name() << " wasted a shot at (" << p1att.r << "," << p1att.c << ")" << endl;
        }
        if (shouldPause){
            cout << "Press enter to continue: ";
            cin.ignore();
        }//enter to continue
        cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
        if (p2->isHuman()){
              b1.display(true);
        }
        else{
            b1.display(false);
        }
        // enter to continue;
        bool b1shotHit;
        bool b1shipDestroyed;
        int b1shipID;
        Point p2att = p2->recommendAttack();
        bool validShot2 = b1.attack(p2att, b1shotHit, b1shipDestroyed, b1shipID);
        p2->recordAttackResult(p2att, validShot2, b1shotHit, b1shipDestroyed, b1shipID);
        if(validShot2){
            string result;
            if (b1shotHit){
                result = "hit something";
            }else{
                result = "missed";
            }
            cout << p2->name() << " attacked (" << p2att.r << "," << p2att.c << ") and " <<  result << ", resulting in:" << endl;
            if(validShot2){
                if (p2->isHuman()){
                    b1.display(true);
                    }
                else{
                    b1.display(false);
                }
                if(b1.allShipsDestroyed()){
                    winner = p2;
                    //output string message, break
                    break;
                }
            }
        }else{
            cout << p2->name() << " wasted a shot at (" << p2att.r << "," << p2att.c << ")" << endl;
        }
        if (shouldPause){
            cout << "Press enter to continue: ";
            cin.ignore();
        }//enter to continue
    }
    cout << winner->name() + " wins!" << endl;
    if (p1->isHuman() && b1.allShipsDestroyed()){
        b2.display(false);
    }
    if (p2->isHuman() && b2.allShipsDestroyed()){
        b1.display(false);
    }
    return winner;// This compiles but may not be correct - don't start yet
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}


