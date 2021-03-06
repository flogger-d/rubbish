/*
// ./prog edit ...
	add F I O Db Adr Ph Notes -> N
	edit n fieldId fieldNewVal -> OK/error
	del n -> OK/error
	find HeldId regexp -> n |...| or NULL
	list //n1 n2 -> n |...| or NULL 
	count -> n
*/

#ifndef PCH_H
#define PCH_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <iterator>

struct Record;

char * field(Record & rcrd, const char* Id); 

// operators

int add(const char * filename, char * args[]);

int count(const char * filename);

bool edit(const char * filename, char * args[]);

bool find(const char * filename, char * args[]);

bool list(const char * filename, char * args[]);

bool list(const char * filename, const int recIdx);

bool del(const char * filename, char * args[]);

// main struct and supporting things

struct Record
{
	char firstname[50];
	char lastname[50];
	char patronomic[50];
	char birthday[50];
	char adress[50];
	char phone[20];
	char note[4096];
};

struct FieldMap
{
	char   Id[20];
	size_t offset;
} fields[] = {
#define RegField(FIELD) {#FIELD, offsetof(Record,  FIELD)}
	RegField(firstname),
	RegField(lastname),
	RegField(patronomic),
	RegField(birthday),
	RegField(adress),
	RegField(phone),
	RegField(note)
#undef RegField
};

class RecordLVal
{
public:
	RecordLVal(std::shared_ptr<FILE> f, size_t idx) : _f(f), _idx(idx) {}
	RecordLVal(const RecordLVal & other) : _f(other._f), _idx(other._idx) {}

	operator Record();
	void operator=(const Record & rcrd);
	RecordLVal & operator=(RecordLVal && other);

private:
	std::weak_ptr<FILE> _f;
	size_t _idx;
};

class RecordArray
{
public:
	RecordArray(const char * fname);
	~RecordArray() {};

	size_t max_idx() { return _max_idx; }
	RecordLVal operator[](size_t idx);

	class iterator : public std::iterator<std::forward_iterator_tag, Record>
	{
	public:
		iterator() : _f(), _idx(-1) {}
		iterator(const iterator & it) : _f(it._f), _idx(it._idx) {}
		iterator & operator=(const iterator & it);

		bool operator==(const iterator & it) { return _idx == it._idx && !_f.owner_before(it._f) && !it._f.owner_before(_f); }
		bool operator!=(const iterator & it) { return !operator==(it); }

		RecordLVal operator*();
		
		iterator & operator++() { _idx++; return *this; }
		iterator   operator++(int) { iterator prev = *this; _idx++; return prev; }
		iterator & operator+(int n) { _idx += n; return *this; }
	private:
		friend RecordArray;
		std::weak_ptr<FILE> _f;
		int _idx;
		iterator(std::shared_ptr<FILE> f, size_t idx) : _f(f), _idx(idx) {}
	};
	
	iterator begin() { return iterator(_f, 0); }
	iterator end() { return iterator(_f, _max_idx + 1); }
private:
	std::shared_ptr<FILE> _f;
	size_t _max_idx;
};
#endif 
