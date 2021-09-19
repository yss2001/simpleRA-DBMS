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
	void nextPage(int pageIndex);
	void moveCursor(int offset);
};
#endif
