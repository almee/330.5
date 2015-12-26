#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "Lexer.h"
using namespace std;

class Terms {
public:
	string lexeme;
	int begin;
	int end;
	int getTag() { return tag; }
	Terms(int t) {
		tag = t;
	}
	Terms(char l, int b, int e) {
		lexeme.append(1,l);
		begin = b;
		end = e;
	}
	Terms(string l, int b, int e) {
		lexeme = l;
		begin = b;
		end = e;
	}
	Terms(int t, string l, int b, int e) {
		tag = t;
		lexeme = l;
		begin = b;
		end = e;
	}
private:
	int tag;
};

class Tokenizer {
public:
	Tokenizer(string f);
	~Tokenizer();
	int getline();
	Terms scan();
	vector<Terms> tokenize();
	string fileToString();
private:
	char peek;
	int line;
	int lastPosition;
	string fileString;
	ifstream file;
protected:
	bool isdigit(char c);
	bool isletter(char c);
};