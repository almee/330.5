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


vector<vector<pair<int, int>>>& pattern(vector<Atom> &atoms, int index, vector<vector<pair<int, int>>> &lastResult) {
	Column thisColumn;
	Span thisSpan;
	Atom thisOne = atoms[index];
	View jiaZhuangYouGeView;			//啧!!!!!!!!!!!!! viewSet放在parser里似乎不是一个很好的选择
	vector<pair<int, int>> newPath;
	vector<vector<pair<int, int>>> result;
	switch (thisOne.type) {
	case 0:
		thisColumn = jiaZhuangYouGeView.findColumnByName(thisOne.second);
		for (int i = 0; i < lastResult.size(); i++) {
			int end = lastResult[i].at(lastResult[i].size() - 1).second;
			for (int j = 0; j < thisColumn.getRow(); j++) {
				thisSpan = thisColumn.findSpanByIndex(j);
				if (end == thisSpan.begin) {
					newPath = lastResult[i];
					newPath.push_back(pair<int, int>(thisSpan.begin, thisSpan.end));
					result.push_back(newPath);
				}
			}
		}
		break;

	case 1:


	default:
		break;
	}
	return pattern(atoms, index + 1, result);
}