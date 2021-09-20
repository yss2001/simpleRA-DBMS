#include "global.h"

MatrixPage::MatrixPage()
{
	this->pageName = "";
	this->matrixName = "";
	this->pageIndex = -1;
	this->elements.clear();
	this->sparseElements.clear();
}

MatrixPage::MatrixPage(string matrixName, int pageIndex)
{
	logger.log("MatrixPage::MatrixPage");
	this->matrixName = matrixName;
	this->pageIndex = pageIndex;
	this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);

	ifstream fin(pageName, ios::in | ios::binary);
	int element;

	while (fin.read(reinterpret_cast<char *>(&element), sizeof(element)))
	{
		this->elements.push_back(element);
	}

	fin.close();
}

int MatrixPage::getElements(int index)
{
	logger.log("MatrixPage::getElements");
	return this->elements[index];
}

SparseNode MatrixPage::sparseGetElements(int index)
{
	logger.log("MatrixPage::sparseGetElements");
	return this->sparseElements[index];
}

int MatrixPage::getElementCount()
{
	logger.log("MatrixPage::getElementCount");
	return this->elements.size();
}

int MatrixPage::sparseGetElementCount()
{
	logger.log("MatrixPage::sparseGetElementCount");
	return this->sparseElements.size();
}

MatrixPage::MatrixPage(string matrixName, int pageIndex, vector<int> elements)
{
	logger.log("MatrixPage::MatrixPage");
	this->matrixName = matrixName;
	this->pageIndex = pageIndex;
	this->elements = elements;
	this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
}

MatrixPage::MatrixPage(string matrixName, int pageIndex, vector<SparseNode> sparseElements)
{
	logger.log("MatrixPage::MatrixPage");
	this->matrixName = matrixName;
	this->pageIndex = pageIndex;
	this->sparseElements = sparseElements;
	this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
}

void MatrixPage::writePage()
{
	logger.log("MatrixPage::writePage");
	ofstream fout(this->pageName, ios::trunc | ios::out | ios::binary);
	for (int element = 0; element < this->elements.size(); element ++)
	{
		fout.write(reinterpret_cast<const char*>(&elements[element]), sizeof(elements[element]));
	}

	fout.close();
}

void MatrixPage::sparseWritePage()
{
	logger.log("MatrixPage::sparseWritePage");
	ofstream fout(this->pageName, ios::trunc | ios::out | ios::binary);
	for (int element = 0; element < this->sparseElements.size(); element ++)
	{
		fout.write(reinterpret_cast<const char*>(&sparseElements[element]), sizeof(sparseElements[element]));
	}

	fout.close();
}



