#include "AQL.h"

int main()
{
	Lexer* lex = new Lexer("test/test3.aql");
	Parser parser(lex);
	parser.aql_stmt();
	parser.aql_stmt();
	parser.aql_stmt();
	parser.aql_stmt();
	system("pause");
}