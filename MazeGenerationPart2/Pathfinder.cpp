#include "Pathfinder.h"
#include <iostream>

/* Want to run pathfinding on all exits to the centre of the map */
std::vector<Player> Pathfinder::Run() {
	Node destination;
	destination._x = (_width / 2);
	destination._y = (_height / 2);
	Position direction(0, 0);
	std::vector<Player> playerList;
	int id = 0;

	for (auto currentExit : _exitPositions) {
		if (currentExit._y == 0) {
			direction = Position(0, 1);
		}
		else if (currentExit._y == _height - 1) {
			direction = Position(0, -1);
		}
		else if (currentExit._x == 0) {
			direction = Position(1, 0);
		}
		else if (currentExit._x == _width - 1) {
			direction = Position(-1, 0);
		}
		Node start;
		start._x = currentExit._x + direction._x;
		start._y = currentExit._y + direction._y;

		Player currentPlayer;
		
		currentPlayer._id = id;
		id++;

		currentPlayer._positions = PathFind(destination, start);
		playerList.push_back(currentPlayer);
	}

	return playerList;

}



/* Finding the destination node */
std::vector<Node> Pathfinder::PathFind(Node destination, Node start) {
	std::vector<Node> empty;

	if (!IsValid(destination._x, destination._y)) { std::cout << "ERROR HAS OCCURED" << std::endl; }
	if (IsDestination(start._x, start._y, destination)) { std::cout << "ERROR HAS OCCURED" << std::endl; }

	CreateNodeMap();

	int x = start._x;
	int y = start._y;
	_nodeMap[y][x]._fCost = 0.0;
	_nodeMap[y][x]._gCost = 0.0;
	_nodeMap[y][x]._hCost = 0.0;
	_nodeMap[y][x]._parentX = x;
	_nodeMap[y][x]._parentY = y;

	std::vector<Node> openList;
	openList.emplace_back(_nodeMap[y][x]);
	bool destinationFound = false;

	/* Want to find the node with the lowest f-cost to explore */
	while (!openList.empty() && (openList.size() < 20 * 30)) {
		Node node;
		do {
			float temp = FLT_MAX;
			std::vector<Node>::iterator itNode;
			for (std::vector<Node>::iterator it = openList.begin(); it != openList.end(); it = next(it)) {
				Node currentNode = *it;
				if (currentNode._fCost < temp) {
					temp = currentNode._fCost;
					itNode = it;
				}
			}
			node = *itNode;
			openList.erase(itNode);
		} while (IsValid(node._x, node._y) == false);

		x = node._x;
		y = node._y;
		_closedList[y][x] = true;
		Position searchRadius[4] = { Position(1,0), Position(-1,0), Position(0,1), Position(0,-1) };
		/* Searching every adjacent node */
		for (int i = 0; i < 4; i++) {
			Position search = searchRadius[i];
			int newX = search._x;
			int newY = search._y;
			int gNew, hNew, fNew;
			/* If the adjacent node is valid, then check if the destination and make a path from the current node map.
			   If not, add the node to the openlist with the new node if the new fcost is lower than than the current nodes
			   fcost, i.e the path is shorter*/
			if (IsValid(x + newX, y + newY)) {
				if (IsDestination(x + newX, y + newY, destination)) {
					_nodeMap[y + newY][x + newX]._parentX = x;
					_nodeMap[y + newY][x + newX]._parentY = y;
					destinationFound = true;
					return MakePath(_nodeMap, destination);
				}
				else if (_closedList[y + newY][x + newX] == false) {
					gNew = node._gCost + 1;
					hNew = CalculateH(x + newX, y + newY, destination);
					fNew = gNew + hNew;

					if (_nodeMap[y + newY][x + newX]._fCost == FLT_MAX || _nodeMap[y + newY][x + newX]._fCost > fNew) {
						_nodeMap[y + newY][x + newX]._fCost = fNew;
						_nodeMap[y + newY][x + newX]._gCost = gNew;
						_nodeMap[y + newY][x + newX]._hCost = hNew;
						_nodeMap[y + newY][x + newX]._parentX = x;
						_nodeMap[y + newY][x + newX]._parentY = y;
						openList.emplace_back(_nodeMap[y + newY][x + newX]);
					}
				}
			}
		}
	}
}

void Pathfinder::CleanUp() {
	for (int y = 0; y < _height; y++) {
		delete[] _nodeMap[y];
		delete[] _closedList[y];
	}
	delete[] _nodeMap;
	delete[] _closedList;
}

/* Initialise all nodes, and set all nodes as unvisited */
void Pathfinder::CreateNodeMap() {
	_closedList = new bool* [_height];
	_nodeMap = new Node * [_height];
	for (int y = 0; y < _height; y++) {
		_closedList[y] = new bool[_width];
		_nodeMap[y] = new Node[_width];
		for (int x = 0; x < _width; x++) {
			_nodeMap[y][x]._fCost = FLT_MAX;
			_nodeMap[y][x]._gCost = FLT_MAX;
			_nodeMap[y][x]._hCost = FLT_MAX;
			_nodeMap[y][x]._parentX = -1;
			_nodeMap[y][x]._parentY = -1;
			_nodeMap[y][x]._x = x;
			_nodeMap[y][x]._y = y;
			_closedList[y][x] = false;
		}
	}
}

/* Find the shortest path to the start, from the destination following the parent value of each node */
std::vector<Node> Pathfinder::MakePath(Node** nodeMap, Node destination) {
	int x = destination._x;
	int y = destination._y;
	std::vector<Node> path;
	while (!(nodeMap[y][x]._parentX == x && nodeMap[y][x]._parentY == y) && nodeMap[y][x]._x != -1 && nodeMap[y][x]._y != -1) {
		path.push_back(nodeMap[y][x]);
		int tempX = nodeMap[y][x]._parentX;
		int tempY = nodeMap[y][x]._parentY;
		x = tempX;
		y = tempY;
	}
	path.push_back(nodeMap[y][x]);
	path.erase(path.begin());
	CleanUp();
	return path;
}

/* Manhattan Distance */
int Pathfinder::CalculateH(int x, int y, Node destination) {
	return abs(x - destination._x) + abs(y - destination._y);
}

bool Pathfinder::IsDestination(int x, int y, Node destination) {
	if (x == destination._x && y == destination._y) { return true; }
	return false;
}

bool Pathfinder::IsValid(int x, int y) {
	if (x < 0 || y < 0 || x >= _width || y >= _height) { return false; }
	if (_maze[y][x] == PATH || _maze[y][x] == START || _maze[y][x] == ESCAPE) { return true; }
	return false;
}
