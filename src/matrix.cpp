#include "global.h"
#include "matrixPage.h"

Matrix::Matrix()
{
	logger.log("Matrix::Matrix");
}

Matrix::Matrix(string matrixName)
{
	logger.log("Matrix::Matrix");
	this->sourceFileName = "../data/" + matrixName + ".csv";
	this->matrixName = matrixName;
}

bool Matrix::load()
{
	logger.log("Matrix::load");
	fstream fin(this->sourceFileName, ios::in);
	string line;

	if (getline(fin, line))
	{
		fin.close();
		if (this->calculateSize(line))
			if (this->blockify())
			{
				fin.close();
				return true;
			}
	}
	fin.close();
	return false;
}

bool Matrix::calculateSize(string line)
{
	logger.log("Matrix::calculateSize");
	string word;
	stringstream s(line);
	int size = 0;
	while (getline(s, word, ','))
	{
		size += 1;
	}
	this->matrixSize = size;
	this->maxValuesPerBlock = int(sqrt((int)((BLOCK_SIZE * 1000) / (sizeof(int)))));
	this->maxValuesPerBlock = min(int(this->maxValuesPerBlock), size);
	return true;
}

bool Matrix::blockify()
{
	logger.log("Matrix::blockify");
	ifstream fin(this->sourceFileName, ios::in);
	string line, word;
	vector<int> elements;
	int x = 0, y = 0;
	int grayArea = this->matrixSize - (this->matrixSize%this->maxValuesPerBlock);

	while(getline(fin, line))
	{
		int pageNumber = 0;
		if(this->matrixSize % this->maxValuesPerBlock == 0) pageNumber += this->matrixSize/this->maxValuesPerBlock * (x/this->maxValuesPerBlock);
		else
		{
			pageNumber += ((this->matrixSize/this->maxValuesPerBlock + 1) * (x/this->maxValuesPerBlock));
		}
		stringstream s(line);

		while(getline(s, word, ','))
		{
			elements.push_back(stoi(word));
			y+=1;
			if(y%(this->maxValuesPerBlock)==0 || y == this->matrixSize)
			{
				if(matrixBufferManager.appendPage(this->matrixName, pageNumber, elements))
					this->pageCount += 1;
				elements.clear();
				pageNumber += 1;
			}
		}
		x += 1;
		y = 0;
	}

	fin.close();
	return true;
}


void Matrix::print()
{
	logger.log("Matrix::print");
	int maxRows = min(PRINT_COUNT, this->matrixSize);

	matrixBufferManager.clearPages();

	int element;
	int lastPage = 0;
	int pageNumber = 0;
	int x = 0, y = 0;
	MatrixCursor cursor(this->matrixName, pageNumber);
	int grayArea = this->matrixSize - (this->matrixSize%this->maxValuesPerBlock);
	while (x<maxRows)
	{
		if(this->matrixSize % this->maxValuesPerBlock == 0) pageNumber = this->matrixSize/this->maxValuesPerBlock * (x/this->maxValuesPerBlock);
		else
		{
			pageNumber = ((this->matrixSize/this->maxValuesPerBlock + 1) * (x/this->maxValuesPerBlock));
		}
		cursor.nextPage(pageNumber);
		y = 0;
		cursor.moveCursor((x%this->maxValuesPerBlock)*this->maxValuesPerBlock);


		while (y<matrixSize)
		{

			element = cursor.getNext();
			cout<<element<<", ";

			y+=1;
			if(y%(this->maxValuesPerBlock)==0 || y == this->matrixSize)
			{
				pageNumber += 1;
				cursor.nextPage(pageNumber);
				if (y >= grayArea && this->matrixSize != this->maxValuesPerBlock)
				{
					cursor.moveCursor((x%this->maxValuesPerBlock)*(this->matrixSize%this->maxValuesPerBlock));
				}
				else
				{
					cursor.moveCursor((x%this->maxValuesPerBlock)*this->maxValuesPerBlock);
				}

			}
		}
		x += 1;
		cout<<"\n";
	}
}

