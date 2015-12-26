#include "View.h"

string intToString(int data) {
	stringstream dataString;
	dataString << data;
	string str;
	dataString >> str;
	return str;
}
Span::Span() {

}
Span::Span(const Span &other) {
	data = other.data;
	begin = other.begin;
	end = other.end;
}
Span::Span(string data_, int begin_, int end_) {
	data = data_;
	begin = begin_;
	end = end_;
}


Column::Column() {
}
Column::~Column() {

}
Column::Column(string name_) {
	width = 0;
	name = name_;

}
Column::Column(string name_, vector<Span> data_) {
	width = 0;
	data = data_;
	name = name_;
	for (int i = 0; i < data_.size(); i++) {
		string content = data_[i].data + ":(" + intToString(data_[i].begin) + "," + intToString(data_[i].end) + ")";
		width = width > content.size() ? width : content.size();
	}
}
Column::Column(const Column &one){
	name = one.name;
	width = one.width;
	data = one.data;
}
string Column::getName() {
	return name;
}
int Column::getWidth() {
	return width;
}
int Column::getRow() {
	return data.size();
}
void Column::setName(string name_) {
	name = name_;
}
void Column::setWidth(int width_) {
	width = width_;
}
void Column::insertSpan(Span newSpan) {
	string content = newSpan.data + ":(" + intToString(newSpan.begin) + "," + intToString(newSpan.end) + ")";
	int newWidth = content.size();
	data.push_back(newSpan);
	width = width > newWidth ? width : newWidth;
}
Span Column::findSpanByIndex(int index) {
	return data[index];
}


View::View() {
	row = 0;
}
View::~View() {
}
View::View(string name_, vector<Column> data_) {
	name = name_;
	data = data_;
	row = data_[0].getRow();
}
string View::getName() {
	return name;
}
int View::getRow() {
	return row;
}
Column View::findColumnByName(string name_) {
	Column result;
	for (int i = 0; i < data.size(); i++) {
		if (data[i].getName() == name_) {
			result = data[i];
			break;
		}
	}
	return result;
}
void View::setName(string name_) {
	name = name_;
}
void View::insertColumn(Column newColumn) {
	data.push_back(newColumn);
}
void View::print(string name) {
	cout << "View: " << name << endl;
	int colNum = data.size();
	int rowNum = getRow();
	vector<int> colWidth;
	for (int i = 0; i < colNum; i++) {
		colWidth.push_back(data[i].getWidth());
	}
	cout << "+";
	for (int i = 0; i < colNum; i++) {
		for (int j = 0; j < colWidth[i] + 2; j++) {
			cout << "-";
		}
		cout << "+";
	}
	cout << endl;

	cout << "|";
	for (int i = 0; i < colNum; i++) {
		int nameLength = data[i].getName().length();
		cout << " " << data[i].getName() << " ";
		for (int j = 0; j < colWidth[i] - nameLength; j++) {
			cout << " ";
		}
		cout << "|";
	}
	cout << endl;

	cout << "+";
	for (int i = 0; i < colNum; i++) {
		for (int j = 0; j < colWidth[i] + 2; j++) {
			cout << "-";
		}
		cout << "+";
	}
	cout << endl;
	
	for (int i = 0; i < rowNum; i++) {
		cout << "|";
		for (int j = 0; j < colNum; j++) {
			Span colSpan = data[j].findSpanByIndex(i);
			string content = colSpan.data + ":(" + intToString(colSpan.begin) + "," + intToString(colSpan.end) + ")";
			cout << " " << content << " ";
			int contentLength = content.length();
			for(int k = 0; k < colWidth[j] - contentLength; k++) {
				cout << " ";
			}
			cout << "|";
		}
		cout << endl;
	}

	cout << "+";
	for (int i = 0; i < colNum; i++) {
		for (int j = 0; j < colWidth[i] + 2; j++) {
			cout << "-";
		}
		cout << "+";
	}
	cout << endl;

	cout << rowNum << " rows in set" << endl << endl;
	
}