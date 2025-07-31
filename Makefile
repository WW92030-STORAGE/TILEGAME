all:
	g++ main.cpp -o main -O3
	./main

usaco:
	g++ usaco.cpp -o usaco -O3
	./usaco

	# usaco.cpp should contain the contents of grid.h, followed by the contents of main.cpp, removing any unnecessary includes as needed. usaco.cpp should also modify the main method to take in a `dream.in` file (see `run_pinsky` for hints) and output a `dream.out` file with the correct solution (solver solution vector size minus one, as it asks for the number of moves, not the sequence of tiles.)

clean:
	rm usaco
	rm main