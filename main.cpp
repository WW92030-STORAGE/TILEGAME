#include "TILEGAME.h"

#include <bits/stdc++.h>
using namespace std;

void simpletest() {
    TileGrid grid;
    grid.defaultDiagonal();

    

    for (int i = 0; i < 5; i++) grid.board[i][3] = GridTile::RED;


    auto v = grid.solve(0, 0);
    for (auto i : v) std::cout << i.to_string() << "\n";
    std::cout << v.size() << "\n";
}

void run_pinsker() {
    std::string FOLDER = "dream_gold_dec15/";
    ifstream fin(FOLDER + "15.in"); // USACO test cases
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

    if (grid.X * grid.Y <= 1000) std::cout << grid.sprintf() << "\n";

    auto v = grid.solve(0, 0, true);
    for (auto i : v) std::cout << i.to_string() << "\n";
    std::cout << (int64_t)(v.size()) - 1 << "\n";
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