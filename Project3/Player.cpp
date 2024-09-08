//
//  Player.cpp
//  Project3
//
//  Created by Ava Bronkar on 5/5/22.
//

#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <stack>
#include <iostream>
#include <string>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL)){
            return false;
        }
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}
/*
bool getLineWithOneDirection(Direction&d)
{
    bool result(cin >> d);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}
*/
class HumanPlayer : public Player
{
public:
    HumanPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
    virtual bool isHuman() const { return true; }
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
: Player(nm, g)
{}

bool HumanPlayer::placeShips(Board& b){
    int sumOfLengths(0);
    for (int i = 0; i < game().nShips(); i++)
        sumOfLengths+=game().shipLength(i);
    if (sumOfLengths > game().rows() * game().cols())
        return false;
    
    Direction d = HORIZONTAL;
    int row(0);
    int col(0);
    Point topmost;
    cout << name() << " the Human must place 5 ships." << endl;
    for (int i = 0; i < game().nShips(); i++){
        b.display(false);
        char dir = '?';
        int correctInput(0);
        while (correctInput == 0){
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            cin >> dir;
            if (dir == 'h'){
                d = HORIZONTAL;
                correctInput++;
            }
            if (dir == 'v'){
                d = VERTICAL;
                correctInput++;
            }
            if (dir != 'v' && dir != 'h'){
                cout << "Direction must be h or v." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            }
        }
        bool isInteger = false;
        bool isValidPoint = false;
        int goodPoint(0);
        for (;!isInteger || !isValidPoint;){
            if (d == VERTICAL)
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
            else
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            isInteger = getLineWithTwoIntegers(row,col);
            if (!isInteger){
                cout << "You must enter two integers." << endl;
                isInteger = false;
            }
            else{
                isInteger = true;
                goodPoint++;
            }
            topmost.r = row;
            topmost.c = col;
            isValidPoint = game().isValid(topmost);
            if(!isValidPoint || !b.placeShip(topmost, i, d)){
                cout << "The ship can not be placed there." << endl;
                isValidPoint = false;
            }
            else
                isValidPoint = true;
        }
    }
    return true;
}

Point HumanPlayer::recommendAttack(){ //while (!board.allShipsDestroyed())
    Point attack;
    int row(0);
    int col(0);
    cout << "Enter the row and column to attack (e.g., 3 5): ";
    if (!getLineWithTwoIntegers(row, col))
        cout << "You must enter two integers. " << endl;
    attack.r = row;
    attack.c = col;
    return attack;
}


void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                      bool shipDestroyed, int shipId){}

void HumanPlayer::recordAttackByOpponent(Point p){}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************



class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
    bool recPlace(Board &b,int shipID, int nShips);
    
private:
    int state;
    Point lasthit;
    bool hasBeenChosen[MAXROWS][MAXCOLS];
    
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g): Player(nm, g), state(1){
    for (int r = 0; r < game().rows(); r++){
        for (int c = 0; c < game().cols(); c++)
            hasBeenChosen[r][c] = false;
    }
}


bool MediocrePlayer::recPlace(Board &b, int shipID, int nShips){
    if (shipID == nShips)
        return true; //base case
    else {
        for (int r = 0; r < game().rows(); r++){
            for (int c = 0; c < game().cols(); c++){
                Point pt(r,c);
                if (b.placeShip(pt,shipID,HORIZONTAL)){
                    if (!recPlace(b, shipID+1, nShips))
                        b.unplaceShip(pt, shipID,HORIZONTAL);
                    else
                        return true;
                }
                if (b.placeShip(pt,shipID,VERTICAL)){
                    if (!recPlace(b, shipID+1, nShips))
                        b.unplaceShip(pt, shipID,VERTICAL);
                    else
                        return true;
                }
            }
        }
//        return false;
    }
    return false;
}



bool MediocrePlayer::placeShips(Board& b){
    int sumOfLengths(0);
    for (int i = 0; i < game().nShips(); i++)
        sumOfLengths+=game().shipLength(i);
    if (sumOfLengths > game().rows() * game().cols())
        return false;
    
    for (int i = 0; i < 50; i++){
        b.block();
        bool returnVal = recPlace(b,0, game().nShips());
        b.unblock();
        if (returnVal)
            return returnVal;
    }
    return false;
}

void MediocrePlayer::recordAttackByOpponent(Point p){
    
};


