#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iterator>

int main(int argc, char * argv[]);

int switcher(const char *);

int main(int argc, char * argv[])
{
	try
	{
		if (argc < 2)
			throw("ERROR // you didn't enter arguments");
		using std::ios_base;
		std::fstream gb("guysbook.dat", ios_base::in | ios_base::out | ios_base::binary);
		if (!gb.is_open())
			throw("ERROR // can't open the file");

		

		
		switch (switcher(argv[1]))
		{
		case 1:
			if (argc != 9)
				throw("ERROR ADD // wrong amount of arguments");
			std::cout << "number of records: " << add(gb, argv + 2) << std::endl;
			break;
		case 2:
			if (argc != 5)
				throw("ERROR EDIT // wrong amount of arguments");
			edit(gb, argv + 2);
			std::cout << "OK";
			break;
		case 3:
			if (argc != 3)
				throw("ERROR DELETE // wrong amount of arguments");
			del(gb, argv + 2);
			break;
		case 4:
			//find(argv);
			break;
		case 5:
			//list(argv);
			break;
		case 6:
			if (argc != 3)
				throw("ERROR DELETE // wrong amount of arguments");
			std::cout << "amount of records: " << count(gb) << std::endl;
			break;
		}
		gb.close();
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


