#pragma once
#include <string>
using namespace std;

struct SelectTemStr{
	string viewName;
	string columnName;
	string newName;
};

struct Atom {
	int type;
	bool ifCatch;
	string first, second;
	int min, max;
};