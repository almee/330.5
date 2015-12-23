#include "Lexer.h"

Lexer::Lexer(string f)
{
	file.open(f, ios::in);
	if (!file) {
		cout << "Could not find AQL file named: '" << f << "'" << endl;
		exit(1);
	}
	line = 1; peek = ' ';
	reserve(new Word(CREATE, "create"));
	reserve(new Word(VIEW, "view"));
	reserve(new Word(AS, "as"));
	reserve(new Word(OUTPUT, "output"));
	reserve(new Word(SELECT, "select"));
	reserve(new Word(FROM, "from"));
	reserve(new Word(EXTRACT, "extract"));
	reserve(new Word(REGEX, "regex"));
	reserve(new Word(ON, "on"));
	reserve(new Word(RETURN, "return"));
	reserve(new Word(AND, "and"));
	reserve(new Word(GROUP, "group"));
	reserve(new Word(PATTERN, "pattern"));
	reserve(new Word(TOKEN, "Token"));
}

Lexer::~Lexer()
{
}

void Lexer::reserve(Word* w) {
	words.insert(pair<string, Word*>(w->lexeme, w));
}

int Lexer::getline() {
	return line;
}

Token* Lexer::scan() {
	// 忽略空格
	for (;!file.eof(); file.get(peek)){
		if (peek == ' ' || peek == '\t') continue;
		else if (peek == '\n') line = line + 1;
		else break;
	}
	// 文本结束返回END
	if (file.eof()) {
		return new Token(END);
	}
	// 数字类型
	if (isdigit(peek)) {
		int v = 0;
		do {
			v = 10 * v + (peek - '0');
		} while (file.get(peek) && isdigit(peek));
		return new Num(v);
	}
	// 词语类型
	if (isletter(peek)){
		string s;
		do {
			s.append(1, peek);
		} while (file.get(peek) && (isletter(peek) || isdigit(peek)));
		Word* w = (Word*)words[s];
		if (w != NULL) return w;
		w = new Word(ID, s);
		words.insert(pair<string, Word*>(s, w));
		return w;
	}
	// 正则类型
	if (peek == '/') {
		string s;
		char record = ' ';		// 记录"\/"转义
		int count = 0;			// 记录'\'连续个数，避免"\\/"
		do {
			s.append(1, peek);
			record = peek;
			if (record == '\\') count++;
			else count = 0;
		} while (file.get(peek) && !(peek=='/'&&count%2==0));
		s.append(1, peek);
		peek = ' ';
		return new Word(REG, s);
	}
	Token* tok = new Token(peek);
	peek = ' ';
	return tok;
}

bool Lexer::isdigit(char c) {
	return (c >= '0' && c <= '9') ? true : false;
}

bool Lexer::isletter(char c) {
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') ? true : false;
}