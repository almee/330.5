#include "Support.h"
#include "regex.h"
extract_data::extract_data() {
	reg = "";
}
extract_data::extract_data(string reg_, map<int, string> group_) {
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




Support::Support() {
	atomIndex = -1;
}
Support::Support(string document_, vector<View> viewSet_, vector<Terms> tokenSet_) {
	atomIndex = -1;
	document = document_;
	viewSet = viewSet_;
	tokenSet = tokenSet_;
}
void Support::addAtomIndex() {
	atomIndex++;
}
int Support::getAtomIndex() {
	return atomIndex;
}
vector<Span> Support::getSpansByReg(string &reg) {
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
int Support::getTokenIndexByEnd(int end) {
	for (int i = 0; i < tokenSet.size(); i++) {
		if (end == tokenSet[i].end)
			return i;
	}
	return 0;

}
vector<pair<int, int> > Support::getAllPossibleTokenMatch(int nowIndex, int min, int max) {
	int left = tokenSet[nowIndex].end;
	vector<pair<int, int>> result;
	for (int offset = min; offset <= max; offset++) {
		result.push_back(pair<int, int>(left, tokenSet[nowIndex + offset].end));
	}
	return result;
}
int Support::getViewNum() {
	return viewSet.size();
}
void Support::insertView(View newView) {
	viewSet.push_back(newView);
}
View Support::getViewByNum(int index) {
	return viewSet[index];
}
View Support::findViewByName(string name_) {
	for (int i = 0; i < viewSet.size(); i++) {
		if (viewSet[i].getName() == name_)
			return viewSet[i];
	}
}
string Support::getContent(int left, int right) {
	return document.substr(left, right);
}
int Support::getBeginOfToken(int index) {
	return tokenSet[index].begin;
}
int Support::getEndOfToken(int index) {
	return tokenSet[index].end;
}
vector<vector<pair<int, int>>> Support::pattern(vector<Atom> &atoms, int index, vector<vector<pair<int, int>>> &lastResult) {
	Atom thisOne = atoms[index];

	int lastEnd;
	Span thisSpan;
	Column thisColumn;
	vector<Span> thisSpans;
	vector<pair<int, int> > allPossibleTokenMatch;
	vector<pair<int, int>> newPath;
	vector<vector<pair<int, int>>> result;

	switch (thisOne.type) {
	case 0:
		thisColumn = findViewByName(thisOne.first).findColumnByName(thisOne.second);
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
			allPossibleTokenMatch = getAllPossibleTokenMatch(getTokenIndexByEnd(lastEnd), thisOne.min, thisOne.max);
			for (int j = 0; j < allPossibleTokenMatch.size(); j++) {
				newPath = lastResult[i];
				newPath.push_back(allPossibleTokenMatch[j]);
				result.push_back(newPath);
			}
		}
		break;

	case 2:
		thisSpans = getSpansByReg(thisOne.first);
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
	return result;
}

