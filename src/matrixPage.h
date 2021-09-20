#ifndef MATRIXPAGE_H
#define MATRIXPAGE_H
#include "logger.h"

struct SparseNode{
	int row,column, value;
};

// add sparse matrix struct, binary file functions
class MatrixPage{

	string matrixName;
	string pageIndex;
	vector<int> elements;
	vector<SparseNode> sparseElements;

	public:

	string pageName = "";
	MatrixPage();
	MatrixPage(string matrixName, int pageIndex);
	MatrixPage(string matrixName, int pageIndex, vector<int> elements);
	MatrixPage(string matrixName, int pageIndex, vector<SparseNode> sparseElements);
	int getElements(int index);
	SparseNode sparseGetElements(int index);
	void writePage();
	void sparseWritePage();
	int getElementCount();
	int sparseGetElementCount();
	void editPage(int value, int pageIndex);
	void reload();
	string getMatrixName(); 
};
#endif
