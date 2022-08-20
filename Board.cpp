#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
public:
    BoardImpl(const Game &g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool &shotHit, bool &shipDestroyed, int &shipId);
    bool allShipsDestroyed() const;

private:
    bool checkValid(const int length, const int r, const int c, const int cr, const int cc) const;
    const Game &m_game;
    char m_board[MAXROWS][MAXCOLS];
    vector<int> existShip;
    bool check(char sym);
};

BoardImpl::BoardImpl(const Game &g)
    : m_game(g)
{
    clear();
}

void BoardImpl::clear()
{
    char clear = '.';
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            m_board[r][c] = clear;
        }
    }
    existShip.clear();
}

void BoardImpl::block()
{
    const char block = 'X';
    int half = m_game.rows() * m_game.cols() / 2;
    int count = 0;
    while (count < half)
    { // if block blocked is less than half the cell
        Point it = m_game.randomPoint();
        if (m_board[it.r][it.c] != block)
        {
            m_board[it.r][it.c] = block;
            count++;
        }
    }
}

void BoardImpl::unblock()
{
    char block = 'X';
    char clear = '.';
    for (int r = 0; r < m_game.rows(); r++) // loop through all cell, clear all that is blocked
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == block)
                m_board[r][c] = clear;
        }
    }
}

bool BoardImpl::checkValid(const int length, const int r, const int c, const int cr, const int cc) const
{
    return !(c < 0 || c + length > cc || r < 0 || r >= cr); // helper function check exceed the board
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId < 0 || shipId >= m_game.nShips())
        return false;
    for (size_t i = 0; i < existShip.size(); i++) // check shipId is already used
    {
        if (shipId == existShip[i])
            return false;
    }
    int length = m_game.shipLength(shipId);
    if (dir == HORIZONTAL) // check if will be place horizontally
    {
        if (!checkValid(length, topOrLeft.r, topOrLeft.c, m_game.rows(), m_game.cols()))
            return false;
        for (int c = topOrLeft.c; c < topOrLeft.c + length; c++) // check whether all cells is clear
        {
            if (m_board[topOrLeft.r][c] != '.')
                return false;
        }
    }
    else // check if will be place vertically
    {
        if (!checkValid(length, topOrLeft.c, topOrLeft.r, m_game.cols(), m_game.rows()))
            return false;
        for (int r = topOrLeft.r; r < topOrLeft.r + length; r++) // check whether all cells is clear
        {
            if (m_board[r][topOrLeft.c] != '.')
                return false;
        }
    }
    // At this point the ship is valid and ready to be placed

    existShip.push_back(shipId);
    char symbol = m_game.shipSymbol(shipId);
    if (dir == HORIZONTAL) // place horizontally
    {
        for (int c = 0; c < length; c++)
            m_board[topOrLeft.r][topOrLeft.c + c] = symbol;
    }
    else // place vertically
    {
        for (int r = 0; r < length; r++)
            m_board[topOrLeft.r + r][topOrLeft.c] = symbol;
    }
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    bool exist = false;
    size_t i;
    if (shipId < 0 || shipId >= m_game.nShips()) // shipId outside range of ships
        return false;

    for (i = 0; i < existShip.size(); i++) // check is shipId match registered shipId
    {
        if (shipId == existShip[i]) // make exist to true and erase the ship
        {
            exist = true;
            break;
        }
    }
    if (!exist)
        return false;

    int length = m_game.shipLength(shipId);
    char symbol = m_game.shipSymbol(shipId);

    if (dir == HORIZONTAL)
    {
        for (int p = 0; p < length; p++) // if some of the symbol in cell of ship not equal to symbol
        {
            if (m_board[topOrLeft.r][topOrLeft.c + p] != symbol)
                return false;
        }
        for (int p = 0; p < length; p++) // change all symbol at cell of ship to clear
        {
            m_board[topOrLeft.r][topOrLeft.c + p] = '.';
        }
    }
    else
    {
        for (int p = 0; p < length; p++) // if some of the symbol in cell of ship not equal to symbol
        {
            if (m_board[topOrLeft.r + p][topOrLeft.c] != symbol)
                return false;
        }
        for (int p = 0; p < length; p++) // change all symbol at cell of ship to clear
        {
            m_board[topOrLeft.r + p][topOrLeft.c] = '.';
        }
    }

    existShip.erase(existShip.begin() + i);
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    if (shotsOnly)
    {
        cout << "  ";
        for (int c = 0; c < m_game.cols(); c++)
            cout << c;
        cout << endl;

        for (int r = 0; r < m_game.rows(); r++)
        {
            cout << r << " ";
            for (int c = 0; c < m_game.cols(); c++)
            {
                char curr = m_board[r][c];
                if (curr == 'X' || curr == 'o') // only display shot hit and miss
                    cout << curr;
                else // ignore the ship symbol
                    cout << '.';
            }
            cout << endl;
        }
    }
    else
    {
        cout << "  ";
        for (int c = 0; c < m_game.cols(); c++)
            cout << c;
        cout << endl;

        for (int r = 0; r < m_game.rows(); r++)
        {
            cout << r << " ";
            for (int c = 0; c < m_game.cols(); c++) // display everything
            {
                cout << m_board[r][c];
            }
            cout << endl;
        }
    }
}

// helper function
// if symbol not exist on board, the ship is destroyed
bool BoardImpl::check(char sym)
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == sym)
                return false;
        }
    }
    return true;
}

bool BoardImpl::attack(Point p, bool &shotHit, bool &shipDestroyed, int &shipId)
{
    shipDestroyed = false;
    char curr = m_board[p.r][p.c];

    if (p.r < 0 || p.r >= m_game.rows() || p.c < 0 || p.c >= m_game.cols()) // check point is outside the board
        return false;
    if (curr == 'X' || curr == 'o') // check cell attacked before
        return false;

    shotHit = false;
    size_t i;
    for (i = 0; i < existShip.size(); i++)
    {
        if (curr == m_game.shipSymbol(existShip[i])) // if the cell is some symbol of exist ship
        {
            shotHit = true;
            break;
        }
    }

    if (!shotHit)
    {
        m_board[p.r][p.c] = 'o';
        return true;
    }

    m_board[p.r][p.c] = 'X';

    shipDestroyed = check(curr);

    if (shipDestroyed) // if ship destroyed erase it from exist ship
    {
        shipId = existShip[i];
        existShip.erase(existShip.begin() + i);
    }
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    return existShip.empty(); // exist ship is empty if all ship destroyed
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.if(

Board::Board(const Game &g)
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

bool Board::attack(Point p, bool &shotHit, bool &shipDestroyed, int &shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}