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
		/*if (lookahead->tag == NUM) {
			cout << static_cast<Num*>(lookahead)->value << endl;
		}
		else if (lookahead->tag >= ID &&lookahead->tag <= TOKEN) {
			cout << static_cast<Word*>(lookahead)->lexeme << endl;
		}
		else {
			cout << (char)lookahead->tag << endl;
		}*/
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
	View newView;
	switch (lookahead->tag) {
	case CREATE:
		newView = create_stmt();
		match(';');
		viewSet.push_back(newView);
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
View Parser::create_stmt() {
	string name;
	vector<Column> data;
	match(CREATE);
	match(VIEW);
	name = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	match(AS);
	data = view_stmt();
	return View(name, data);
}

// view_stmt -> select_stmt | extract_stmt
vector<Column> Parser::view_stmt() {
	switch (lookahead->tag) {
	case SELECT:
		match(SELECT);
		return select_stmt();

	case EXTRACT:
		extract_stmt();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined create statement" << endl;
		system("pause");
	}
}

// output_stmt -> OUTPUT VIEM ID alias
pair<string, string> Parser::output_stmt() {
	string viewName, nickName;
	match(OUTPUT);
	match(VIEW);
	viewName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	nickName = alias();
	return pair<string, string>(viewName, nickName);
}

// alias ->AS ID | E
string Parser::alias() {
	string result;
	if (lookahead->tag == AS) {
		match(AS);
		result = static_cast<Word*>(lookahead)->lexeme;
		match(ID);
	}
	return result;
}


/*******************/
/* select statement*/
/*******************/

// select_stmt -> SELECT select_list FROM from_list
vector<Column> Parser::select_stmt() {
	vector<Column> result;
	map<string, string> mapping;
	vector<SelectTemStr> allNewCol;
	match(SELECT);
	allNewCol = select_list();
	match(FROM);
	mapping = from_list();
	for (int i = 0; i < allNewCol.size(); i++) {
		Column tmpCol(findViewByName(mapping[allNewCol[i].viewName]).findColumnByName(allNewCol[i].columnName));
		tmpCol.setName(allNewCol[i].newName);
		result.push_back(tmpCol);
	}
	return result;
}

// select_list -> select_item | select_list, select_item
vector<SelectTemStr> Parser::select_list() {
	vector<SelectTemStr> allNewCol;
	allNewCol.push_back(select_item());
	while (lookahead->tag == ',') {
		match(',');
		allNewCol.push_back(select_item());
	}
	return allNewCol;
}

// select_item -> ID.ID alias
SelectTemStr Parser::select_item(){
	SelectTemStr newOne;
	newOne.viewName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	match('.');
	newOne.columnName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	newOne.newName = alias();
	return newOne;
}

// from_list -> from_item | from_list, from_item
map<string, string> Parser::from_list() {
	map<string, string> result;
	result.insert(from_item());
	while (lookahead->tag == ',') {
		match(',');
		result.insert(from_item());
	}
	return result;
}

// from_item -> ID ID
pair<string, string> Parser::from_item() {
	string realName, nickName;
	realName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	nickName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	return pair<string, string>(nickName, realName);
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
vector<pair<int, string> >  Parser::name_spec() {
	string name;
	vector<pair<int, string> > result;
	switch (lookahead->tag){
	case AS:
		match(AS);
		name = static_cast<Word*>(lookahead)->lexeme;
		match(ID);
		result.push_back(pair<int, string>(0, name));
		return result;
		break;
	case RETURN:
		match(RETURN);
		return group_spec();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined name statement" << endl;
		break;
	}
}

// group_spec -> single_group | group_spec AND single_group
vector<pair<int, string> > Parser::group_spec() {
	vector<pair<int, string> > result;
	result.push_back(single_group());
	while (lookahead->tag == AND) {
		match(AND);
		result.push_back(single_group());
	}
	return result;
}

// single_group -> GROUP NUM AS ID
pair<int, string> Parser::single_group() {
	int number;
	string name;
	match(GROUP);
	number = static_cast<Num*>(lookahead)->value;
	match(NUM);
	match(AS);
	name = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	return pair<int, string>(number, name);
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


View Parser::findViewByName(string name_) {
	for (int i = 0; i < viewSet.size(); i++) {
		if (viewSet[i].getName() == name_)
			return viewSet[i];
	}
}