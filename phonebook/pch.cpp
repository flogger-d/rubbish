#include "pch.h"
#include <cstring>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <string>
#include <regex>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <share.h>
#elif __linux
#include <io.h>
#endif

// add
int add(const char * filename, char * args[])
{

	for (int i = 0; i < 5; i++)
		if (strlen(args[i]) > 50)
			throw("invalid length of arguments");
	if (strlen(args[5]) > 20 || strlen(args[6]) > 4096)
		throw("invalid length of arguments");

	FILE * fst;
	if (!(fst = fopen(filename, "r+b")))
		throw("ERROR // can't open the file");

	Record temp;
	strncpy(temp.firstname, args[0], strlen(args[0]) + 1);
	strncpy(temp.lastname, args[1], strlen(args[1]) + 1);
	strncpy(temp.patronomic, args[2], strlen(args[2]) + 1);
	strncpy(temp.birthday, args[3], strlen(args[3]) + 1);
	strncpy(temp.adress, args[4], strlen(args[4]) + 1);
	strncpy(temp.phone, args[5], strlen(args[5]) + 1);
	strncpy(temp.note, args[6], strlen(args[6]) + 1);

	fseek(fst, 0, std::ios_base::end);
	fwrite(reinterpret_cast<char *>(&temp), sizeof(temp), 1, fst);

	fclose(fst);

	std::cout << "record added\n";
	return count(filename);
}

// count
int count(const char * filename)
{
	FILE * fst;
	if (!(fst = fopen(filename, "r+b")))
		throw("ERROR // can't open the file");


	fseek(fst, 0, SEEK_END);
	int pos{ static_cast<int>(ftell(fst)) };
	fclose(fst);

	return pos / sizeof(Record);
}

// edit
static const size_t FM_size = sizeof(fields) / sizeof(FieldMap);

char* field(Record & rcrd, const char * Id)
{
	FieldMap* fm_ptr = std::find_if(fields, fields + FM_size,
		[Id](const FieldMap& fm)->bool {return strcmp(Id, fm.Id) == 0; });

	return fm_ptr == fields + FM_size ?
		nullptr : reinterpret_cast<char*>(&rcrd) + fm_ptr->offset;
}

bool edit(const char * filename, char * args[])
{
	FILE * fst;
	if (!(fst = fopen(filename, "r+b")))
		throw("ERROR // can't open the file");

	int numOfRec = std::stoi(std::string(args[0]));
	if (numOfRec > count(filename))
		throw("ERROR // invalid argument: phonebook have less records, than you've enter");


	std::string fieldId = args[1];
	std::transform(fieldId.begin(), fieldId.end(), fieldId.begin(), tolower);

	int posOffset{ -1 };
	char * fieldValue{ nullptr };
	int fieldSize{ 0 };

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
		fieldSize = fieldSize(birthday);
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


	strncpy(fieldValue, args[2], strlen(args[2]) + 1);
	fseek(fst, (numOfRec - 1) * sizeof(Record) + posOffset, SEEK_SET);
	fwrite(fieldValue, sizeof(char), fieldSize, fst);

	delete[] fieldValue;

	fclose(fst);

	return true;
}

//find
bool find(const char * filename, char * args[])
{
	FILE * fst;
	if (!(fst = fopen(filename, "r+b")))
		throw("ERROR // can't open the file");

	std::string fieldId{ args[0] };
	std::regex regExp{ args[1] };
	
	int posOffset{ -1 };
	int fieldSize{ 0 };
#define fieldSize(field) sizeof(((struct Record *)0)->field)

	if (!fieldId.compare("firstname")) {
		posOffset = offsetof(Record, firstname);
		fieldSize = fieldSize(firstname);
	}
	else if (!fieldId.compare("lastname")) {
		posOffset = offsetof(Record, lastname);
		fieldSize = fieldSize(lastname);
	}
	else if (!fieldId.compare("patronomic")) {
		posOffset = offsetof(Record, patronomic);
		fieldSize = fieldSize(patronomic);
	}
	else if (!fieldId.compare("birthday")) {
		posOffset = offsetof(Record, birthday); // можно ли было сделать это как-нибудь покрасивее?
		fieldSize = fieldSize(birthday);
	}
	else if (!fieldId.compare("adress")) {
		posOffset = offsetof(Record, adress);
		fieldSize = fieldSize(adress);
	}
	else if (!fieldId.compare("phone")) {
		posOffset = offsetof(Record, phone);
		fieldSize = fieldSize(phone);
	}
	else if (!fieldId.compare("note")) {
		posOffset = offsetof(Record, note);
		fieldSize = fieldSize(note);
	}
#undef fieldSize
	if (posOffset == -1)
		throw("can't parse fieldId");


	for (int numOfRec = 0; numOfRec < count(filename); ++numOfRec)
	{
		char * fieldValue = new char[fieldSize];
		fseek(fst, numOfRec * sizeof(Record) + posOffset, SEEK_SET);
		fread(fieldValue, sizeof(char), fieldSize, fst);
		
		if (std::regex_search(fieldValue, regExp))
			list(filename, numOfRec);

		delete[] fieldValue;
	}

	fclose(fst);
	return true;
}