Point MediocrePlayer::recommendAttack(){
    if (state == 1){
        Point randPt = game().randomPoint();
        int rowOfRandPt = randPt.r;
        int colOfRandPt = randPt.c;
        while (hasBeenChosen[rowOfRandPt][colOfRandPt]){
            randPt = game().randomPoint();
            rowOfRandPt = randPt.r;
            colOfRandPt = randPt.c;
        }
        hasBeenChosen[rowOfRandPt][colOfRandPt] = true;
        return randPt;
    }else{
        int positive = randInt(2);
        int vertical = randInt(2);
        int magnitude = randInt(4) + 1;
        if(!positive){
            magnitude = -magnitude;
        }
        Point changept = lasthit;
        if(vertical){
            changept.r += magnitude;
        }else{
            changept.c += magnitude;
        }
        while (!game().isValid(changept) || hasBeenChosen[changept.r][changept.c]){
            positive = randInt(2);
            vertical = randInt(2);
            magnitude = randInt(4) + 1;
            if(!positive){
                magnitude = -magnitude;
            }
            changept = lasthit;
            if(vertical){
                changept.r += magnitude;
            }else{
                changept.c += magnitude;
            }
        }
        hasBeenChosen[changept.r][changept.c] = true;
        return changept;
    }
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                        bool shipDestroyed, int shipId){
    if (shotHit){
        if(!shipDestroyed){
            state = 2;
            lasthit = p;
        }else{
            state = 1;
        }
    }
}


//*********************************************************************
//  GoodPlayer
//*********************************************************************


class GoodPlayer : public Player
{
public:
    GoodPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    void update_probability(Point p);
private:
    Point getmax();
    int state;
    int delta_r;
    int delta_c;
    bool triedBothSides = false;
    Point firstHit;
    Point lastHit;
    Point nextAttack;
    stack<Point> history;
    bool isDiscovered[MAXROWS][MAXCOLS];
    int probability[MAXROWS][MAXCOLS];
    vector<int> ships;
};

GoodPlayer::GoodPlayer(string nm, const Game& g)
: Player(nm, g), state(1)
{
    for (int r = 0; r < game().rows(); r++){
        for (int c = 0; c < game().cols(); c++){
            isDiscovered[r][c] = false;
            probability[r][c] = 0;
        }
    }
    for (int i = 0; i < game().nShips(); i++){
        int shiplen = game().shipLength(i);
        ships.push_back(shiplen);
        for(int r = 0; r < game().rows() - shiplen+1; r++){
            for(int c = 0; c < game().cols(); c++){
                for(int k = 0; k < shiplen; k++)
                    probability[r+k][c]++;
            }
        }
        for(int r = 0; r < game().rows(); r++){
            for(int c = 0; c < game().cols()-shiplen+1; c++){
                for(int k = 0; k < shiplen; k++)
                    probability[r][c+k]++;
            }
        }
        
    }
    for(int i = 0; i < game().rows(); i++){
        for(int j = 0; j < game().cols(); j++){
            cout << probability[i][j] << " " ;
        }
        cout << endl;
    }
    
}

bool GoodPlayer::placeShips(Board& b){
    int sumOfLengths(0);
    for (int i = 0; i < game().nShips(); i++)
        sumOfLengths+=game().shipLength(i);
    if (sumOfLengths > game().rows() * game().cols())
        return false;
    for (int i = 0; i < game().nShips(); i++){
        bool notplaced = true;
        while (notplaced){
            Point randPoint = game().randomPoint();
            int dir = randInt(2);
            if(dir){
                if(b.placeShip(randPoint,i,HORIZONTAL))
                    notplaced = false;
                else if (b.placeShip(randPoint,i,VERTICAL))
                    notplaced = false;
            }else{
                if(b.placeShip(randPoint,i,VERTICAL))
                    notplaced = false;
                else if (b.placeShip(randPoint,i,HORIZONTAL))
                    notplaced = false;
                
            }
            
        }
    }
    return true;
}
Point GoodPlayer::getmax(){
    for(int i = 0; i < game().rows(); i++){
        for(int j = 0; j < game().cols(); j++){
            cout << probability[i][j] << " " ;
        }
        cout << endl;
    }
    int maxr;
    int maxc;
    int val = -1;
    for(int i = 0; i < game().rows(); i++){
        for(int j = 0; j <game().cols(); j++){
            if(val < probability[i][j]){
                val = probability[i][j];
                maxr = i;
                maxc = j;
            }
        }
    }
    Point p(maxr, maxc);
    return p;
}

