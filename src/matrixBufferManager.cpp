#include "global.h"

MatrixBufferManager::MatrixBufferManager()
{
	logger.log("MatrixBufferManager::MatrixBufferManager");
}

MatrixPage MatrixBufferManager::getPage(string matrixName, int pageIndex)
{
	logger.log("MatrixBufferManager::getPage");
	string pageName = "../data/temp/" + matrixName + "_Page" + to_string(pageIndex);

	if (this->inPool(pageName))
		return this->getFromPool(pageName);
	else
		return this->insertIntoPool(matrixName, pageIndex);
}

bool MatrixBufferManager::inPool(string pageName)
{
	logger.log("MatrixBufferManager::inPool");
	for (auto page : this->matrixPages)
	{
		if (pageName == page.pageName) return true;
	}
	return false;
}

MatrixPage MatrixBufferManager::getFromPool(string pageName)
{
	logger.log("MatrixBufferManager::getFromPool");
	for (auto page : this->matrixPages)
	{
		if (pageName == page.pageName) return page;
	}
}

MatrixPage MatrixBufferManager::insertIntoPool(string matrixName, int pageIndex)
{
	logger.log("MatrixBufferManager::insertIntoPool");
	MatrixPage matrixPage(matrixName, pageIndex);
	if (this->matrixPages.size () >= BLOCK_COUNT)
		matrixPages.pop_front();
	matrixPages.push_back(matrixPage);
	return matrixPage;
}

void MatrixBufferManager::writePage(string matrixName, int pageIndex, vector<int> elements)
{
	logger.log("MatrixBufferManager::writePage");
	MatrixPage matrixPage(matrixName, pageIndex, elements);
	matrixPage.writePage();
}

void MatrixBufferManager::deleteFile(string fileName)
{
	if (remove(fileName.c_str()))
		logger.log("MatrixBufferManager::deleteFile: Err");
	else logger.log("MatrixBufferManager::deleteFile: Success");
}

void MatrixBufferManager::deleteFile(string matrixName, int pageIndex)
{
	logger.log("MatrixBufferManager::deleteFile");
	string fileName = "../data/temp/" + matrixName + "_Page" + to_string(pageIndex);
	this->deleteFile(fileName);
}

void MatrixBufferManager::reloadPages(string matrixName)
{
	logger.log("MatrixBufferManager::reloadPages");

	for (int i=0; i<matrixPages.size(); i++)
	{
		if(matrixPages[i].getMatrixName() == matrixName)
		{
			matrixPages[i].reload();
		}
	}
}
