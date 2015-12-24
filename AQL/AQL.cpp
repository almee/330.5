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
			cout << word->tag << endl;
		}
		word = static_cast<Word*>(tokenizer->scan());
	}
	//Parser parser(lex);
	//parser.aql_stmt();
	//parser.aql_stmt();
	//parser.aql_stmt();
	system("pause");
}