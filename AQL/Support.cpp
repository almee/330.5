#include "Support.h"
#include "regex.h"
extract_data::extract_data() {
	reg = "";
}
extract_data::extract_data(string reg_, vector<pair<int, string> > group_) {
	reg = reg_;
	group = group_;
}
extract_data::extract_data(vector<Atom> atoms_) {
	atoms = atoms_;
}
void extract_data::operator+=(const extract_data &other) {
	atoms.insert(atoms.end(), other.atoms.begin(), other.atoms.end());
	catchList.insert(catchList.end(), other.catchList.begin(), other.catchList.end());
}
//没有处理正则内的捕获
vector<Span> getSpansByReg(string &reg, string &document) {
	string content;
	int left, right;
	vector<Span> result;
	vector<vector<int> > tmp = findall(reg.c_str(), document.c_str());
	for (int i = 0; i < tmp.size(); i++) {
		left = tmp[i][0];
		right = tmp[i][1];
		content = document.substr(left, right - left);
		result.push_back(Span(content, left, right));
	}
	return result;
}
