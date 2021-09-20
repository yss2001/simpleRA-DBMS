#include "global.h"

// sparse matrix add all equivalent functions (sparseGetNext)
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
	if (this->pagePointer == this->page.getElementCount())
	{
		matrixCatalogue.getMatrix(this->matrixName)->getNextPage(this);
	}

	logger.log("MatrixCursor::getNext");
	int result = this->page.getElements(this->pagePointer);
	this->pagePointer++;

	return result;
}

SparseNode MatrixCursor::sparseGetNext()
{
	if (this->pagePointer == this->page.getElementCount())
	{
		matrixCatalogue.getMatrix(this->matrixName)->getNextPage(this);
	}

	logger.log("MatrixCursor::sparseGetNext");
	SparseNode result = this->page.sparseGetElements(this->pagePointer);
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
