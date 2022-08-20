#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

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
    Player *play(Player *p1, Player *p2, Board &b1, Board &b2, bool shouldPause);

private:
    // print the result of attacking
    void attackMessage(string name, Point cor, bool shotHit, bool shipDestroyed, int shipId);
    int m_r, m_c, m_size;
    vector<int> m_sL;
    vector<char> m_sym;
    vector<string> m_name;
};

// helper function
// print the result of attacking
void GameImpl::attackMessage(string name, Point cor, bool shotHit, bool shipDestroyed, int shipId)
{
    cout << name << " attacked (" << cor.r << "," << cor.c << ") and ";
    if (shotHit)
        if (shipDestroyed)
            cout << "destroyed the " << shipName(shipId);
        else
            cout << "hit something";
    else
        cout << "missed";
    cout << ", resulting in:" << endl;
}

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
    : m_r(nRows), m_c(nCols), m_size(0)
{
    m_sym.push_back('X'); // store the three symbols used to mark board into symbol used
    m_sym.push_back('o');
    m_sym.push_back('.');
}

int GameImpl::rows() const
{
    return m_r;
}

int GameImpl::cols() const
{
    return m_c;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    for (char i : m_sym) // check symbol not used before
    {
        if (symbol == i)
            return false;
    }
    m_sym.push_back(symbol); // add the symbol, length, and name to vector
    m_sL.push_back(length);
    m_name.push_back(name);
    m_size++;
    return true;
}

int GameImpl::nShips() const
{
    return m_size;
}

int GameImpl::shipLength(int shipId) const
{
    return m_sL[shipId];
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_sym[shipId + 3]; // skip the first three element of the vector
}

string GameImpl::shipName(int shipId) const
{
    return m_name[shipId];
}

Player *GameImpl::play(Player *p1, Player *p2, Board &b1, Board &b2, bool shouldPause)
{
    if (!p1->placeShips(b1) || !p2->placeShips(b2)) // return nullptr if either side placeships failed
        return nullptr;
    bool p1Human = p1->isHuman();
    bool p2Human = p2->isHuman();
    bool c_shotHit = false, c_shipDestoryed = false;
    int c_shipId;
    bool valid;
    Point cor;
    // int attackCount = 0;
    while (true) // infinite loop break when winner found
    {
        // attackCount++;
        cout << p1->name() << "'s turn.  Board for " << p2->name() << ":" << endl; // start of p1 attack
        b2.display(p1Human);
        cor = p1->recommendAttack();
        valid = b2.attack(cor, c_shotHit, c_shipDestoryed, c_shipId);
        if (!valid)
            cout << p1->name() << " wasted a shot at (" << cor.r << "," << cor.c << ")." << endl;
        // cerr << c_shotHit << endl; //test
        else
        {
            attackMessage(p1->name(), cor, c_shotHit, c_shipDestoryed, c_shipId);
            b2.display(p1Human);
        }
        if (b2.allShipsDestroyed())
        {
            cout << p1->name() << " wins!" << endl;
            // cerr << attackCount << endl;    //test
            if (p2Human)
            {
                cout << "Here is where " << p1->name() << "'s ships were:" << endl;
                b1.display(false);
            }
            return p1;
        }
        p1->recordAttackResult(cor, valid, c_shotHit, c_shipDestoryed, c_shipId);
        p2->recordAttackByOpponent(cor);
        if (shouldPause)
            waitForEnter();

        cout << p2->name() << "'s turn.  Board for " << p1->name() << ":" << endl; // start of p2 attack
        b1.display(p2Human);
        cor = p2->recommendAttack();
        valid = b1.attack(cor, c_shotHit, c_shipDestoryed, c_shipId);
        if (!valid)
            cout << p2->name() << " wasted a shot at (" << cor.r << "," << cor.c << ")." << endl;
        // cerr << c_shotHit << endl;  //test
        else
        {
            attackMessage(p2->name(), cor, c_shotHit, c_shipDestoryed, c_shipId);
            b1.display(p2Human);
        }
        if (b1.allShipsDestroyed())
        {
            cout << p2->name() << " wins!" << endl;
            if (p1Human)
            {
                cout << "Here is where " << p2->name() << "'s ships were:" << endl;
                b2.display(false);
            }
            // cerr << attackCount << endl;    //test
            return p2;
        }
        p2->recordAttackResult(cor, valid, c_shotHit, c_shipDestoryed, c_shipId);
        p1->recordAttackByOpponent(cor);
        if (shouldPause)
            waitForEnter();
    }
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1 || nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1 || nCols > MAXCOLS)
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
    if (length > rows() && length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol) || !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X' || symbol == '.' || symbol == 'o')
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
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipName(shipId);
}

Player *Game::play(Player *p1, Player *p2, bool shouldPause)
{
    if (p1 == nullptr || p2 == nullptr || nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}
