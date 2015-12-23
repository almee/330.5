#include "regex.h"
#include "Parser.h"

int main()
{
	Lexer* lex = new Lexer("test/test.aql");
	Parser parser(lex);
	parser.program();
	system("pause");
}