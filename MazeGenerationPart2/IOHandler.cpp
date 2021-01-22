#include "IOHandler.h"


void IOHandler::WriteToFile(std::string fileName,  std::string mazeBuffer) {
	std::ofstream FileToWrite(fileName);
	FileToWrite << mazeBuffer;
	FileToWrite.close();
}

void  IOHandler::ReadFromFile(std::string fileName) {

	std::fstream readFile(fileName, std::fstream::in);

	if (readFile) {
		std::string line;
		while (std::getline(readFile, line)) {
			std::cout << line << std::endl;
		}
	}
	else {
		std::cout << "ERROR - File does not exist" << std::endl;
	}
}