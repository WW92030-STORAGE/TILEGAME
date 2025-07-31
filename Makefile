all:
	g++ main.cpp -o main -O3
	./main

usaco:
	g++ usaco.cpp -o usaco -O3
	./usaco

	# usaco.cpp should contain the contents of grid.h, followed by the contents of main.cpp, removing any unnecessary includes as needed.

clean:
	rm usaco
	rm main