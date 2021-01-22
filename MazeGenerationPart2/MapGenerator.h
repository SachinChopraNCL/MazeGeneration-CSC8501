#pragma once
#include "Pathfinder.h"
#include "Maze.h"
#include <vector>
#include <string>  
#include <iostream> 
#include <sstream> 
class MapGenerator {
	struct  Path {
		int _parentPosition[2];
		int _childPosition[2];
		bool _visited = false;
		Path(int parentPosition[2], int childPosition[2]) {
			_parentPosition[0] = parentPosition[0];
			_parentPosition[1] = parentPosition[1];
			_childPosition[0] = childPosition[0];
			_childPosition[1] = childPosition[1];
		}
	};

private:
	std::vector<Path>    _paths;
	std::vector<Position> _currentExits;
	std::vector<Player>	  _currentPlayers;
	unsigned const int    _width, _height;
    int					  _midpointX, _midpointY;
	unsigned const int    _exits;
	int					  _deadlockCounter = 0;
	char**				  _maze;
	std::stringstream     _mazeBuffer;

	void  AddStartingArea();
	void  AddPaths();
	void  AddExits();
	void  ValidateExits(std::vector<Position> currentExits);
	void  TunnelPath(Position exitPosition, Position direction);
	void  AddWalls();
	void  Generate();
	void  RunPathFinding();
	void  ResolvePlayers();
	void  CheckIfStuck(Node position, Player* currentPlayer, std::vector<Player> &currentPlayers, int totalCounter);
	void  CleanUp();
	void  WriteToBuffer();

	int   CalculateStep(Position directionVector);

public:
	            MapGenerator(unsigned int width, unsigned int height, unsigned int exits) :_width(width), _height(height), _exits(exits), _maze(nullptr), _midpointX((_width / 2) - 1), _midpointY((_height / 2) - 1) { Generate(); }
	void		Run();
	std::string GetMazeBuffer() { return _mazeBuffer.str(); }
};