#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "Lexer.h"
using namespace std;

class TokenOfTokenizer {
public:
	string lexeme;
	int begin;
	int end;
	int getTag() { return tag; }
	TokenOfTokenizer(int t) {
		tag = t;
	}
	TokenOfTokenizer(char l, int b, int e) {
		lexeme.append(1,l);
		begin = b;
		end = e;
	}
	TokenOfTokenizer(string l, int b, int e) {
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
	TokenOfTokenizer* scan();
	vector<TokenOfTokenizer*> Tokenize();
private:
	char peek;
	int line;
	int charPosition;
	ifstream file;
	string fileToString();
protected:
	bool isdigit(char c);
	bool isletter(char c);
};