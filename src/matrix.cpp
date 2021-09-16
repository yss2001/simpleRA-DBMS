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
	this->maxValuesPerBlock = (int)((BLOCK_SIZE * 1000) / (sizeof(int)));
	return true;
}

bool Matrix::blockify()
{
	logger.log("Matrix::blockify");
	ifstream fin(this->sourceFileName, ios::in);
	string line, word;
	int pageIndex = 0;
	int pageCounter = 0;
	vector<int> elements;
	while (getline(fin, line))
	{
		stringstream s(line);
		while (getline(s, word, ','))
		{
			int x = stoi(word);
			elements.push_back(x);

			pageCounter += 1;

			if (pageCounter == this->maxValuesPerBlock)
			{
				matrixBufferManager.writePage(this->matrixName, pageIndex, elements);
				elements.clear();
				pageIndex += 1;
				pageCounter = 0;
			}
		}
	}

	if (pageCounter)
	{
		matrixBufferManager.writePage(this->matrixName, pageIndex, elements);
		pageIndex += 1;
		pageCounter = 0;
	}

	this->pageCount = pageIndex + 1;
	return true;
}


void Matrix::print()
{
	logger.log("Matrix::print");
	int maxRows = min(PRINT_COUNT, this->matrixSize);

	MatrixCursor cursor(this->matrixName, 0);
	int element;

	for (int i=0; i<maxRows; i++)
	{
		for (int j = 0; j<matrixSize; j++)
		{
			element = cursor.getNext();
			
			if(j==matrixSize-1) cout<<element;
			else cout<<element<<" ";
		}
		cout<<"\n";
	}
}

/*void Matrix::transpose()
{
	logger.log("Matrix::transpose");
	
	MatrixCursor outerCursor(this->matrixName, 0);
	int element = 0;
	int outerOffset = 0;

	for (int x=0; x<matrixSize; x++)
	{
		for (int y=0; y<matrixSize; y++)
		{
			element = outerCursor.getNext();
			cout<<"at x = "<< x << " and y = "<<y <<" value is = "<<element<<"\n";
			outerOffset = ((x*matrixSize) + y) % maxValuesPerBlock;
			cout<<"outeroff = "<<outerOffset<<"\n";

			if (x!=y && y>=x)
			{
				int pageNum = ((y*matrixSize) + x) / maxValuesPerBlock;
				int pageOff = ((y*matrixSize) + x) % maxValuesPerBlock;
				cout<<"transpose n and o: "<<pageNum<<", "<<pageOff<<"\n";

				MatrixCursor innerCursor(this->matrixName, pageNum);

				for (int i=0; i<pageOff; i++) innerCursor.getNext();

				int transposeElement = innerCursor.getNext();
				cout<<"transpose element = "<<transposeElement<<"\n";
				outerCursor.putValue(outerOffset, transposeElement);
				innerCursor.putValue(pageOff, element);
			}
		}
	}
}*/

void Matrix::makePermanent()
{
	logger.log("Matrix::makePermanent");

	if(!this->isPermanent())
		matrixBufferManager.deleteFile(this->sourceFileName);
	string newSourceFile = "../data/" + this->matrixName + ".csv";
	ofstream fout(newSourceFile, ios::out);


	MatrixCursor cursor(this->matrixName, 0);
	int element = 0;
	for (int i=0; i<matrixSize; i++)
	{
		for (int j = 0; j<matrixSize; j++)
		{
			element = cursor.getNext();
			
			if(j==matrixSize-1) fout << element;
			else fout << element <<", ";
		}
		fout << "\n";
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


