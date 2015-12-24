#include "Tokenizer.h"

Tokenizer::Tokenizer(string f) {
	file.open(f, ios::in);
	if (!file) {
		cout << "Could not find AQL file named: '" << f << "'" << endl;
		exit(1);
	}
	line = 1; peek = ' ';
}

Tokenizer::~Tokenizer() {

}

int Tokenizer::getline() {
	return line;
}

Token* Tokenizer::scan() {
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

	if (isdigit(peek) || isletter(peek)) {
		string s;
		do {
			s.append(1,peek);
		} while (file.get(peek) && (isdigit(peek) || isletter(peek)));
		Word* w = new Word(ID, s);
		return w;
	}
	
	Token* tok = new Token(peek);
	peek = ' ';
	return tok;
}

bool Tokenizer::isdigit(char c) {
	return (c >= '0' && c <= '9') ? true : false;
}

bool Tokenizer::isletter(char c) {
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') ? true : false;
}