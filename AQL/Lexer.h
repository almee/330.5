#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <hash_map>
using namespace std;

static enum Tag
{
	END = 256, NUM, ID, CREATE, VIEW, AS,
	OUTPUT, SELECT, FROM, EXTRACT, REG, REGEX,
	ON, RETURN, AND, GROUP, PATTERN, TOKEN
};

class Token {
public:
	int tag;
	Token(int t) { tag = t; }
};

class Num: public Token {
public:
	int value;
	Num(int v):Token(NUM) { value = v; }
};

class Word : public Token {
public:
	string lexeme;
	Word(int type, string s) :Token(type) { lexeme = s; }
};

class Lexer {
public:
	Lexer(string f);
	~Lexer();
	void reserve(Word* w);
	int getline();
	Token* scan();
private:
	char peek;
	int line;
	ifstream file;
	hash_map<string, Word*> words;
protected:
	bool isdigit(char c);
	bool isletter(char c);
};