void Matrix::transpose()
{
	logger.log("Matrix::transpose");
	int superSize = (this->matrixSize % this->maxValuesPerBlock!=0) ? this->matrixSize/this->maxValuesPerBlock + 1 : this->matrixSize/this->maxValuesPerBlock;

	matrixBufferManager.clearPages();

	for(int i=0; i<this->pageCount; i++)
	{
		MatrixPage page = matrixBufferManager.getPage(this->matrixName, i);
		int columnStride = 0;

		if (this->matrixSize == this->maxValuesPerBlock)
			columnStride = this->matrixSize;
		else if (i%superSize != superSize-1 && i/superSize != superSize-1 )
		{
			columnStride = this->maxValuesPerBlock;	
		}
		else if (i%superSize == superSize-1 && i/superSize != superSize-1)
		{
			columnStride = this->matrixSize - this->maxValuesPerBlock;
		}
		else if (i%superSize != superSize-1 && i/superSize == superSize-1)
		{
			columnStride = this->maxValuesPerBlock;
		}
		else
		{
			columnStride = this->matrixSize - this->maxValuesPerBlock;
		}
		int x = 0, y = 0;

		vector<int> transposedElements;
		int size = page.getElementCount();
		for(int i=0; i<columnStride; i++)
		{
			for(int j=i; j<size; j+=columnStride)
			{
				transposedElements.push_back(page.getElements(j));
			}
		}
		page.editPage(transposedElements);
		page.writePage();
	}

	int x = 0, y = 0;

	for (int i=0; i<this->pageCount; i++)
	{
		x = i/superSize, y = i%superSize;

		if(x!=y && y>x)
		{
			MatrixPage page = matrixBufferManager.getPage(this->matrixName, i);
			page.renamePage(y*superSize+x);

			MatrixPage transposePage = matrixBufferManager.getPage(this->matrixName, y*superSize+x);
			transposePage.renamePage(i);

			page.writePage();
			transposePage.writePage();
		}
	}

}

void Matrix::makePermanent()
{
	logger.log("Matrix::makePermanent");
	matrixBufferManager.clearPages();

	if(!this->isPermanent())
		matrixBufferManager.deleteFile(this->sourceFileName);
	string newSourceFile = "../data/" + this->matrixName + ".csv";
	ofstream fout(newSourceFile, ios::out);

	int element = 0;
	int pageNumber = 0;
	int x = 0, y = 0;
	MatrixCursor cursor(this->matrixName, pageNumber);
	int grayArea = this->matrixSize - (this->matrixSize%this->maxValuesPerBlock);

	while (x<matrixSize)
	{
		if(this->matrixSize % this->maxValuesPerBlock == 0)
			pageNumber = this->matrixSize/this->maxValuesPerBlock * (x/this->maxValuesPerBlock);
		else
			pageNumber =  ((this->matrixSize/this->maxValuesPerBlock + 1) * (x/this->maxValuesPerBlock));

		cursor.nextPage(pageNumber);
		y = 0;
		cursor.moveCursor((x%this->maxValuesPerBlock)*this->maxValuesPerBlock);

		while (y<matrixSize)
		{
			element = cursor.getNext();
			
			if (y == this->matrixSize-1)
				fout << element << "\n";
			else 
				fout << element << ", ";

			y += 1;

			if (y%this->maxValuesPerBlock == 0 || y == this->matrixSize)
			{
				pageNumber += 1;
				cursor.nextPage(pageNumber);

				if (y >= grayArea)
				{
					cursor.moveCursor((x%this->maxValuesPerBlock)*(this->matrixSize % this->maxValuesPerBlock));
				}
				else
				{
					cursor.moveCursor((x%this->maxValuesPerBlock)*this->maxValuesPerBlock);
				}
			}
		}
		x += 1;
	}

	fout.close();
}

bool Matrix::isPermanent()
{
	logger.log("Matrix::isPermanent");
	if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
		return true;
	return false;
}

void Matrix::unload()
{
	logger.log("Matrix::unload");

	for (int i = 0; i<this->pageCount; i++)
		matrixBufferManager.deleteFile(this->matrixName, i);
	if(!isPermanent())
		matrixBufferManager.deleteFile(this->sourceFileName);
}

void Matrix::getNextPage(MatrixCursor *cursor)
{
	logger.log("Matrix::getNextPage");

	if (cursor->pageIndex < this->pageCount -1)
		cursor->nextPage(cursor->pageIndex+1);
	else cout<<"won't get next page\n";
}

MatrixCursor Matrix::getCursor()
{
	logger.log("Matrix::getCursor");
	MatrixCursor cursor(this->matrixName, 0);
	return cursor;
}


