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
	void editPage(vector<int> newElements);
	void sparseEditPage(SparseNode value, int pageIndex);
	void appendPage(vector<int> newElements);
	void reload();
	void sparseReload();
	string getMatrixName(); 
	void renamePage(int newIndex);
};
#endif
