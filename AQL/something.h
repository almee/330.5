#pragma once
#include<string>
struct SelectTemStr{
	string viewName;
	string columnName;
	string newName;
};

struct Atom {
	int type;			// 0 for column, 1 for token, 2 for reg
	string first, second;
	int min, max;
};