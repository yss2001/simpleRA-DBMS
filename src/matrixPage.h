#ifndef MATRIXPAGE_H
#define MATRIXPAGE_H
#include "logger.h"

class MatrixPage{

	string matrixName;
	string pageIndex;
	vector<int> elements;

	public:

	string pageName = "";
	MatrixPage();
	MatrixPage(string matrixName, int pageIndex);
	MatrixPage(string matrixName, int pageIndex, vector<int> elements);
	int getElements(int index);
	void writePage();
	void appendPage(vector<int> newElements);
	int getElementCount();
	void editPage(vector<int> newElements);
	void reload();
	string getMatrixName(); 
	void renamePage(int newIndex);
};
#endif
