#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

class IOHandler {
private:
	std::string _mazeSet;
public:
	IOHandler() {};

	void WriteToFile(std::string fileName, std::string mazeBuffer);
	void ReadFromFile(std::string fileToRead);
};