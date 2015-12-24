#include "Lexer.h"

using namespace std;

class Tokenizer {
public:
	Tokenizer(string f);
	~Tokenizer();
	int getline();
	Token* scan();
private:
	char peek;
	int line;
	ifstream file;
protected:
	bool isdigit(char c);
	bool isletter(char c);
};