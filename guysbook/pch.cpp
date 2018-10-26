#include "pch.h"
#include <cstring>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>

static const size_t FM_size = sizeof(fields) / sizeof(FieldMap);

char* field(Record & rcrd, const char* Id)
{
	FieldMap* fm_ptr = std::find_if(fields, fields + FM_size,
		[Id](const FieldMap& fm)->bool {return strcmp(Id, fm.Id) == 0; });

	return fm_ptr == fields + FM_size ?
		nullptr : reinterpret_cast<char*>(&rcrd) + fm_ptr->offset;
}

int add(std::fstream & fst, char * args[])
{
	for (int i = 0; i < 5; i++)
		if (strlen(args[i]) > 50)
			throw("invalid length of arguments");
	if (strlen(args[5]) > 20 || strlen(args[6]) > 4096)
		throw("invalid length of arguments");

	Record temp;
	strncpy(temp.firstname, args[0], strlen(args[0]));
	strncpy(temp.lastname, args[1], strlen(args[1]));
	strncpy(temp.patronomic, args[2], strlen(args[2]));
	strncpy(temp.birthday, args[3], strlen(args[3])); // наверняка был метод лучше инициализировать этот массив?
	strncpy(temp.adress, args[4], strlen(args[4]));
	strncpy(temp.phone, args[5], strlen(args[5]));
	strncpy(temp.note, args[6], strlen(args[6]));

	fst.seekp(0, std::ios_base::end);
	fst.write(reinterpret_cast<char *>(&temp), sizeof(temp));

	return count(fst);
}

int count(std::fstream & fst)
{
	fst.seekg(0, fst.end);
	return static_cast<int>(fst.tellg()) / sizeof(Record);
}

bool edit(std::fstream & fst, char * args[])
{
	int numOfRec = *args[0] - 48;
	if (numOfRec > count(fst))
		throw("ERROR // invalid argument: guysbook have less records, than you've enter");
	
	
	std::string fieldId = args[1];
	std::transform(fieldId.begin(), fieldId.end(), fieldId.begin(), tolower);
	
	int posOffset = -1;
	char * fieldValue = nullptr;
	int fieldSize = 0;
#define fieldSize(field) sizeof(((struct Record *)0)->field)

	if (!fieldId.compare("firstname")) {
		posOffset = offsetof(Record, firstname);
		fieldSize = fieldSize(firstname);
		fieldValue = new char[fieldSize];
	} else if (!fieldId.compare("lastname")) {
		posOffset = offsetof(Record, lastname);
		fieldSize = fieldSize(lastname);
		fieldValue = new char[fieldSize];
	} else if (!fieldId.compare("patronomic")) {
		posOffset = offsetof(Record, patronomic);
		fieldSize = fieldSize(patronomic);
		fieldValue = new char[fieldSize];
	} else if (!fieldId.compare("birthday")) {
		posOffset = offsetof(Record, birthday); // можно ли было сделать это как-нибудь покрасивее?
		fieldSize = fieldSize(patronomic);
		fieldValue = new char[fieldSize];
	} else if (!fieldId.compare("adress")) {
		posOffset = offsetof(Record, adress);
		fieldSize = fieldSize(adress);
		fieldValue = new char[fieldSize];
	} else if (!fieldId.compare("phone")) {
		posOffset = offsetof(Record, phone);
		fieldSize = fieldSize(phone);
		fieldValue = new char[fieldSize];
	} else if (!fieldId.compare("note")) {
		posOffset = offsetof(Record, note);
		fieldSize = fieldSize(note);
		fieldValue = new char[fieldSize];
	}
#undef fieldSize

	
	if (posOffset == -1)
		throw("can't parse fieldId");


	strncpy(fieldValue, args[2], strlen(args[2]));
	fst.seekp((numOfRec - 1) * sizeof(Record) + posOffset);
	fst.write(fieldValue, fieldSize);

	delete[] fieldValue;

	return true;
}
/*
bool del(std::fstream & fst, char * args[])
{
	int numOfRec = *args[0] - 48;
	if (numOfRec > count(fst))
		throw("ERROR // invalid argument: guysbook have less records, than you've enter");

	std::vector<Record> records = std::vector<Record>(std::istreambuf_iterator<Record>(fst), std::istreambuf_iterator<Record>());
	records.erase(records.begin() + (numOfRec - 1));

	std::cout << records.size() << std::endl;
	


}*/

