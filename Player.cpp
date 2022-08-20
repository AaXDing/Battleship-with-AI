#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// helper function
// find the position of the same Point in a vector
int find(vector<Point> cell, Point p)
{
    for (size_t i = 0; i < cell.size(); i++)
    {
        if (cell[i].r == p.r && cell[i].c == p.c)
            return i;
    }
    return cell.size();
}

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
    AwfulPlayer(string nm, const Game &g);
    virtual bool placeShips(Board &b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);

private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game &g)
    : Player(nm, g), m_lastCellAttacked(0, 0)
{
}

bool AwfulPlayer::placeShips(Board &b)
{
    // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
            return false;
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

class HumanPlayer : public Player
{
public:
    HumanPlayer(string nm, const Game &g);
    virtual bool isHuman() const;
    virtual bool placeShips(Board &b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);

private:
};

// helper function
// get two input return true if both are int
bool getLineWithTwoIntegers(int &r, int &c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear(); // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// helper function
// get a input return true if it is char
bool getLineWithChar(char &dir)
{
    bool result(cin >> dir);
    if (!result)
        cin.clear();
    cin.ignore(10000, '\n');
    return result;
}

HumanPlayer::HumanPlayer(string nm, const Game &g)
    : Player(nm, g)
{
}

bool HumanPlayer::isHuman() const
{
    return true;
}

bool HumanPlayer::placeShips(Board &b)
{
    cout << name() << " must place " << game().nShips() << " ships." << endl; // ship name to be placed
    for (int k = 0; k < game().nShips(); k++)
    {
        b.display(false);
        cout << "Enter h or v for direction of " << game().shipName(k) << " (length "
             << game().shipLength(k) << "): ";
        char dir;
        while (!getLineWithChar(dir) || (dir != 'h' && dir != 'v')) // if input is not h or v, print message
        {
            cout << "Direction must be h or v." << endl;
            cout << "Enter h or v for direction of " << game().shipName(k) << " (length "
                 << game().shipLength(k) << "): ";
        }
        int r, c;
        Direction D_dir;
        if (dir == 'h')
        {
            D_dir = HORIZONTAL;
            cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
        }
        else
        {
            D_dir = VERTICAL;
            cout << "Enter row and column of topmost cell (e.g., 3 5): ";
        }

        while (!getLineWithTwoIntegers(r, c)) // if input not two int, print message
        {
            cout << "You must enter two integers." << endl;
            if (D_dir == HORIZONTAL)
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            else
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
        }

        while (!b.placeShip(Point(r, c), k, D_dir)) // if ship unable to be placed
        {
            cout << "The ship can not be placed there." << endl;
            if (D_dir == HORIZONTAL)
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            else
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
            while (!getLineWithTwoIntegers(r, c)) // if input not two int, print message
            {
                cout << "You must enter two integers." << endl;
                if (D_dir == HORIZONTAL)
                    cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
                else
                    cout << "Enter row and column of topmost cell (e.g., 3 5): ";
            }
        }
    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
    cout << "Enter the row and column to attack (e.g., 3 5): ";
    int r, c;
    while (!getLineWithTwoIntegers(r, c)) // loop until get two integer value
    {
        cout << "You must enter two integers." << endl;
        cout << "Enter the row and column to attack (e.g., 3 5): ";
    }
    return Point(r, c);
}

void HumanPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
}

void HumanPlayer::recordAttackByOpponent(Point /* p */)
{
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string nm, const Game &g);
    virtual bool placeShips(Board &b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);

private:
    bool place(int k, int total, Board &b);
    vector<Point> CellNotDiscovered;
    vector<Point> cellToHit;
    Point m_shipCell;
    bool m_shotHit;
    bool m_shipDestroyed;
    bool m_state;
};

MediocrePlayer::MediocrePlayer(string nm, const Game &g)
    : Player(nm, g), m_state(true)
{
    for (int r = 0; r < g.rows(); r++) // push all points into undiscovered
        for (int c = 0; c < g.cols(); c++)
            CellNotDiscovered.push_back(Point(r, c));
}

bool MediocrePlayer::placeShips(Board &b)
{
    bool res;
    for (int count = 0; count < 50; count++) // try place ship 50 times
    {
        b.block();
        res = place(0, game().nShips(), b);
        b.unblock();
        if (res) // if succeed return
            return res;
    }
    return false;
}

bool MediocrePlayer::place(int k, int total, Board &b)
{
    for (int r = 0; r < game().rows(); r++) // try placing horizontally first
    {
        for (int c = 0; c <= game().cols() - game().shipLength(k); c++)
        {
            Point cor(r, c);
            if (b.placeShip(cor, k, HORIZONTAL)) // if successfully placed, place next ship
            {
                if (k + 1 < total)
                {
                    if (place(k + 1, total, b))
                        return true;
                    b.unplaceShip(cor, k, HORIZONTAL); // if placement not working unplace it
                }
                else
                    return true;
            }
        }
    }

    for (int c = 0; c < game().cols(); c++) // try placing vertically
    {
        for (int r = 0; r <= game().rows() - game().shipLength(k); r++)
        {
            Point cor(r, c);
            if (b.placeShip(cor, k, VERTICAL)) // if successfully placed, place next ship
            {
                if (k + 1 < total)
                {
                    if (place(k + 1, total, b))
                        return true;
                    b.unplaceShip(cor, k, VERTICAL); // if placement not working unplace it
                }
                else
                    return true;
            }
        }
    }

    return false;
}

void MediocrePlayer::recordAttackByOpponent(Point /* p */)
{
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                        bool shipDestroyed, int shipId)
{
    int x = find(CellNotDiscovered, p);
    CellNotDiscovered.erase(CellNotDiscovered.begin() + x); // erase from not attacked cell vector
    m_shotHit = shotHit;
    m_shipDestroyed = shipDestroyed;
    if (m_state) // true if hit randomly
    {
        if (!m_shotHit) // off the target
            return;
        if (shipDestroyed) // ship destroyed
            return;
        m_state = false;
        m_shipCell = p; // save the point, initial shipcell hit
        for (int i = 1; i < 5; i++)
        {
            cellToHit.push_back(Point(p.r, p.c + i));
            cellToHit.push_back(Point(p.r + i, p.c));
            cellToHit.push_back(Point(p.r, p.c - i));
            cellToHit.push_back(Point(p.r - i, p.c));
        }
    }
    else // ship hitting focus
    {
        if (shipDestroyed) // ship destroy turn on randomly hit
            m_state = true;
    }
}

Point MediocrePlayer::recommendAttack()
{
    if (m_state)
    {
        int p = randInt(CellNotDiscovered.size()); // find a point not hit before
        Point cor = CellNotDiscovered[p];
        return cor;
    }
    else
    {
        while (!cellToHit.empty())
        {
            int i = randInt(cellToHit.size());
            Point curr = cellToHit[i];
            cellToHit.erase(cellToHit.begin() + i);
            if (CellNotDiscovered.begin() + find(CellNotDiscovered, curr) ==
                CellNotDiscovered.end()) // check if the point attacked before
                continue;
            return curr;
        }
        m_state = 1;
        return recommendAttack();
    }
}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
public:
    GoodPlayer(string nm, const Game &g);
    virtual bool placeShips(Board &b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);

private:
    bool place(int k, int total, Board &b);
    bool randomPlace(int k, int total, Board &b, vector<Point> &placedCell);
    Point m_shipCell;
    bool m_shotHit;
    bool m_shipDestroyed;
    bool m_state;
    Point toCheck;
    int shortestShip;
    vector<int> shipIdDestroyed;
    vector<Point> CellNotDiscovered;
    vector<Point> ship;
};

