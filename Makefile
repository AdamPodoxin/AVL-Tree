main:
	g++ -Wall -std=c++11 -o main AVLTree.h main.cpp

test:
	g++ -Wall -std=c++11 -o tester AVLTree.h tester.cpp
