#pragma once
#include "View.h"
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


//只包含正则部分的内容
struct extract_data{
	string reg;
	vector<pair<int, string> > group;

	extract_data();
	extract_data(string, vector<pair<int, string> >);
};

vector<Span> getSpansByReg(string &, string &);