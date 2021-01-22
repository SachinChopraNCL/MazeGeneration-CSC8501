#pragma once
#include <cmath>
#include <vector>
#include <stack>
#include "Maze.h"
class Pathfinder {
private:
	int                   _height, _width;
	char**                _maze;
	Node**				  _nodeMap = nullptr;
	bool**                _closedList = nullptr;
	std::vector<Position> _exitPositions;
	
	std::vector<Node>	  MakePath(Node** nodeMap, Node destination);
	std::vector<Node>     PathFind(Node destination, Node start);
	
	int                   CalculateH(int x, int y, Node destination);
	
	bool                  IsDestination(int x, int y, Node destination);
	bool                  IsValid(int x, int y);
	
	void                  CreateNodeMap();
	void				  CleanUp();

public:
	                      Pathfinder(char** maze, int height, int width, std::vector<Position> exitPositions) :_maze(maze), _exitPositions(exitPositions), _height(height), _width(width) {}
	std::vector<Player>   Run();
};