Point GoodPlayer::recommendAttack(){
    Point changept;
    cout << "STATE: " << state << endl;
    if (state == 1){
        Point p = getmax();
        isDiscovered[p.r][p.c] = true;
        update_probability(p);
        return p;
    }else if (state == 2){
        changept = lastHit;
        Point up(lastHit.r+1,lastHit.c);
        Point down(lastHit.r-1, lastHit.c);
        Point right(lastHit.r, lastHit.c+1);
        Point left(lastHit.r, lastHit.c-1);
        bool upb = !game().isValid(up) || isDiscovered[up.r][up.c];
        bool downb = !game().isValid(down) || isDiscovered[down.r][down.c];
        bool leftb = !game().isValid(left) || isDiscovered[left.r][left.c];
        bool rightb = !game().isValid(right) || isDiscovered[right.r][right.c];
        bool unreachable = upb && downb && leftb && rightb;
        if(!upb){
            isDiscovered[up.r][up.c] = true;
            history.push(up);
        }
        if (!downb){
            isDiscovered[down.r][down.c] = true;
            history.push(down);
        }
        if (!leftb){
            isDiscovered[left.r][left.c] = true;
            history.push(left);
        }
        if (!rightb){
            isDiscovered[right.r][right.c] = true;
            history.push(right);
        }
        if(history.empty()){
            state = 1;
            return recommendAttack();;
        }else{
            Point p = history.top();
            history.pop();
            update_probability(p);
            return p;
        }
    }
    return changept;
}

void GoodPlayer::update_probability(Point p){
    //decrease left vals
    for (int k = 0; k < ships.size(); k++){
        int leftbound = max(0, p.r - ships[k]);
        int rightbound = min(game().rows(), p.r + ships[k]);
        for(int i = leftbound; i < rightbound; i++){
            if(probability[i][p.c] != 0)
                probability[i][p.c]-= 1;
        }
        int downbound = max(0, p.c - ships[k]);
        int upbound = min(game().cols(), p.c + ships[k]);
        for(int j = downbound; j < upbound; j++){
            if(probability[p.r][j] != 0)
                probability[p.r][j] -= 1;
        }
    }
}
    
    /*
    Point changept;
    if (state != 1){
        vector<Point> dir={{-1,0},{0,-1},{1,0},{0,1}}; //breadth first search
        history.push(lastHit);
        while (!history.empty()) {
            Point curr = history.front();
            history.pop();
            int row = curr.r;
            int col = curr.c;
            for (Point p: dir){
                Point savept(row + p.r, col + p.c);
                if (game().isValid(savept) && !isDiscovered[savept.r][savept.c]){
                    history.push(savept);
                }
            }
            if(history.empty()){
                state = 1;
            }else{
                Point front = history.front();
                history.pop();
                bool broken = false;
                while(isDiscovered[front.r][front.c]){
                    if(history.empty()){
                        broken = true;
                        break;
                    }
                    front = history.front();
                    history.pop();
                }
                if (!broken && !isDiscovered[front.r][front.c]){
        
                    isDiscovered[front.r][front.c] = true;
                    cout << "YO" << endl;
                    return front;
                }
                else state =1;
            }
        }
    }
    if (state == 1){
        Point randPt = game().randomPoint();
        int rowOfRandPt = randPt.r;
        int colOfRandPt = randPt.c;
        while (isDiscovered[rowOfRandPt][colOfRandPt]){
            randPt = game().randomPoint();
            rowOfRandPt = randPt.r;
            colOfRandPt = randPt.c;
        }
        isDiscovered[rowOfRandPt][colOfRandPt] = true;
        return randPt;
    }
    cout << "should not be here" << endl;
    return changept;

}
     */
    /*
    else if (state == 3){
        
        changept = lastHit;
        changept.r += delta_r;
        changept.c += delta_c;
        if (!game().isValid(changept) || isDiscovered[changept.r][changept.c]){
            state = 2;
        }else{
            return changept;
        }
    }
    
    if (state == 2){
        int positive = randInt(2);
        int vertical = randInt(2);
        int magnitude = 1;
        if(!positive){
            magnitude = -magnitude;
        }
        changept = attackHit;
        if(vertical){
            changept.r += magnitude;
        }else{
            changept.c += magnitude;
        }
        while (!game().isValid(changept) || isDiscovered[changept.r][changept.c]){
            positive = randInt(2);
            vertical = randInt(2);
            magnitude = 1;
            if(!positive){
                magnitude = -magnitude;
            }
            changept = attackHit;
            if(vertical){
                changept.r += magnitude;
            }else{
                changept.c += magnitude;
            }
        }
    }
    */
/*
    state 2 on hit go to state3
    State3 ->
    
 */


void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId){
    if (shotHit){
        if(state==1 ){
            state = 2;
        }
    }else{
        /*
        if(state == 3){
            lastHit = history.top();
            state = 2;
        }
         */
    }
    if(shipDestroyed){
        ships.erase(ships.begin() + shipId);
    }
}
     
    

void GoodPlayer::recordAttackByOpponent(Point p){
    
}


//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}




