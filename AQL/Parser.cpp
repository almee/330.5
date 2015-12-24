#include "Parser.h"

Parser::Parser(Lexer* l, string document_, vector<View> viewSet_, vector<TokenOfTokenizer> tokenSet_) {
	lexer = l;
	lookahead = lexer->scan();
	support = Support(document_, viewSet_, tokenSet_);
	/*
	Span a("123", 1, 2);
	Span b("321", 5, 6);
	vector<Span> span;
	span.push_back(a);
	span.push_back(b);
	Column aa("aa", span);
	Column bb("bb", span);
	vector<Column> Col;
	Col.push_back(aa);
	Col.push_back(bb);
	View gg("gg", Col);
	viewSet.push_back(gg);
	*/
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
		support.insertView(newView);
		break;

	case OUTPUT:
		name = output_stmt();
		match(';');
		if(name.second == "") {
			support.findViewByName(name.first).print(name.first);
		} else {
			support.findViewByName(name.first).print(name.second);
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
		Column tmpCol(support.findViewByName(mapping[allNewCol[i].viewName]).findColumnByName(allNewCol[i].columnName));
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
	int left, right;
	extract_data data;
	Column newColumn;
	vector<Column> result;
	map<string, string> mapping;
	vector<pair<int, int>> thisPath;
	vector<vector<pair<int, int>>> allPath;
	match(EXTRACT);
	data = extract_spec();
	match(FROM);
	mapping = from_list();

	if (data.reg != "") {
		//regular
		vector<Span> spans = support.getSpansByReg(data.reg.substr(1, data.reg.size() - 2));
		Column newColumn(data.group[0], spans);
		result.push_back(newColumn);
	} else {
		//pattern
		for (int i = 0; i < data.atoms.size(); i++)				//获取所有可能情况
			allPath = support.pattern(data.atoms, i, allPath);

		for (int i = 0; i <= data.catchList.size(); i++) {			//给每一个捕获创建列,还要补上一个整体的
			result.push_back(Column(data.group[i]));
		}

		for (int i = 0; i < allPath.size(); i++) {					//逐条处理匹配
			thisPath = allPath[i];

			left = thisPath[0].first;								//完整的pattern
			right = thisPath[thisPath.size() - 1].second;
			result[0].insertSpan(Span(support.getContent(left, right), left, right));

			for (int j = 0; j < data.catchList.size(); j++) {				//捕获处理
				left = support.getBeginOfToken(data.catchList[j].first);					
				right = support.getEndOfToken(data.catchList[j].second);
				result[j + 1].insertSpan(Span(support.getContent(left, right), left, right));
			}

		}
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
	map<int, string> group;
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
map<int, string>  Parser::name_spec() {
	string name;
	map<int, string> result;
	switch (lookahead->tag){
	case AS:
		match(AS);
		name = static_cast<Word*>(lookahead)->lexeme;
		match(ID);
		result[0] = name;
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
map<int, string> Parser::group_spec() {
	map<int, string> result;
	pair<int, string> data;
	data = single_group();
	result[data.first] = data.second;
	while (lookahead->tag == AND) {
		match(AND);
		data = single_group();
		result[data.first] = data.second;
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
	support.addAtomIndex();
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
	left = support.getAtomIndex();
	result = pattern_expr();
	right = left + result.atoms.size();
	result.catchList.insert(result.catchList.begin(), pair<int, int>(left, right));
	match(')');
	return result;
}


