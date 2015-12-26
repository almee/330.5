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
	map<int, string> group;

	void operator += (const extract_data &);
	extract_data();
	extract_data(string, map<int, string>);
	extract_data(vector<Atom>);
};

class Support {
public:
	Support();
	Support(string, vector<View>, vector<Terms>);
	vector<Span> getSpansByReg(string&);
	int getTokenIndexByEnd(int);
	vector<pair<int, int> > getAllPossibleTokenMatch(int, int, int);
	vector<Column> getColumnsByPattern(vector<pair<int, int> >&, vector<pair<int, int> >&);
	vector<vector<pair<int, int>>> pattern(vector<Atom>&, int, vector<vector<pair<int, int>>>&);
	View findViewByName(string);
	int getViewNum();
	void insertView(View);
	View getViewByNum(int);
	void addAtomIndex();
	int getAtomIndex();
	string getContent(int, int);
	int getBeginOfToken(int);
	int getEndOfToken(int);
private:
	int atomIndex;
	string document;
	vector<View> viewSet;
	vector<Terms> tokenSet;
};