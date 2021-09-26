#ifndef MATRIXCURSOR_H
#define MATRIXCURSOR_H
#include "matrixBufferManager.h"

class MatrixCursor{
	
	public:

	MatrixPage page;
	int pageIndex;
	string matrixName;
	int pagePointer;

	MatrixCursor(string matrixName, int pageIndex);
	int getNext();
	SparseNode sparseGetNext();
	void nextPage(int pageIndex);
	void sparsePutValue(SparseNode value, int index);
	void gotoPage(int pageId);
	void moveCursor(int offset);
};
#endif
