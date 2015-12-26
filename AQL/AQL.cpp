#include "AQL.h"

int main()
{
	Tokenizer tokenizer("test/Revenue.input");
	Lexer lexer("test/Revenue.aql");
	Parser parser(&lexer, tokenizer.tokenize(), tokenizer.getFileString());
	parser.program();
	system("pause");
}