// list
bool list(const char * filename, char * args[])
{
	using std::cout; using std::endl;
	RecordArray rarr(filename);
	int recIdxBegin = std::stoi(std::string(args[0])) - 1;
	int recIdxEnd = std::stoi(std::string(args[1])); // w/o -1, cuz stl concept need it
	if (recIdxBegin > count(filename) || recIdxEnd > count(filename))
		throw("ERROR // invalid argument: phonebook have less records, than you ask for");
	
	for_each(rarr.begin() + recIdxBegin, rarr.begin() + recIdxEnd, [](const Record & rec)
	{
		cout << "Firstname: " << rec.firstname << endl;
		cout << "Lastname " << rec.lastname << endl;
		cout << "Patronomic: " << rec.patronomic << endl;
		cout << "Birthday: " << rec.birthday << endl;
		cout << "Adress: " << rec.adress << endl;
		cout << "Phone: " << rec.phone << endl;
		cout << "Note: " << rec.note << endl << endl;
	});
	return true;
}

bool list(const char * filename, const int recIdx)
{
	using std::cout; using std::endl;
	RecordArray rarr(filename);
	if (recIdx > count(filename) - 1)
		throw("ERROR // invalid argument: phonebook have less records, than you ask for");
	Record temp;
	temp = rarr[recIdx];

	cout << "Here is phonebook[" << recIdx + 1 << "] record:\n\n";
	cout << "Firstname: " << temp.firstname << endl;
	cout << "Lastname " << temp.lastname << endl;
	cout << "Patronomic: " << temp.patronomic << endl;
	cout << "Birthday: " << temp.birthday << endl;
	cout << "Adress: " << temp.adress << endl;
	cout << "Phone: " << temp.phone << endl;
	cout << "Note: " << temp.note << endl << endl;
	return true;
}

//delete
bool del(const char * filename, char * args[])
{
	RecordArray rarr(filename);
	int	idxOfRec = std::stoi(std::string(args[0])) - 1;
	if (idxOfRec > count(filename) - 1)
		throw("ERROR // invalid argument: phonebook have less records, than you've enter");

	for (int i = idxOfRec; i < count(filename) - 1; i++)
		rarr[i] = rarr[i + 1];
	rarr.~RecordArray();

#ifdef _WIN32
	int fh, result;
	unsigned int nbytes = BUFSIZ;

	// Open a file   
	if (_sopen_s(&fh, "phonebook.dat", _O_RDWR | _O_CREAT, _SH_DENYNO,
		_S_IREAD | _S_IWRITE) == 0)
	{
		if ((result = _chsize(fh, (count(filename) - 1) * sizeof(Record))) == 0) {
			_close(fh);
			return true;
		}
		else {
			_close(fh);
			return false;
		}
	}
#elif __linux
	if (!truncate("phonebook.dat", (count(filename) - 1) * sizeof(Record)) 
		return true;
		else 
		return false;
#endif
	throw("your operating system is not supporting");
}




// class methods
RecordArray::RecordArray(const char * fname) : _f(fopen(fname, "r+b"), fclose), _max_idx(0)
{
	if (!_f) throw "file not opened";
	fseek(_f.get(), 0L, SEEK_END);
	_max_idx = ftell(_f.get()) / sizeof(Record) - 1;
}

RecordLVal RecordArray::operator[](size_t idx)
{
	if (idx > _max_idx)
		throw "our of bounds";
	return RecordLVal(_f, idx);
}

RecordLVal::operator Record()
{
	if (std::shared_ptr<FILE> f = _f.lock()) {
		Record temp;
		fseek(f.get(), _idx * sizeof(Record), SEEK_SET);
		fread(&temp, sizeof(Record), 1, f.get());
		return temp;
	}
	else throw "file is closed";
}

void RecordLVal::operator=(const Record & rcrd)
{
	if (std::shared_ptr<FILE> f = _f.lock()) {
		fseek(f.get(), _idx * sizeof(Record), SEEK_SET);
		fwrite((char*)&rcrd, sizeof(Record), 1, f.get());
	}
	else throw "file is closed";
}

RecordLVal & RecordLVal::operator=(RecordLVal && other)
{
	Record val = other.operator Record();
	operator=(val);
	return *this;
}

RecordArray::iterator & RecordArray::iterator::operator=(const iterator & it)
{
	if (&it == this)
		return *this;
	else {
		_f = it._f;
		_idx = it._idx;
		return *this;
	}
}

RecordLVal RecordArray::iterator::operator*()
{
	if (!_f.lock()) throw "file is closed";
	return RecordLVal(_f.lock(), _idx);
}
