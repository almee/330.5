#include "Parser.h"

Parser::Parser(Lexer* l) {
	lexer = l;
	lookahead = lexer->scan();
}

Parser::~Parser()
{
}

void Parser::program() {
	while (lookahead->tag != END) {
		aql_stmt();
	}
}

void Parser::match(int i) {
	if (lookahead->tag == i) {
		// 通过查看match了解匹配过程
		if (lookahead->tag == NUM) {
			cout << static_cast<Num*>(lookahead)->value << endl;
		}
		else if (lookahead->tag >= ID &&lookahead->tag <= TOKEN) {
			cout << static_cast<Word*>(lookahead)->lexeme << endl;
		}
		else {
			cout << (char)lookahead->tag << endl;
		}
		lookahead = lexer->scan();
	}
	else {
		cout << "syntax error: line " << lexer->getline() << endl;
		system("pause");
	}
}


/*****************/
/* AQL statement */
/*****************/

// aql_stmt -> create_stmt; | output_stmt;
void Parser::aql_stmt() {
	switch (lookahead->tag) {
	case CREATE:
		create_stmt(); match(';');
		break;
	case OUTPUT:
		output_stmt(); match(';');
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined AQL statement" << endl;
		system("pause");
	}
}

// create_stmt -> CREATE VIEW ID AS view_stmt
void Parser::create_stmt() {
	match(CREATE); match(VIEW); match(ID); match(AS); view_stmt();
}

// view_stmt -> select_stmt | extract_stmt
void Parser::view_stmt() {
	switch (lookahead->tag) {
	case SELECT:
		select_stmt();
		break;
	case EXTRACT:
		extract_stmt();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined create statement" << endl;
		system("pause");
	}
}

// output_stmt -> OUTPUT VIEM ID alias
void Parser::output_stmt() {
	match(OUTPUT); match(VIEW); match(ID); alias();
}

// alias ->AS ID | E
void Parser::alias() {
	if (lookahead->tag == AS) {
		match(AS); match(ID);
	}
}


/*******************/
/* select statement*/
/*******************/

// select_stmt -> SELECT select_list FROM from_list
void Parser::select_stmt() {
	match(SELECT); select_list(); match(FROM); from_list();
}

// select_list -> select_item | select_list, select_item
void Parser::select_list() {
	select_item();
	while (lookahead->tag == ',') {
		match(',');
		select_item();
	}
}

// select_item -> ID.ID alias
void Parser::select_item(){
	match(ID); match('.'); match(ID); alias();
}

// from_list -> from_item | from_list, from_item
void Parser::from_list() {
	from_item();
	while (lookahead->tag == ',') {
		match(',');
		from_item();
	}
}

// from_item -> ID ID
void Parser::from_item() {
	match(ID); match(ID);
}


/*******************************/
/* extract statement - regular */
/*******************************/

// extract_stmt -> EXTRACT extract_spec FROM from_list
void Parser::extract_stmt() {
	match(EXTRACT); extract_spec(); match(FROM); from_list();
}

// extract_spec -> regex_spec | pattern_spec
void Parser::extract_spec() {
	switch (lookahead->tag) {
	case REGEX:
		regex_spec();
		break;
	case PATTERN:
		pattern_spec();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined extract statement" << endl;
		system("pause");
	}
}

// regex_spec -> REGEX REG ON column name_spec
void Parser::regex_spec() {
	match(REGEX); match(REG); match(ON); column(); name_spec();
}

// column -> ID.ID
void Parser::column() {
	match(ID); match('.'); match(ID);
}

// name_spec -> AS ID | RETURN group_spec
void Parser::name_spec() {
	switch (lookahead->tag){
	case AS:
		match(AS); match(ID);
		break;
	case RETURN:
		match(RETURN); group_spec();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined name statement" << endl;
		system("pause");
	}
}

// group_spec -> single_group | group_spec AND single_group
void Parser::group_spec() {
	single_group();
	while (lookahead->tag == AND) {
		match(AND);
		single_group();
	}
}

// single_group -> GROUP NUM AS ID
void Parser::single_group() {
	match(GROUP); match(NUM); match(AS); match(ID);
}


/*******************************/
/* extract statement - pattern */
/*******************************/

// pattern_spec -> PATTERN pattern_expr name_spec
void Parser::pattern_spec() {
	match(PATTERN); pattern_expr(); name_spec();
}

// pattern_expr -> pattern_pkg | pattern_expr pattern_pkg
void Parser::pattern_expr() {
	pattern_pkg();
	while (lookahead->tag == '<' || lookahead->tag == REG || lookahead->tag == '(') {
		pattern_pkg();
	}
}

// pattern_pkg -> atom | atom {NUM, NUM} | pattern_group
void Parser::pattern_pkg() {
	switch (lookahead->tag) {
	case '<':
		atom();
		if (lookahead->tag == '{') {
			match('{'); match(NUM); match(','); match(NUM); match('}');
		}
		break;
	case REG:
		atom();
		break;
	case '(':
		pattern_group();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined pattern statement" << endl;
		system("pause");
	}
}

// atom -> <column> | <TOKEN> | REG
void Parser::atom() {
	switch (lookahead->tag) {
	case '<':
		match('<');
		if (lookahead->tag == ID) {
			column();
		}
		else {
			match(TOKEN);
		}
		match('>');
		break;
	case REG:
		match(REG);
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined atom statement" << endl;
		system("pause");
	}
}

// pattern_group -> (pattern_expr)
void Parser::pattern_group() {
	match('('); pattern_expr(); match(')');
}
