#include "Tokenizer.h"

Tokenizer::Tokenizer(string f) {
	file.open(f, ios::in);
	if (!file) {
		cout << "Could not find AQL file named: '" << f << "'" << endl;
		exit(1);
	}
	line = 1; peek = ' '; charPosition = -1;
}

Tokenizer::~Tokenizer() {

}

int Tokenizer::getline() {
	return line;
}

TokenOfTokenizer* Tokenizer::scan() {
	int begin = 0, end = 0;
	// 忽略空格
	for (;!file.eof(); file.get(peek)){
		if (peek == ' ' || peek == '\t') {
			charPosition++;
			continue;
		}
		else if (peek == '\n') {
			charPosition++;
			line = line + 1;
		}
		else break;
	}
	// 文本结束返回END
	if (file.eof()) {
		return new TokenOfTokenizer(END);
	}

	if (isdigit(peek) || isletter(peek)) {
		string s;
		begin = charPosition;
		do {
			s.append(1,peek);
			charPosition++;
		} while (file.get(peek) && (isdigit(peek) || isletter(peek)));
		end = charPosition;
		TokenOfTokenizer* tot = new TokenOfTokenizer(s, begin, end);
		return tot;
	}
	
	begin = charPosition;
	end = ++charPosition;
	TokenOfTokenizer* tok = new TokenOfTokenizer(peek, begin, end);
	peek = ' ';
	return tok;
}

vector<TokenOfTokenizer*> Tokenizer::Tokenize() {
	vector<TokenOfTokenizer*> tokenVector;
	TokenOfTokenizer* tot = scan();
	while(tot->getTag() != END) {
		tokenVector.push_back(tot);
		tot = scan();
	}
	return tokenVector;
}

bool Tokenizer::isdigit(char c) {
	return (c >= '0' && c <= '9') ? true : false;
}

bool Tokenizer::isletter(char c) {
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') ? true : false;
}