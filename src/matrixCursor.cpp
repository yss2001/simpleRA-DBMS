#include "global.h"

MatrixCursor::MatrixCursor(string matrixName, int pageIndex)
{
	logger.log("MatrixCursor::MatrixCursor");
	this->page = matrixBufferManager.getPage(matrixName, pageIndex);
	this->pagePointer = 0;
	this->matrixName = matrixName;
	this->pageIndex = pageIndex;
}

int MatrixCursor::getNext()
{
	if(this->pagePointer == this->page.getElementCount())
	{
		matrixCatalogue.getMatrix(this->matrixName)->getNextPage(this);
	}

	logger.log("MatrixCursor::getNext");
	int result = this->page.getElements(this->pagePointer);
	this->pagePointer++;

	return result;
}


void MatrixCursor::nextPage(int pageIndex)
{
	logger.log("MatrixCursor::nextPage");
	this->page = matrixBufferManager.getPage(this->matrixName, pageIndex);
	this->pageIndex = pageIndex;
	this->pagePointer = 0;
}

void MatrixCursor::moveCursor(int offset)
{
	logger.log("MatrixCursor::moveCursor");
	this->pagePointer += offset;
}
