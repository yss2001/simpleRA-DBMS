#include "global.h"

MatrixPage::MatrixPage()
{
	this->pageName = "";
	this->matrixName = "";
	this->pageIndex = -1;
	this->elements.clear();
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

int MatrixPage::getElementCount()
{
	logger.log("MatrixPage::getElementCount");
	return this->elements.size();
}

MatrixPage::MatrixPage(string matrixName, int pageIndex, vector<int> elements)
{
	logger.log("MatrixPage::MatrixPage");
	this->matrixName = matrixName;
	this->pageIndex = pageIndex;
	this->elements = elements;
	this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
}


void MatrixPage::editPage(vector<int> newElements)
{
	logger.log("MatrixPage::editPage");
	this->elements = newElements;
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

void MatrixPage::appendPage(vector<int> newElements)
{
	logger.log("MatrixPage::appendPage");
	fstream fout(this->pageName, ios::app | ios::out | ios::binary);
	fout.seekg(0, ios_base::end);
	for (int element = 0; element < newElements.size(); element++)
	{
		fout.write(reinterpret_cast<const char*>(&newElements[element]), sizeof(newElements[element]));
		this->elements.push_back(newElements[element]);
	}
	fout.close();
}

void MatrixPage::reload()
{
	logger.log("MatrixPage::reload");
	int i=0, element;

	ifstream fin(this->pageName, ios::in | ios::binary);
	while (fin.read(reinterpret_cast<char *>(&element), sizeof(element)))
	{
		this->elements[i] = element;
		i++;
	}
	fin.close();
}

string MatrixPage::getMatrixName()
{
	return this->matrixName;
}

void MatrixPage::renamePage(int newIndex)
{
	this->pageIndex = newIndex;
	this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(newIndex);
}
