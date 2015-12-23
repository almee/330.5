#include "Parser.h"

Parser::Parser(Lexer* l) {
	lexer = l;
	lookahead = lexer->scan();
}

Parser::~Parser(){
}

Token* Parser::getLookahead() {
	return lookahead;
}

void Parser::match(int i) {
	if (lookahead->tag == i) {
		// 查看match的过程
		//if (lookahead->tag == NUM) {
		//	cout << static_cast<Num*>(lookahead)->value << endl;
		//}
		//else if (lookahead->tag >= ID &&lookahead->tag <= TOKEN) {
		//	cout << static_cast<Word*>(lookahead)->lexeme << endl;
		//}
		//else {
		//	cout << (char)lookahead->tag << endl;
		//}
		lookahead = lexer->scan();
	}
	else {
		cout << "syntax error: line " << lexer->getline() << endl;
		exit(1);
	}
}

// AQL statement
void Parser::aql_stmt() {
	View newView;
	pair<string, string> name;
	switch (lookahead->tag) {
	case CREATE:
		match(CREATE);
		newView = create_stmt();
		match(';');
		viewSet.push_back(newView);
		break;

	case OUTPUT:
		match(OUTPUT);
		name = output_stmt();
		match(';');
		if(name.second == "") {
			findViewByName(name.first).print(name.first);
		} else {
			findViewByName(name.first).print(name.second);
		}
		break;

	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined AQL statement" << endl;
		break;
	}
}

View Parser::create_stmt() {
	string name;
	vector<Column> data;
	match(VIEW); 
	name = static_cast<Word*>(lookahead)->lexeme;
	match(ID); 
	match(AS); 
	data = view_stmt();
	return View(name, data);
}

vector<Column> Parser::view_stmt() {
	switch (lookahead->tag) {
	case SELECT:
		match(SELECT); 
		return select_stmt();
	case EXTRACT:
		match(EXTRACT); extract_stmt();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined create statement" << endl;
		break;
	}
}

pair<string, string> Parser::output_stmt() {
	string name;
	match(VIEW);
	name = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	string otherName = alias();
	return make_pair(name, otherName);
}

string Parser::alias() {
	string result;
	if (lookahead->tag == AS) {
		match(AS);
		result =  static_cast<Word*>(lookahead)->lexeme;
		match(ID);
	}
	return result;
}

// select statement
vector<Column> Parser::select_stmt() {
	Span a("123", 1, 2);
	vector<Span> span;
	span.push_back(a);
	Column aa("aa", span);
	Column bb("bb", span);
	vector<Column> col;
	col.push_back(aa);
	col.push_back(bb);
	viewSet.push_back(View("gg", col));







	vector<Column> result;
	map<string, string> mapping;
	vector<SelectTemStr> allNewCol;
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

vector<SelectTemStr> Parser::select_list() {
	vector<SelectTemStr> allNewCol;
	allNewCol.push_back(select_item());
	while (lookahead->tag == ',') {
		match(',');
		allNewCol.push_back(select_item());
	}
	return allNewCol;
}

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

map<string, string> Parser::from_list() {
	map<string, string> result;
	result.insert(from_item());
	while (lookahead->tag == ',') {
		match(',');
		result.insert(from_item());
	}
	return result;
}

pair<string, string> Parser::from_item() {
	string realName, nickName;
	realName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	nickName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	return pair<string, string>(nickName, realName);

}

// extract statement - regular
void Parser::extract_stmt() {
	extract_spec(); match(FROM); from_list();
}

void Parser::extract_spec() {
	//直接返回
	switch (lookahead->tag) {
	case REGEX:
		match(REGEX); regex_spec();
		break;
	case PATTERN:
		match(PATTERN); pattern_spec();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined extract statement" << endl;
		break;
	}
}

void Parser::regex_spec() {
	string reg, name;
	reg = static_cast<Word*>(lookahead)->lexeme;
	match(REG);
	match(ON);
	column();
	name = name_spec()[0].second;

	/*
	**使用正则引擎获取span填入col中 从这里生成一个只有Columb的vector<Column>向上返回即可
	**否定上一行解决方法,与pattern在extract_spec内的返回类型冲突
	*/
}

void Parser::column() {
	match(ID); match('.'); match(ID);
}

vector<pair<int, string> > Parser::name_spec() {
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

vector<pair<int, string> > Parser::group_spec() {
	vector<pair<int, string> > result;
	result.push_back(single_group());
	while (lookahead->tag == AND) {
		match(AND);
		result.push_back(single_group());
	}
	return result;
}

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

// extract statement - pattern
void Parser::pattern_spec() {
	pattern_expr();	name_spec();
}

void Parser::pattern_expr() {
	pattern_pkg();
	while (1) {
		switch (lookahead->tag) {
		case '<':
			match('<');
			atom();
			if (lookahead->tag == '{') {
				int min, max;
				match('{');
				min = static_cast<Num*>(lookahead)->value;
				match(NUM);
				match(',');
				max = static_cast<Num*>(lookahead)->value;
				match(NUM);
				match('}');
			}
			break;
		default:
			break;
		}
	}
}

void Parser::pattern_pkg() {

}

void Parser::atom() {

}

void Parser::pattern_group() {

}

View Parser::findViewByName(string name_) {
	for (int i = 0; i < viewSet.size(); i++) {
		if (viewSet[i].getName() == name_)
			return viewSet[i];
	}
}