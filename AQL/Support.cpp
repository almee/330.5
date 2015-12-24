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

int getTokenIndexByEnd(vector<TokenOfTokenizer> tokens, int end) {
	for (int i = 0; i < tokens.size(); i++) {
		if (end == tokens[i].end)
			return i;
	}
	return 0;

}

vector<pair<int, int> > getAllPossibleTokenMatch(vector<TokenOfTokenizer> tokens, int nowIndex, int min, int max) {
	int left = tokens[nowIndex].end;
	vector<pair<int, int>> result;
	for (int offset = min; offset <= max; offset++) {
		result.push_back(pair<int, int>(left, tokens[nowIndex + offset].end));
	}
	return result;
}

vector<vector<pair<int, int>>>& pattern(vector<Atom> &atoms, int index, vector<vector<pair<int, int>>> &lastResult) {
	Atom thisOne = atoms[index];

	int lastEnd;
	Span thisSpan;
	Column thisColumn;
	vector<Span> thisSpans;
	vector<pair<int, int> > allPossibleTokenMatch;
	vector<pair<int, int>> newPath;

	vector<vector<pair<int, int>>> result;

	//不应该出现在这里的变量
	View jiaZhuangYouGeView;							//啧! viewSet放在parser里似乎不是一个很好的选择
	string jiaZhuangYouGeWenDang;                       //原来都是这种问题
	vector<TokenOfTokenizer> jiaZhuangYouGeTokenVector;	//同样需要考虑这个东西的存放位置

	switch (thisOne.type) {
	case 0:
		thisColumn = jiaZhuangYouGeView.findColumnByName(thisOne.second);
		for (int i = 0; i < lastResult.size(); i++) {
			lastEnd = lastResult[i].at(lastResult[i].size() - 1).second;
			for (int j = 0; j < thisColumn.getRow(); j++) {
				thisSpan = thisColumn.findSpanByIndex(j);
				if (lastEnd == thisSpan.begin) {
					newPath = lastResult[i];
					newPath.push_back(pair<int, int>(thisSpan.begin, thisSpan.end));
					result.push_back(newPath);
				}
			}
		}
		break;

	case 1:
		for (int i = 0; i < lastResult.size(); i++) {
			lastEnd = lastResult[i].at(lastResult[i].size() - 1).second;
			allPossibleTokenMatch = getAllPossibleTokenMatch(jiaZhuangYouGeTokenVector, getTokenIndexByEnd(jiaZhuangYouGeTokenVector, lastEnd), thisOne.min, thisOne.max);
			for (int j = 0; j < allPossibleTokenMatch.size(); j++) {
				newPath = lastResult[i];
				newPath.push_back(allPossibleTokenMatch[j]);
				result.push_back(newPath);
			}
		}
		break;

	case 2:
		thisSpans = getSpansByReg(thisOne.first, jiaZhuangYouGeWenDang);
		for (int i = 0; i < lastResult.size(); i++) {
			int end = lastResult[i].at(lastResult[i].size() - 1).second;
			for (int j = 0; j < thisSpans.size(); j++) {
				thisSpan = thisSpans[j];
				if (end == thisSpan.begin) {
					newPath = lastResult[i];
					newPath.push_back(pair<int, int>(thisSpan.begin, thisSpan.end));
					result.push_back(newPath);
				}
			}
		}
		break;

	default:
		break;
	}
	return pattern(atoms, index + 1, result);
}