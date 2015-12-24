#include "Parser.h"

Parser::Parser(Lexer* l) {
	lexer = l;
	lookahead = lexer->scan();
	document = "aaaa11111";
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
	pair<string, string> name;
	switch (lookahead->tag) {
	case CREATE:
		newView = create_stmt();
		match(';');
		viewSet.push_back(newView);
		break;

	case OUTPUT:
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
		return select_stmt();

	case EXTRACT:
		return extract_stmt();

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
vector<Column> Parser::extract_stmt() {
	extract_data data;
	vector<Column> result;
	map<string, string> mapping;
	match(EXTRACT);
	data = extract_spec();
	match(FROM);
	mapping = from_list();

	if (data.reg != "") {
		//regular
		vector<Span> spans = getSpansByReg(data.reg.substr(1, data.reg.size() - 2), document);
		Column newColumn(data.group[0].second, spans);
		result.push_back(newColumn);
	} else {
		//pattern
	}

	return result;
}

// extract_spec -> regex_spec | pattern_spec
extract_data Parser::extract_spec() {
	switch (lookahead->tag) {
	case REGEX:
		 return regex_spec();
	case PATTERN:
		return pattern_spec();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined extract statement" << endl;
		system("pause");
	}
}

// regex_spec -> REGEX REG ON column name_spec
extract_data Parser::regex_spec() {
	string reg;
	vector<pair<int, string> > group;
	match(REGEX);
	reg = static_cast<Word*>(lookahead)->lexeme; 
	match(REG);
	match(ON);
	column();
	group = name_spec();
	return extract_data(reg, group);
}

// column -> ID.ID
pair<string, string> Parser::column() {
	string viewName, columnName;
	viewName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	match('.'); 
	columnName = static_cast<Word*>(lookahead)->lexeme;
	match(ID);
	return pair<string, string>(viewName, columnName);
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
extract_data Parser::pattern_spec() {
	extract_data result;
	match(PATTERN);
	result += pattern_expr();
	result.group = name_spec();
	return result;
}

// pattern_expr -> pattern_pkg | pattern_expr pattern_pkg
extract_data Parser::pattern_expr() {
	extract_data result;
	result += pattern_pkg();
	while (lookahead->tag == '<' || lookahead->tag == REG || lookahead->tag == '(') {
		result += pattern_pkg();
	}
	return result;
}

// pattern_pkg -> atom | atom {NUM, NUM} | pattern_group
extract_data Parser::pattern_pkg() {
	int min, max;
	vector<Atom> result;
	switch (lookahead->tag) {
	case '<':
		result.push_back(atom());
		if (lookahead->tag == '{') {
			match('{');
			min = static_cast<Num*>(lookahead)->value;
			match(NUM);
			match(',');
			max = static_cast<Num*>(lookahead)->value;
			match(NUM);
			match('}');
			result[0].min = min;
			result[0].max = max;
		}
		return extract_data(result);
		break;
	case REG:
		result.push_back(atom());
		return extract_data(result);
		break;
	case '(':
		 return pattern_group();
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined pattern statement" << endl;
		system("pause");
	}
}

// atom -> <column> | <TOKEN> | REG
Atom Parser::atom() {
	Atom result;
	string reg;
	switch (lookahead->tag) {
	case '<':
		match('<');
		if (lookahead->tag == ID) {
			pair<string, string> twoStrings = column();
			result.type = 0;
			result.first = twoStrings.first;
			result.second = twoStrings.second;
		}
		else {
			match(TOKEN);
			result.type = 1;
			result.min = 1;
			result.max = 1;
		}
		match('>');
		break;
	case REG:
		reg = static_cast<Word*>(lookahead)->lexeme;
		match(REG);
		result.type = 0;
		result.first = reg;
		break;
	default:
		cout << "syntax error: line " << lexer->getline() << ": undefined atom statement" << endl;
		system("pause");
	}
	return result;
}

// pattern_group -> (pattern_expr)
extract_data Parser::pattern_group() {
	extract_data result;
	int left, right;
	match('(');
	/*  问题是如何获得这个min的值呢?
	**  声明一个初始值为0的变量,每执行一次atom加一
	**  表示已经读过的atom的个数,则在atom中经过了这么多个
	**  atom之后就是当前的atom??
	*/
	left = 0;
	result = pattern_expr();
	right = left + result.atoms.size();
	result.catchList.insert(result.catchList.begin(), pair<int, int>(left, right));
	match(')');
	return result;
}


View Parser::findViewByName(string name_) {
	for (int i = 0; i < viewSet.size(); i++) {
		if (viewSet[i].getName() == name_)
			return viewSet[i];
	}
}