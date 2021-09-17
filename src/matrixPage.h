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
	int getElementCount();
	void editPage(int value, int pageIndex);
	void reload();
	string getMatrixName(); 
};
#endif
