// Version of the program in one file and specifically tailored for the USACO problem. Everything is included -- even stuff that looks unnecessary.

#include <vector>
#include <set>
#include <map>
#include <cstdint>
#include <queue>
#include <string>
#include <algorithm>
#include <iostream>

// GRID.H

namespace GridTile
{
    typedef std::pair<int64_t, int64_t> Point;
    // Add and sub vectors

    Point add(Point a, Point b)
    {
        return {a.first + b.first, a.second + b.second};
    }
    Point sub(Point a, Point b)
    {
        return {a.first - b.first, a.second - b.second};
    }

    inline std::string disp(Point p)
    {
        return "(" + std::to_string(p.first) + ", " + std::to_string(p.second) + ")";
    }

    enum Tile : uint8_t
    {
        NIL = 0,
        START = 1,
        END = 2,
        RED = 3,
        ORANGE = 4,
        YELLOW = 5,
        GREEN = 6,
        BLUE = 7,
        VIOLET = 8,
        PINK = 9,
    };

    std::string abb(Tile t)
    {
        if (t == NIL)
            return ".";
        if (t == START)
            return "#";
        if (t == END)
            return "X";
        if (t == RED)
            return "R";
        if (t == ORANGE)
            return "O";
        if (t == YELLOW)
            return "Y";
        if (t == GREEN)
            return "G";
        if (t == BLUE)
            return "B";
        if (t == VIOLET)
            return "V";
        if (t == PINK)
            return "_";
        return ":";
    }

    struct State
    {
        Point pos = {0, 0};
        bool noBlue = false;
        Point sliding = {0, 0}; // if (0, 0) then not sliding

        State()
        {
        }

        State(const State &other)
        {
            pos = other.pos;
            noBlue = other.noBlue;
            sliding = other.sliding;
        }

        State(Point p, bool nb, Point s)
        {
            pos = p;
            noBlue = nb;
            sliding = s;
        }

        bool isSliding()
        {
            return sliding.first || sliding.second;
        }

        bool operator<(const State &other) const
        {
            if (pos != other.pos)
                return pos < other.pos;
            if (noBlue != other.noBlue)
                return noBlue < other.noBlue;
            if (sliding != other.sliding)
                return sliding < other.sliding;
            return false;
        }

        bool operator==(const State &other) const
        {
            if (pos != other.pos)
                return false;
            if (noBlue != other.noBlue)
                return false;
            if (sliding != other.sliding)
                return false;
            return true;
        }

        inline std::string to_string()
        {
            return "State[" + disp(pos) + ", " + std::to_string(noBlue) + ", " + disp(sliding) + "]";
        }
    };

    // Cardinal movement
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, 1, 0, -1};

    // Diagonal movement
    int ox[4] = {1, -1, -1, 1};
    int oy[4] = {1, 1, -1, -1};

    // Knight movement
    int nx[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    int ny[8] = {1, 2, 2, 1, -1, -2, -2, -1};
}

// Standard version where the orange tiles prevent you from accessing blue tiles.
class TileGrid
{
public:
    int X = 8;
    int Y = 6;

    std::vector<std::vector<GridTile::Tile>> board;
    std::set<GridTile::Point> hostileBlue; // Set of existing hostile blue tiles. This is BFS'ed upon a solve().
    void init()
    {
        board = std::vector<std::vector<GridTile::Tile>>(X, std::vector<GridTile::Tile>(Y, GridTile::PINK));
    }

    void defaultDiagonal()
    {
        board[0][0] = GridTile::START;
        board[X - 1][Y - 1] = GridTile::END;
    }

    TileGrid()
    {
        init();
    }

    TileGrid(int x, int y)
    {
        X = x;
        Y = y;
        init();
    }

    bool inBounds(int x, int y)
    {
        if (x < 0 || x >= X)
            return false;
        if (y < 0 || y >= Y)
            return false;
        return true;
    }
    bool inBounds(GridTile::Point p)
    {
        return inBounds(p.first, p.second);
    }

    GridTile::Tile get(int x, int y)
    {
        if (!inBounds(x, y))
            return GridTile::NIL;
        return board[x][y];
    }

    GridTile::Tile get(GridTile::Point p)
    {
        return get(p.first, p.second);
    }

