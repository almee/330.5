#include "AQL.h"

int main()
{
	Lexer* lex = new Lexer("test/test3.aql");
	
	Tokenizer* tokenizer = new Tokenizer("test/test2.aql");
	vector<TokenOfTokenizer*> totVector;
	totVector = tokenizer->Tokenize();
	for(int i = 0; i < totVector.size(); i++) {
		cout << totVector[i]->lexeme << "(" << totVector[i]->begin << "," << totVector[i]->end << ")" << endl;
	}
	
	system("pause");
}