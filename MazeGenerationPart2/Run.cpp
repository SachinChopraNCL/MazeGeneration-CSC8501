#include "IOHandler.h"
#include <time.h>
#include <iostream>
#include <limits>
#include "MapGenerator.h"

int readInput(int mazePerimeter) {
	int maxExit = (mazePerimeter == 0) ? INT_MAX : mazePerimeter - 7;
	int minLength = (mazePerimeter == 0) ? 5 : 0;
	int input = -1;
	bool valid = false;
	do {
		std::cin >> input;
		if (std::cin.good() && input > 0 && input < maxExit && input > minLength) {
			valid = true;
		}
		else {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input. Try Again!" << std::endl;
			std::cout << "Enter new value: ";
		}
	} while (!valid);
	return input;
}


int main(void) {
	srand(time(NULL));

	std::cout << "Welcome to the Maze Generator!" << std::endl;
	int choice = -1;

	while (choice != 3) {
		choice = -1;
		std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "What would you like to do ? Please select an option: " << std::endl;
		std::cout << "1. Generate a new maze" << std::endl;
		std::cout << "2. Load in a solved maze" << std::endl;
		std::cout << "3. Exit program" << std::endl;
		std::cout << "Please enter a choice: ";
		std::cin >> choice;

		if (choice == 1) {
			unsigned int width, height, exits;
			std::cout << "Enter width of the maze: ";
			width = readInput(0);
			std::cout << "Enter height of the maze: ";
			height = readInput(0);
			std::cout << "Enter number of Players for the maze: ";
			exits = readInput(width + width + height + height);
			MapGenerator* currentMaze = new MapGenerator(width, height, exits);
			currentMaze->Run();
			int saveChoice = -1;
			std::cout << std::endl;
			std::cout << "--------------------------------------------------------" << std::endl;
			std::cout << "Would you like to save this Maze? Please select an option: " << std::endl;
			std::cout << "1. Save" << std::endl;
			std::cout << "Any Number. Discard" << std::endl;
			std::cin >> saveChoice;
			if (saveChoice == 1) {
				IOHandler* writer = new IOHandler();
				std::string fileName;
				std::cout << "Please enter a file name: ";
				std::cin >> fileName;
				writer->WriteToFile(fileName, currentMaze->GetMazeBuffer());
				delete writer;
				writer = nullptr;
			}

			delete currentMaze;
			currentMaze = nullptr;
		}
		else if (choice == 2) {
			std::string fileToRead;
			IOHandler* reader = new IOHandler();
			std::cout << "Please Enter the file name to read from: ";
			std::cin >> fileToRead;
			reader->ReadFromFile(fileToRead);
			delete reader;
			reader = nullptr;
		}
	}

}