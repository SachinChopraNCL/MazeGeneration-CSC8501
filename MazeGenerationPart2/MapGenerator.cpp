#include "MapGenerator.h"
#include <iostream>
#include <vector>

/* Generate empty maze */
void MapGenerator::Generate() {

	char** maze = new char* [_height];

	for (int y = 0; y < _height; y++) {
		maze[y] = new char[_width];

		for (int x = 0; x < _width; x++) {
			maze[y][x] = WALL;
		}
	}
	_maze = maze;
}

/* Printing Maze */
void MapGenerator::WriteToBuffer() {
	for (int y = 0; y < _height; y++) {
		_mazeBuffer << "\n";
		for (int x = 0; x < _width; x++) {
			_mazeBuffer << _maze[y][x] << " ";
		}
	}
	_mazeBuffer << std::endl;
}

void MapGenerator::Run() {
	AddPaths();
	AddStartingArea();
	AddExits();
	RunPathFinding();
	AddWalls();
	ResolvePlayers();
	CleanUp();
}

void MapGenerator::CleanUp() {
	for (int y = 0; y < _height; y++) {
		delete[] _maze[y];
	}
	delete[] _maze;
}

/* Adding a 3x3 starting zone in the centre of the maze. Generates a random point one cell outside of the starting zone as the starting path
   ensuring a path is always coming out of the starting area. */
void MapGenerator::AddStartingArea() {
	for (int y = _midpointY; y < _midpointY + 3; y++) {
		for (int x = _midpointX; x < _midpointX + 3; x++) {
			_maze[y][x] = PATH;
		}
	}
	_maze[_midpointY + 1][_midpointX + 1] = START;
}


/* Generate a list of all possible exits, using the edges of the maze, discarding the corner tiles as they cannot be apart of the Maze.
   Then randomly assign these positions as exits, based on the number inputted by user */
void MapGenerator::AddExits() {
	/* Randomly positions the exits */
	std::vector<Position> allExits;
	for (int i = 2; i < _width - 2; i++) {
		allExits.push_back(Position(i, 0));
		allExits.push_back(Position(i, _height - 1));
	}
	for (int j = 2; j < _height - 2; j++) {
		allExits.push_back(Position(0, j));
		allExits.push_back(Position(_width - 1, j));
	}

    for (int k = 0; k < _exits; k++) {
		int index = rand() % allExits.size();
		Position currentPosition = allExits.at(index);
		_maze[currentPosition._y][currentPosition._x] = EXIT;
		allExits.erase(allExits.begin() + index);
		_currentExits.push_back(currentPosition);
	}

	ValidateExits(_currentExits);
}

/* Need to check if each exit is valid, i.e. the exit is adjacent to a path. This works by progressing the position in the adjacent direction of the wall */
void MapGenerator::ValidateExits(std::vector<Position> currentExits) {
	for (Position exitPosition : currentExits) {
		bool isValid = true;
		Position direction = Position(0, 0);
		if (exitPosition._y == 0) {
			direction = Position(0, 1);
		}
		else if (exitPosition._y == _height - 1) {
			direction = Position(0, -1);
		}
		else if (exitPosition._x == 0) {
			direction = Position(1, 0);
		}
		else if (exitPosition._x == _width - 1) {
			direction = Position(-1, 0);
		}

		Position validPosition = exitPosition + direction;

		if (_maze[validPosition._y][validPosition._x] != PATH) { isValid = false; }
		if (!isValid) { TunnelPath(exitPosition, direction); }
	}
}

