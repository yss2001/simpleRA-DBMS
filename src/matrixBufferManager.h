#ifndef MATRIXBUFFERMANAGER_H
#define MATRIXBUFFERMANAGER_H
#include "matrixPage.h"

class MatrixBufferManager{

	deque<MatrixPage> matrixPages;
	bool inPool(string pageName);
	MatrixPage getFromPool(string pageName);
	MatrixPage insertIntoPool(string matrixName, int pageIndex);

	public:

	MatrixBufferManager();
	MatrixPage getPage(string matrixName, int pageIndex);
	void writePage(string pageName, vector<int> elements);
	void deleteFile(string matrixName, int pageIndex);
	void deleteFile(string fileName);
	void writePage(string matrixName, int pageIndex, vector<int> elements);
	void sparseWritePage(string matrixName, int pageIndex, vector<SparseNode> elements);
	bool appendPage(string matrixName, int pageIndex, vector<int> elements);
	void reloadPages(string matrixName);
	void clearPages();
};

#endif
