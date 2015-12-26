#include "Tokenizer.h"

Tokenizer::Tokenizer(string f) {
	file.open(f, ios::in);
	if (!file) {
		cout << "Could not find AQL file named: '" << f << "'" << endl;
		exit(1);
	}
	line = 1; peek = ' '; lastPosition = 0;
	fileString = fileToString();
}

Tokenizer::~Tokenizer() {

}

int Tokenizer::getline() {
	return line;
}

Terms Tokenizer::scan() {
	int begin = 0, end = 0;
	// 忽略空格
	for (int i = lastPosition; i < fileString.length(); i++){
		peek = fileString[i];
		if (peek == ' ' || peek == '\t') {
			lastPosition = i;
			continue;
		}
		else if (peek == '\n') {
			lastPosition = i;
			line = line + 1;
		}
		else if (isdigit(peek) || isletter(peek)) {
			string s;
			begin = i;
			do {
				s.append(1,peek);
				i++;
				peek = fileString[i];
			} while (isdigit(peek) || isletter(peek));
			end = i;
			Terms tms(s, begin, end);
			lastPosition = i;
			return tms;
		} else {
			begin = i;
			end = ++i;
			Terms tms(peek, begin, end);
			peek = ' ';
			lastPosition = i;
			return tms;
		}
	}
	// 文本结束返回END
	return Terms(END);
}

vector<Terms> Tokenizer::tokenize() {
	vector<Terms> tokenVector;
	Terms tms = scan();
	while(tms.getTag() != END) {
		tokenVector.push_back(tms);
		tms = scan();
	}
	return tokenVector;
}

string Tokenizer::getFileString() {
	return fileString;
}

string Tokenizer::fileToString() {
	string str;
	for (;!file.eof(); file.get(peek)){
		str.append(1, peek);
	}
	if(str[0] == ' ') {
		str = str.substr(1, str.length());
	}
	return str;
}

bool Tokenizer::isdigit(char c) {
	return (c >= '0' && c <= '9') ? true : false;
}

bool Tokenizer::isletter(char c) {
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') ? true : false;
}