/* If an exit is invalid, dig through the maze towards the centre, destroying any walls in it's way. This will keep destroying until a path is found*/
void MapGenerator::TunnelPath(Position exitPosition, Position direction) {
	Position centrePoint = Position(_midpointX, _midpointY);
	Position directionVector = centrePoint - exitPosition;

	int steps = CalculateStep(directionVector);

	float xIncrement = (float)directionVector._x / (float)steps;
	float yIncrement = (float)directionVector._y / (float)steps;


	Position currentPosition = exitPosition;
	currentPosition = currentPosition + direction;
	_maze[currentPosition._y][currentPosition._x] = PATH;


	float xPosition = currentPosition._x;
	float yPosition = currentPosition._y;
	bool foundPath = false;
	int xGridPosition = xPosition;
	int yGridPosition = yPosition;
	int previousXPosition;
	int previousYPosition;

	while (!foundPath) {
		previousXPosition = xGridPosition;
		previousYPosition = yGridPosition;

		xPosition += xIncrement;
		yPosition += yIncrement;

		xGridPosition = round(xPosition);
		yGridPosition = round(yPosition);

		if (previousXPosition != xGridPosition && previousYPosition != yGridPosition) {
			int xWallPosition = round(previousXPosition);
			int yWallPosition = round(previousYPosition);
			Position wallToDestroy = Position(xWallPosition, yWallPosition) + direction;
			_maze[wallToDestroy._y][wallToDestroy._x] = PATH;
		}

		if (_maze[yGridPosition][xGridPosition] == WALL) {
			_maze[yGridPosition][xGridPosition] = PATH;
		}
		else if (!(_maze[yGridPosition][xGridPosition] == WALL)) {
			foundPath = true;
		}
	}
}

/* Helper function that determines the dominant direction towards the starting point */
int MapGenerator::CalculateStep(Position directionVector) {
	float steps;
	if (std::abs(directionVector._x) > std::abs(directionVector._y)) {
		steps = std::abs(directionVector._x);
	}
	else {
		steps = std::abs(directionVector._y);
	}
	return steps;
}

/* using a variation of Prim's algorithm to produce a maze using a minimal spanning tree that is randomly expanded */
void MapGenerator::AddPaths() {
	int startingPosition[2] = { _midpointX + 1, _midpointY + 1 };
	Path startingPath = Path(startingPosition, startingPosition);
	_paths.push_back(startingPath);
	while (!_paths.empty()) {
		int indexToDelete = rand() % _paths.size();
		Path* currentPath = &_paths.at(indexToDelete);
		int parentXPosition = currentPath->_parentPosition[0];
		int parentYPosition = currentPath->_parentPosition[1];
		int childXPosition = currentPath->_childPosition[0];
		int childYPosition = currentPath->_childPosition[1];

		bool checkYBounds = parentXPosition != 0 && parentXPosition != _height - 1;
		bool checkXBounds = parentYPosition != 0 && parentYPosition != _width - 1;
		bool canAdd = checkYBounds && checkXBounds;

		_paths.erase(_paths.begin() + indexToDelete);

		if (_maze[parentYPosition][parentXPosition] == WALL) {
			_maze[parentYPosition][parentXPosition] = PATH;
			_maze[childYPosition][childXPosition] = PATH;
			if (canAdd) {
				if (parentXPosition >= 2 && _maze[parentYPosition][parentXPosition - 2] == WALL) {
					int newParentPosition[2] = { parentXPosition - 2, parentYPosition };
					int newChildPosition[2] = { parentXPosition - 1, parentYPosition };
					Path newPath = Path(newParentPosition, newChildPosition);
					_paths.push_back(newPath);
				}
				if (parentXPosition < (_width - 3) && _maze[parentYPosition][parentXPosition + 2] == WALL) {
					int newParentPosition[2] = { parentXPosition + 2, parentYPosition };
					int newChildPosition[2] = { parentXPosition + 1, parentYPosition };
					Path newPath = Path(newParentPosition, newChildPosition);
					_paths.push_back(newPath);

				}
				if (parentYPosition >= 2 && _maze[parentYPosition - 2][parentXPosition] == WALL) {
					int newParentPosition[2] = { parentXPosition, parentYPosition - 2 };
					int newChildPosition[2] = { parentXPosition, parentYPosition - 1 };
					Path newPath = Path(newParentPosition, newChildPosition);
					_paths.push_back(newPath);
				}
				if (parentYPosition < (_height - 3) && _maze[parentYPosition + 2][parentYPosition] == WALL) {
					int newParentPosition[2] = { parentXPosition, parentYPosition + 2 };
					int newChildPosition[2] = { parentXPosition, parentYPosition + 1 };
					Path newPath = Path(newParentPosition, newChildPosition);
					_paths.push_back(newPath);

				}

			}
		
		}

	}
}

/* The outside of the maze must be walled off. Only exits and walls should be on the outside of the maze*/
void MapGenerator::AddWalls() {
	for (int x = 0; x < _width; x++) {
		if (_maze[0][x] == PATH) { _maze[0][x] = WALL; }
		if (_maze[_height - 1][x] == PATH) { _maze[_height - 1][x] = WALL; }
	}
	for (int y = 0; y < _height; y++) {
		if (_maze[y][0] == PATH) { _maze[y][0] = WALL; }
		if (_maze[y][_width - 1] == PATH) { _maze[y][_width - 1] = WALL; }
	}

}