GoodPlayer::GoodPlayer(string nm, const Game &g)
    : Player(nm, g), m_state(true), toCheck(0, 0), shortestShip(MAXROWS)
{
    for (int i = 0; i < g.nShips(); i++) // find shortest ship on board
        if (shortestShip > g.shipLength(i))
            shortestShip = g.shipLength(i);

    for (int r = 0; r < g.rows(); r++) // input all cell not attacked
        for (int c = 0; c < g.cols(); c++)
            CellNotDiscovered.push_back(Point(r, c));
}

bool GoodPlayer::place(int k, int total, Board &b)
{
    for (int r = 0; r < game().rows(); r++) // try placing horizontally first
    {
        for (int c = 0; c <= game().cols() - game().shipLength(k); c++)
        {
            Point cor(r, c);
            if (b.placeShip(cor, k, HORIZONTAL)) // if successfully placed, place next ship
            {
                if (k + 1 < total)
                {
                    if (place(k + 1, total, b))
                        return true;
                    b.unplaceShip(cor, k, HORIZONTAL); // if placement not working unplace it
                }
                else
                    return true;
            }
        }
    }

    for (int c = 0; c < game().cols(); c++) // try placing vertically first
    {
        for (int r = 0; r <= game().rows() - game().shipLength(k); r++)
        {
            Point cor(r, c);
            if (b.placeShip(cor, k, VERTICAL)) // if successfully placed, place next ship
            {
                if (k + 1 < total)
                {
                    if (place(k + 1, total, b))
                        return true;
                    b.unplaceShip(cor, k, VERTICAL); // if placement not working unplace it
                }
                else
                    return true;
            }
        }
    }

    return false;
}