    std::set<GridTile::Point> getTiles(GridTile::Tile tile)
    {
        std::set<GridTile::Point> res;
        for (int x = 0; x < X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                if (board[x][y] == tile)
                    res.insert({x, y});
            }
        }
        return res;
    }

    virtual GridTile::Point displace(GridTile::Point p, int i)
    {
        while (i < 0)
            i += 8;
        i &= 3;
        return {p.first + GridTile::dx[i], p.second + GridTile::dy[i]};
    }

    void initHostiles()
    {
        hostileBlue.clear();
        std::queue<GridTile::Point> q;
        for (auto i : getTiles(GridTile::YELLOW))
            q.push(i);

        std::set<GridTile::Point> vis;

        while (q.size())
        {
            GridTile::Point p = q.front();
            q.pop();
            vis.insert(p);

            for (int i = 0; i < 4; i++)
            {
                GridTile::Point next = displace(p, i);
                if (!inBounds(next))
                    continue;
                if (get(next) != GridTile::BLUE)
                    continue;
                if (vis.find(next) != vis.end())
                    continue;

                vis.insert(next);
                hostileBlue.insert(next);
                q.push(next);
            }
        }
    }

    // Get the possible reachable states from the current State in a single move. This can be overriden in subclasses of Grid for variants in game mechanics.
    virtual std::set<GridTile::State> getNeighbors(GridTile::State state)
    {
        // If we are sliding we can only go one way
        std::set<GridTile::State> res;
        std::queue<GridTile::State> v;
        if (state.isSliding())
        {
            GridTile::State next;
            next.noBlue = state.noBlue;
            next.pos = GridTile::add(state.pos, state.sliding);
            if (!inBounds(next.pos))
            {
                next.pos = state.pos;
                next.sliding = {0, 0};
            }
            v.push(next);
        }
        else
        {
            // Examine all possible routes
            for (int i = 0; i < 4; i++)
            {
                GridTile::State next;
                next.noBlue = state.noBlue;
                next.pos = displace(state.pos, i);
                if (inBounds(next.pos))
                    v.push(next);
            }
        }

        // Update statistics for all chosen neighbors

        while (v.size())
        {
            GridTile::State next = v.front();
            v.pop();

            GridTile::Point displacement = GridTile::sub(next.pos, state.pos);
            GridTile::Point inverseDisplacement = GridTile::sub(state.pos, next.pos);

            auto NT = get(next.pos);

            // If we land on an ORANGE TILE we add the restriction on blues
            if (NT == GridTile::ORANGE)
            {
                next.noBlue = true;
            }
            // If we land on a YELLOW or CONNECTING BLUE we immediately backtrack
            else if (NT == GridTile::YELLOW || hostileBlue.find(next.pos) != hostileBlue.end())
            {
                next.sliding = inverseDisplacement;
            }
            // If we land on a VIOLET TILE we slide forwards and take off the restriction
            else if (NT == GridTile::VIOLET)
            {
                next.sliding = displacement;
                next.noBlue = false;
            }
            // If we land on a RED TILE we halt motion prematurely
            else if (NT == GridTile::RED)
            {
                next.sliding = {0, 0};
                next.pos = state.pos;
            }
            // If we land on a BLUE TILE we do the checks:
            else if (NT == GridTile::BLUE)
            {
                if (next.noBlue)
                {
                    next.sliding = inverseDisplacement;
                }
                else
                {
                }
            }

            // All other colors mean nothing.

            res.insert(next);
        }

        return res;
    }

    std::vector<GridTile::State> solve(int x, int y, bool startingRestriction = false)
    {
        // initHostiles();

        std::map<GridTile::State, GridTile::State> parent;

        GridTile::State start = {{x, y}, startingRestriction, {0, 0}};
        parent.insert({start, start});

        std::queue<GridTile::State> q;
        q.push(start);

        bool solved = false;
        GridTile::State ending;
        while (q.size())
        {
            GridTile::State now = q.front();
            q.pop();
            for (auto i : getNeighbors(now))
            {
                if (parent.find(i) != parent.end())
                    continue;
                parent.insert({i, now});
                q.push(i);

                if (get(i.pos) == GridTile::END)
                {
                    ending = i;
                    solved = true;
                }
            }
            if (solved)
                break;
        }

        std::vector<GridTile::State> res;
        if (!solved)
            return res;

        res.push_back(ending);
        while (!(parent.at(ending) == ending))
        {
            ending = parent.at(ending);
            if (parent.at(ending).pos != ending.pos || (parent.at(ending) == ending)) res.push_back(ending);
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    std::string to_string()
    {
        std::string res = "Grid[" + std::to_string(X) + ", " + std::to_string(Y) + "]";
        for (int y = Y - 1; y >= 0; y--)
        {
            res = res + "[";
            for (int x = 0; x < X; x++)
            {
                if (x)
                    res = res + " ";
                res = res + GridTile::abb(get(x, y));
            }
            res = res + "]";
        }
        return res;
    }
    std::string sprintf()
    {
        std::string res = "Grid[" + std::to_string(X) + ", " + std::to_string(Y) + "]\n";
        for (int y = Y - 1; y >= 0; y--)
        {
            res = res + "[";
            for (int x = 0; x < X; x++)
            {
                if (x)
                    res = res + " ";
                res = res + GridTile::abb(get(x, y));
            }
            res = res + "]\n";
        }
        return res;
    }
};

// Grid with the orange tiles taking the restriction while violet tiles add the restriction. Blue tiles simply block instead of pushing back.
class PinskerGrid : public TileGrid
{
public:
    PinskerGrid() : TileGrid()
    {
    }

    PinskerGrid(int x, int y) : TileGrid(x, y)
    {
    }

    virtual std::set<GridTile::State> getNeighbors(GridTile::State state)
    {
        // If we are sliding we can only go one way
        std::set<GridTile::State> res;
        std::queue<GridTile::State> v;
        if (state.isSliding())
        {
            GridTile::State next;
            next.noBlue = state.noBlue;
            next.pos = GridTile::add(state.pos, state.sliding);
            if (!inBounds(next.pos))
            {
                next.pos = state.pos;
                next.sliding = {0, 0};
            }
            v.push(next);
        }
        else
        {
            // Examine all possible routes
            for (int i = 0; i < 4; i++)
            {
                GridTile::State next;
                next.noBlue = state.noBlue;
                next.pos = displace(state.pos, i);
                if (inBounds(next.pos))
                    v.push(next);
            }
        }

        // Update statistics for all chosen neighbors

        while (v.size())
        {
            GridTile::State next = v.front();
            v.pop();

            GridTile::Point displacement = GridTile::sub(next.pos, state.pos);
            GridTile::Point inverseDisplacement = GridTile::sub(state.pos, next.pos);

            auto NT = get(next.pos);

            // If we land on an ORANGE TILE we take out the restriction on blues
            if (NT == GridTile::ORANGE)
            {
                next.noBlue = false;
            }
            // If we land on a YELLOW or CONNECTING BLUE we immediately backtrack
            else if (NT == GridTile::YELLOW || hostileBlue.find(next.pos) != hostileBlue.end())
            {
                next.sliding = inverseDisplacement;
            }
            // If we land on a VIOLET TILE we slide forwards and add the restriction
            else if (NT == GridTile::VIOLET)
            {
                next.sliding = displacement;
                next.noBlue = true;
            }
            // If we land on a RED TILE we halt motion prematurely
            else if (NT == GridTile::RED)
            {
                next.sliding = {0, 0};
                next.pos = state.pos;
            }
            // If we land on a BLUE TILE we do the checks:
            else if (NT == GridTile::BLUE)
            {
                if (next.noBlue)
                {
                    next.sliding = {0, 0};
                    next.pos = state.pos;
                }
                else
                {
                }
            }

            // All other colors mean nothing.

            res.insert(next);
        }

        return res;
    }
};

// MAIN.CPP

#include <bits/stdc++.h>
using namespace std;

void run_pinsker() {
    ifstream fin("dream.in"); // USACO test cases
    int x, y;
    fin >> x >> y;

    PinskerGrid grid(x, y);

    for (int i = 0; i < grid.X; i++) {
        std::cout << i << "\n";
        for (int j = 0; j < grid.Y; j++) {
            fin >> x;
            if (x == 0) grid.board[i][j] = GridTile::RED;
            if (x == 1) grid.board[i][j] = GridTile::PINK;
            if (x == 2) grid.board[i][j] = GridTile::ORANGE;
            if (x == 3) grid.board[i][j] = GridTile::BLUE;
            if (x == 4) grid.board[i][j] = GridTile::VIOLET;
        }
    }
    std::cout << "INPUT DONE\n";
    grid.defaultDiagonal();

    auto v = grid.solve(0, 0, true);

    ofstream fout("dream.out");
    fout << (int64_t)(v.size()) - 1;

    std::cout << (int64_t)(v.size()) - 1 << "\n";

    fin.close();
    fout.close();
}

int main() {
    srand(42069);

	std::cout << "BEGIN\n";
	auto start = std::chrono::high_resolution_clock::now();


    run_pinsker();




	auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	int64_t count = duration.count();
    std::cout << "Time taken: " << count << " us" << std::endl;
	std::cout << "Time taken: " << count * 0.001 << " ms" << std::endl;
	std::cout << "Time taken: " << count * 0.000001 << " s" << std::endl;
    return 0;
}