/* Run pathfinding */
void MapGenerator::RunPathFinding() {
	Pathfinder pathFinder = Pathfinder(_maze, _height, _width, _currentExits);
	_currentPlayers = pathFinder.Run();
}

void MapGenerator::ResolvePlayers() {
	
	for (auto& currentPlayer: _currentPlayers) {
		auto position = currentPlayer._positions.back();
		_maze[position._y][position._x] = PLAYER;
		currentPlayer._positions.pop_back();
		currentPlayer._lastPosition = position;
	}
	
	_mazeBuffer << "----------------------------------------------------------" << std::endl;
	_mazeBuffer << "MAZE START" << std::endl;
	WriteToBuffer();
	_mazeBuffer << "----------------------------------------------------------" << std::endl;


	bool finished = false;
	int  finishedCounter = 0;
	while (!_currentPlayers.empty() && !finished){
		_mazeBuffer << "----------------------------------------------------------" << std::endl;
		_mazeBuffer << "MAZE STATUS" << std::endl;
		for (auto& currentPlayer: _currentPlayers) {
			if (!currentPlayer._deadlocked && !currentPlayer._positions.empty()) {
				auto position = currentPlayer._positions.back();
				CheckIfStuck(position, &currentPlayer, _currentPlayers,finishedCounter + _deadlockCounter);
				if (!currentPlayer._stuck) {
					currentPlayer._positions.pop_back();
					_maze[currentPlayer._lastPosition._y][currentPlayer._lastPosition._x] = PATH;
					_maze[position._y][position._x] = PLAYER;
					currentPlayer._lastPosition = position;
				}
			}
			else if(!currentPlayer._deadlocked && !currentPlayer._hasFinished) {
				finishedCounter++;
				_maze[currentPlayer._lastPosition._y][currentPlayer._lastPosition._x] = PATH;
				currentPlayer._hasFinished = true;
				_mazeBuffer << "Player " << currentPlayer._id << " has completed the maze " << std::endl;
			}
		}
		WriteToBuffer();
		_mazeBuffer << "----------------------------------------------------------" << std::endl;
		if (finishedCounter + _deadlockCounter == _currentPlayers.size()) {
			finished = true;
		}
	}
	if (_deadlockCounter > 0) {
		_mazeBuffer << "Maze Completed - Remaining Players cannot complete Maze" << std::endl;

	}
	else {
		_mazeBuffer << "Maze Completed - All Player's have completed the Maze" << std::endl;
	}

	std::cout << _mazeBuffer.str();
}

void MapGenerator::CheckIfStuck(Node position, Player* currentPlayer, std::vector<Player> &currentPlayers, int totalCounter) {
	for (auto& collidingPlayer : currentPlayers) {
		if (collidingPlayer._id != currentPlayer->_id) {
			if (position._x == collidingPlayer._lastPosition._x && position._y == collidingPlayer._lastPosition._y && !collidingPlayer._hasFinished) {
				_mazeBuffer << "Player " << currentPlayer->_id << " is stuck and cannot move this turn" << std::endl;
				currentPlayer->_idStuckTo = collidingPlayer._id;
				currentPlayer->_stuck = true;
				if (collidingPlayer._idStuckTo == currentPlayer->_id ) {
					currentPlayer->_deadlocked = true;
					collidingPlayer._deadlocked = true;
					_deadlockCounter += 2;
					_mazeBuffer << "Player " << currentPlayer->_id << " and Player " << collidingPlayer._id << " are in a deadlock and can no longer proceed " << std::endl;
				}
				if (collidingPlayer._deadlocked && !currentPlayer->_deadlocked) {
					currentPlayer->_deadlocked = true;
					_deadlockCounter++;
					_mazeBuffer << "Player " << currentPlayer->_id << " and Player " << collidingPlayer._id << " are in a deadlock and can no longer proceed " << std::endl;
				}
				break; 
			}
			else {
				currentPlayer->_stuck = false;
			}
		}
	}

	if (totalCounter == currentPlayers.size() - 1) {
		currentPlayer->_stuck = false;
	}
}