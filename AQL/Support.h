#pragma once
#include "View.h"
#include "Tokenizer.h"
using namespace std;

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


struct extract_data{
	string reg;

	vector<Atom> atoms;
	vector<pair<int, int> > catchList;
	vector<pair<int, string> > group;

	void operator += (const extract_data &);
	extract_data();
	extract_data(string, vector<pair<int, string> >);
	extract_data(vector<Atom>);
};

vector<Span> getSpansByReg(string&, string&);

int getTokenIndexByEnd(vector<TokenOfTokenizer>&, int);

vector<pair<int, int> > getAllPossibleTokenMatch(vector<TokenOfTokenizer>&, int, int, int);

vector<Column> getColumnsByPattern(vector<pair<int, int> >&, vector<pair<int, int> >&);