bool GoodPlayer::randomPlace(int k, int total, Board &b, vector<Point> &placedCell)
{
    int count = 0;
    while (count < 50)
    {
        count++;
        int r = randInt(game().rows());
        int c = randInt(game().cols());
        Direction dir;
        bool valid = true;
        const int length = game().shipLength(k);
        if (randInt(2)) // randomly select direction
        {
            dir = HORIZONTAL;
            if (c + length > game().cols()) // do it again if out of bound
                continue;
            for (int i = c; i < c + length; i++)
            {
                for (Point x : placedCell)
                {
                    int d = (x.r - r) * (x.r - r) + (x.c - i) * (x.c - i); // if the ship is in contact with other ship
                    if (d < 1)                                             // saved by placed cell do it again
                        valid = false;
                    break;
                }
                if (!valid)
                    break;
            }
            if (!valid)
                continue;
            for (int i = c; i < c + length; i++) // save it to placedCell
                placedCell.push_back(Point(r, i));
        }
        else
        {
            dir = VERTICAL;
            if (r + length > game().rows())
                continue;
            for (int i = r; i < r + length; i++)
            {
                for (Point x : placedCell)
                {
                    int d = (x.r - i) * (x.r - i) + (x.c - c) * (x.c - c);
                    if (d < 1)
                        valid = false;
                    break;
                }
                if (!valid)
                    break;
            }
            if (!valid)
                continue;
            for (int i = r; i < r + length; i++)
                placedCell.push_back(Point(i, c));
        }

        if (b.placeShip(Point(r, c), k, dir)) // if place ship success proceed to next ship
        {
            if (k + 1 < total)
            {
                if (place(k + 1, total, b))
                    return true;
                b.unplaceShip(Point(r, c), k, dir); // if placement not working unplace it
                int cellCount = 0;
                for (int i = 0; i < k; i++)
                    cellCount += game().shipLength(i);

                for (int i = 0; i < length; i++)
                    placedCell.erase(placedCell.begin() + cellCount);
            }
            else
                return true;
        }
        else
        {
            for (int i = 0; i < length; i++)
                placedCell.pop_back();
        }
    }
    return false;
}

bool GoodPlayer::placeShips(Board &b)
{
    vector<Point> placedCell;
    if (randomPlace(0, game().nShips(), b, placedCell))
        return true;
    b.clear();
    return place(0, game().nShips(), b);
}

