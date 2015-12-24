#pragma once
#include "Support.h"
#include "Lexer.h"

class Parser
{
public:
	Parser(Lexer* l);
	~Parser();

	// AQL statement
	void aql_stmt();
	View create_stmt();
	vector<Column> view_stmt();
	pair<string, string> output_stmt();
	string alias();

	// select statement
	vector<Column> select_stmt();
	vector<SelectTemStr> select_list();
	SelectTemStr select_item();
	map<string, string> from_list();
	pair<string, string> from_item();

	// extract statement - regular
	vector<Column> extract_stmt();
	extract_data extract_spec();
	extract_data regex_spec();
	void column();
	vector<pair<int, string> > name_spec();
	vector<pair<int, string> > group_spec();
	pair<int, string> single_group();

	// extract statement - pattern
	void pattern_spec();
	void pattern_expr();
	void pattern_pkg();
	void atom();
	void pattern_group();

	void program();
	View findViewByName(string);
private:
	Lexer* lexer;
	Token* lookahead;
	void match(int i);
	vector<View> viewSet;
};


