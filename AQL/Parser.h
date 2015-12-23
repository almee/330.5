#pragma once
#include "Lexer.h"

class Parser
{
public:
	Parser(Lexer* l);
	~Parser();
	// AQL statement
	void aql_stmt();
	void create_stmt();
	void view_stmt();
	void output_stmt();
	void alias();
	// select statement
	void select_stmt();
	void select_list();
	void select_item();
	void from_list();
	void from_item();
	// extract statement - regular
	void extract_stmt();
	void extract_spec();
	void regex_spec();
	void column();
	void name_spec();
	void group_spec();
	void single_group();
	// extract statement - pattern
	void pattern_spec();
	void pattern_expr();
	void pattern_pkg();
	void atom();
	void pattern_group();

	void program();
private:
	Lexer* lexer;
	Token* lookahead;
	void match(int i);
};