void GoodPlayer::recordAttackByOpponent(Point /* p */)
{
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId)
{
    int x = find(CellNotDiscovered, p);
    CellNotDiscovered.erase(CellNotDiscovered.begin() + x);
    m_shotHit = shotHit;
    m_shipDestroyed = shipDestroyed;
    if (shipDestroyed) // clear vector ship, which is the cell pending to be checked
        ship.clear();
    if (m_state) // randomly select number
    {
        if (!m_shotHit)
            return;
        if (shipDestroyed)
            return;
        m_state = false; // change into destroy ship mode
        m_shipCell = p;

        // following function to find a cell all four direction and put to the back of ship
        int c = p.c + 1;
        while (c < game().cols() && c - p.c < 5)
        {
            if (CellNotDiscovered.begin() + find(CellNotDiscovered, Point(p.r, c)) ==
                CellNotDiscovered.end()) // if cell already attack proceed to next one
            {
                c++;
            }
            else
            {
                ship.push_back(Point(p.r, c)); // push into ship if not attacked before
                break;
            }
        }
        c = p.c - 1;
        while (c >= 0 && p.c - c < 5)
        {
            if (CellNotDiscovered.begin() + find(CellNotDiscovered, Point(p.r, c)) ==
                CellNotDiscovered.end())
            {
                c--;
            }
            else
            {
                ship.push_back(Point(p.r, c));
                break;
            }
        }
        int r = p.r + 1;
        while (r < game().rows() && r - p.r < 5)
        {
            if (CellNotDiscovered.begin() + find(CellNotDiscovered, Point(r, p.c)) ==
                CellNotDiscovered.end())
            {
                r++;
            }
            else
            {
                ship.push_back(Point(r, p.c));
                break;
            }
        }
        r = p.r - 1;
        while (r >= 0 && p.r - r < 5)
        {
            if (CellNotDiscovered.begin() + find(CellNotDiscovered, Point(r, p.c)) ==
                CellNotDiscovered.end())
            {
                r--;
            }
            else
            {
                ship.push_back(Point(r, p.c));
                break;
            }
        }
        // cout << "size: " << ship.size() << endl;
    }
    else
    {
        if (shipDestroyed)
        {
            shipIdDestroyed.push_back(shipId);
            m_state = true;
            if (game().shipLength(shipId) == shortestShip) // when ship destroyed, check if the shortest ship still exist
            {
                shortestShip = MAXROWS;
                for (int i = 0; i < game().nShips(); i++) // search for shortest ship
                {
                    bool jump = false;
                    for (int x : shipIdDestroyed)
                    {
                        if (i == x)
                        {
                            jump = true;
                            break;
                        }
                    }
                    if (jump)
                        continue;
                    if (shortestShip > game().shipLength(i))
                        shortestShip = game().shipLength(i);
                }
            }
        }
        if (shotHit)
        {
            if (m_shipCell.r == p.r) // find the direction of the previous attack
            {
                if (m_shipCell.c < p.c)
                {
                    if (p.c - m_shipCell.c + 1 < 5 && p.c + 1 < game().cols()) // find next cell available
                        ship.push_back(Point(p.r, p.c + 1));
                }
                else
                {
                    if (m_shipCell.c - p.c + 1 < 5 && p.c - 1 >= 0)
                        ship.push_back(Point(p.r, p.c - 1));
                }
            }
            else
            {
                if (m_shipCell.r < p.r)
                {
                    if (p.r - m_shipCell.r + 1 < 5 && p.r + 1 < game().rows())
                        ship.push_back(Point(p.r + 1, p.c));
                }
                else
                {
                    if (m_shipCell.r - p.r + 1 < 5 && p.r - 1 >= 0)
                        ship.push_back(Point(p.r - 1, p.c));
                }
            }
        }
        if (ship.empty()) // if ship is empty and did not destroy ship, attack randomly
                          // this is cause by enemy place two ship next to each other
            m_state = true;
    }
}

Point GoodPlayer::recommendAttack()
{
    if (m_state)
    {
        while (CellNotDiscovered.begin() + find(CellNotDiscovered, toCheck) == CellNotDiscovered.end()) // if cell already attacked, find next
        {
            toCheck.c += shortestShip;
            if (toCheck.c >= game().cols()) // if exceed column, row++
            {
                toCheck.r++;
                if (toCheck.r >= game().rows()) // if row exceed limit find the first not attacked cell
                    return CellNotDiscovered.front();
                toCheck.c = 0;
                while ((toCheck.c + toCheck.r) % shortestShip != 0) // find the column "shortestShip" length behind prev step
                    toCheck.c++;
            }
        }
        return toCheck;
    }
    else
    {
        if (ship.size() > 200) // if run into infinite loop, since repitition is not checked when push into ship
        {
            m_state = true;
            return CellNotDiscovered.front();
        }
        while (ship.size() != 0)
        {
            Point curr = ship.back(); // return next point in ship
            ship.pop_back();
            if (CellNotDiscovered.begin() + find(CellNotDiscovered, curr) ==
                CellNotDiscovered.end())
                continue;
            return curr;
        }
        m_state = true;
        return CellNotDiscovered.front();
    }
}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player *createPlayer(string type, string nm, const Game &g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"};

    int pos;
    for (pos = 0; pos != sizeof(types) / sizeof(types[0]) &&
                  type != types[pos];
         pos++)
        ;
    switch (pos)
    {
    case 0:
        return new HumanPlayer(nm, g);
    case 1:
        return new AwfulPlayer(nm, g);
    case 2:
        return new MediocrePlayer(nm, g);
    case 3:
        return new GoodPlayer(nm, g);
    default:
        return nullptr;
    }
}
