#pragma once
#include <vector>
#include <string>
#include <iostream>
using namespace std;

struct Span {
	string data;
	int begin;
	int end;
	
	Span(const Span &);
	Span(string, int, int);
};

class Column {
public:
	Column();
	Column(const Column &);
	Column(string, vector<Span>);
	~Column();

	string getName();
	void setName(string);
	void setWidth(int);
	int getWidth();
	void insertSpan(Span);
	Span findSpanByIndex(int);


private:
	string name;
	int width;
	vector<Span> data;
};

class View {
public:
	View();
	View(string, vector<Column>);
	~View();

	int getRow();
	void print(string);
	string getName();
	void insertColumn(Column);
	void setName(string);
	Column findColumnByName(string);

private:
	string name;
	int row;
	vector<Column> data;
};