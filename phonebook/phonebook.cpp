#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iterator>

int switcher(const char *);

int main(int argc, char * argv[])
{
	try
	{
		if (argc < 2)
			throw("ERROR // you didn't enter arguments");

		const char filename[] = "phonebook.dat";

		
		switch (switcher(argv[1]))
		{
		case 1:
			if (argc != 9)
				throw("ERROR ADD // wrong amount of arguments");
			std::cout << "number of records: " << add(filename, argv + 2) << std::endl;
			break;
		case 2:
			if (argc != 5)
				throw("ERROR EDIT // wrong amount of arguments");
			if (edit(filename, argv + 2)) {
				std::cout << "OK\n";
			}
			break;
		case 3:
			if (argc != 3)
				throw("ERROR DELETE // wrong amount of arguments");
			if (del(filename, argv + 2))
				std::cout << "OK\n";
			break;
		case 4:
			if (!(find(filename, argv + 2)))
				throw("ERROR FIND // ");
			break;
		case 5:
			if (argc == 4)			
				list(filename, argv + 2);
			else if (argc == 3) {
				const int recIdx = std::stoi(std::string(argv[2])) - 1;
				list(filename, recIdx);
			}
			else 
				throw("ERROR LIST // wrong amount of arguments");
			break;
		case 6:
			if (argc != 2)
				throw("ERROR COUNT // wrong amount of arguments");
			std::cout << "amount of records: " << count(filename) << std::endl;
			break;
		}
	}
	catch (const char * exemp)
	{
		std::cout << exemp << std::endl;
	}

	return 0;
}

int switcher(const char * argv1)
{
	std::string funcName = argv1;
	std::transform(funcName.begin(), funcName.end(), funcName.begin(), tolower);
	
	if (!funcName.compare("add"))						
		return 1;

	if (!funcName.compare("edit"))						
		return 2;

	if (!funcName.compare("del"))						
		return 3;

	if (!funcName.compare("find"))						
		return 4;

	if (!funcName.compare("list"))						
		return 5;

	if (!funcName.compare("count"))						
		return 6;

	throw("ERROR // invalid first argument");
}


