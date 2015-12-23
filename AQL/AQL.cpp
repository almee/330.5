#include "regex.h"
#include "Parser.h"

int main()
{
	Lexer* lex = new Lexer("test/test2.aql");
	Parser parser(lex);
	parser.aql_stmt();
	parser.aql_stmt();
	parser.aql_stmt();
	system("pause");
}