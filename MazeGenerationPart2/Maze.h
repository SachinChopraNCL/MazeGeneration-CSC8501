#pragma once
const char PATH = ' ';
const char WALL = 'X';
const char EXIT = 'E';
const char START = 'F';
const char ESCAPE = 'o';
const char PLAYER = 'p';

struct Node {
	int   _x, _y;
	int   _parentX, _parentY;
	float _gCost, _hCost, _fCost;

	bool operator<(const Node& rhs) {
		return this->_fCost < rhs._fCost;
	}
};

struct Position {
	int _x;
	int _y;
	Position(int x, int y) :_x(x), _y(y) {}
	Position operator+(const Position& rhs) {
		Position position = Position(this->_x + rhs._x, this->_y + rhs._y);
		return position;
	}
	Position operator-(const Position& rhs) {
		Position position = Position(this->_x - rhs._x, this->_y - rhs._y);
		return position;
	}
};

struct Player {
	int _id = -1;
	int _idStuckTo;
	bool _stuck = false;
	bool _deadlocked = false;
	bool _hasFinished = false;
	Node _lastPosition;
	std::vector<Node> _positions;
};
