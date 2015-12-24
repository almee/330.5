#include "AQL.h"

int main()
{
	Lexer* lex = new Lexer("test/test3.aql");
	Tokenizer* tokenizer = new Tokenizer("test/test2.aql");
	Word* word = static_cast<Word*>(tokenizer->scan());
	while(word->tag != END) {
		if(word->tag == ID) {
			cout << static_cast<Word*>(word)->lexeme << endl;
		} else {
			cout << static_cast<char>(word->tag) << endl;
		}
		word = static_cast<Word*>(tokenizer->scan());
	}
	system("pause");
}