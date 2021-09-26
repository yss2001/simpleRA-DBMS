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
		{
			if (this->isSparse)
			{
				if (this->sparseBlockify())
				{
					fin.close();
					return true;
				}
			}
			else
			{
				if (this->blockify())
				{
					fin.close();
					return true;
				}
			}
		}
	}
	fin.close();
	return false;
}

bool Matrix::calculateSize(string line)
{
	logger.log("Matrix::calculateSize");
	string word, sparseLine;
	stringstream s(line);
	int size = 0, sparseCount = 0, errorCount = 0, rowCount = 0;
	while (getline(s, word, ','))
		size += 1;
	this->matrixSize = size;
	fstream fin(this->sourceFileName, ios::in);
	while (getline(fin, sparseLine))
	{
		stringstream s(sparseLine);
		while (getline(s, word, ','))
		{
			errorCount++;
			int x = stoi(word);
			if (x != 0)
				sparseCount++;
		}
		if (errorCount != size)
			return false;
		errorCount = 0;
		rowCount++;
	}
	if (rowCount != size)
		return false;
	if (float(sparseCount) / float(size * size) >= 0.6)
	{
		this->isSparse = true;
		this->sparseElementCount = sparseCount;
		this->sparseMaxValuesPerBlock = (int)(BLOCK_SIZE * 1000) / (sizeof(SparseNode));
	}
	else
		this->maxValuesPerBlock = (int)((BLOCK_SIZE * 1000) / (sizeof(int)));
	return true;
}

bool Matrix::sparseBlockify()
{
	logger.log("Matrix::sparseBlockify");
	ifstream fin(this->sourceFileName, ios::in);
	string line, word;
	int pageIndex = 0, pageCounter = 0, rowCounter = 0, columnCounter = 0;
	vector<SparseNode> elements;
	while (getline(fin, line))
	{
		stringstream s(line);
		while (getline(s, word, ','))
		{
			int x = stoi(word);
			if (x != 0)
			{
				elements.push_back({rowCounter, columnCounter, x});
				pageCounter += 1;
				if (pageCounter == this->sparseMaxValuesPerBlock)
				{
					matrixBufferManager.sparseWritePage(this->matrixName, pageIndex, elements);
					elements.clear();
					pageIndex += 1;
					pageCounter = 0;
				}
			}
			columnCounter++;
		}
		columnCounter = 0;
		rowCounter++;
	}
	if (pageCounter)
	{
		matrixBufferManager.sparseWritePage(this->matrixName, pageIndex, elements);
		pageIndex += 1;
		pageCounter = 0;
	}

	this->pageCount = pageIndex;
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

void Matrix::sparsePrint()
{
	logger.log("Matrix::sparsePrint");
	int maxRows = min(PRINT_COUNT, this->matrixSize), currentRow = 0, currentColumn = 0, elementCount = 0;

	MatrixCursor cursor(this->matrixName, 0);
	SparseNode element;

	while (elementCount < this->sparseElementCount)
	{
		element = cursor.sparseGetNext();
		elementCount++;
		if (element.column >= maxRows)
			break;
		int elementValue = (element.row * this->matrixSize + element.column) - (currentRow * this->matrixSize + currentColumn);
		int currentValue = 0, tempRow = currentRow, tempColumn = currentColumn;
		while (currentValue < elementValue)
		{
			cout << "0 ";
			currentValue++, tempColumn++;
			if (tempColumn == this->matrixSize)
			{
				tempColumn = 0, tempRow++;
				cout << "\n";
			}
		}
		cout << element.value << " ";
		tempColumn++;
		if (tempColumn == this->matrixSize)
		{
			tempColumn = 0, tempRow++;
			cout << "\n";
		}
		currentRow = tempRow, currentColumn = tempColumn;
	}
	for (currentRow; currentRow < maxRows; currentRow++)
	{
		for (currentColumn; currentColumn < this->matrixSize; currentColumn++)
			cout << "0 ";
		cout << "\n";
		currentColumn = 0;
	}
}

void Matrix::print()
{
	logger.log("Matrix::print");
	int maxRows = min(PRINT_COUNT, this->matrixSize);

	MatrixCursor cursor(this->matrixName, 0);
	int element;

	for (int i = 0; i < maxRows; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			element = cursor.getNext();

			if (j == matrixSize - 1)
				cout << element;
			else
				cout << element << " ";
		}
		cout << "\n";
	}
}

void Matrix::transpose()
{
	logger.log("Matrix::transpose");

	MatrixCursor outerCursor(this->matrixName, 0);
	int element = 0;
	int outerOffset = 0;

	for (int x = 0; x < matrixSize; x++)
	{
		for (int y = 0; y < matrixSize; y++)
		{
			element = outerCursor.getNext();
			outerOffset = ((x * matrixSize) + y) % maxValuesPerBlock;

			if (x != y && y > x)
			{
				int transposePage = ((y * matrixSize) + x) / maxValuesPerBlock;
				int transposeOffset = ((y * matrixSize) + x) % maxValuesPerBlock;

				MatrixCursor innerCursor(this->matrixName, transposePage);

				int transposeElement = 0;

				for (int i = 0; i <= transposeOffset; i++)
					transposeElement = innerCursor.getNext();

				outerCursor.putValue(transposeElement, outerOffset);
				innerCursor.putValue(element, transposeOffset);
			}
		}
	}
	matrixBufferManager.reloadPages(this->matrixName);
}

void Matrix::sparseTranspose()
{
}

void Matrix::makePermanent()
{
	logger.log("Matrix::makePermanent");

	if (!this->isPermanent())
		matrixBufferManager.deleteFile(this->sourceFileName);
	string newSourceFile = "../data/" + this->matrixName + ".csv";
	ofstream fout(newSourceFile, ios::out);

	MatrixCursor cursor(this->matrixName, 0);
	int element = 0;
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			element = cursor.getNext();

			if (j == matrixSize - 1)
				fout << element;
			else
				fout << element << ", ";
		}
		fout << "\n";
	}

	fout.close();
}

void Matrix::sparseMakePermanent()
{
	logger.log("Matrix::sparseMakePermanent");
	if (!this->isPermanent())
		matrixBufferManager.deleteFile(this->sourceFileName);

	string newSourceFile = "../data/" + this->matrixName + ".csv";
	ofstream fout(newSourceFile, ios::out);
	int currentRow = 0, currentColumn = 0, elementCount = 0;

	MatrixCursor cursor(this->matrixName, 0);
	SparseNode element;

	while (elementCount < this->sparseElementCount)
	{
		element = cursor.sparseGetNext();
		elementCount++;
		int elementValue = (element.row * this->matrixSize + element.column) - (currentRow * this->matrixSize + currentColumn);
		int currentValue = 0, tempRow = currentRow, tempColumn = currentColumn;
		while (currentValue < elementValue)
		{
			fout << "0 ";
			currentValue++, tempColumn++;
			if (tempColumn == this->matrixSize)
			{
				tempColumn = 0, tempRow++;
				fout << "\n";
			}
		}
		fout << element.value << " ";
		tempColumn++;
		if (tempColumn == this->matrixSize)
		{
			tempColumn = 0, tempRow++;
			fout << "\n";
		}
		currentRow = tempRow, currentColumn = tempColumn;
	}
	for (currentRow; currentRow < this->matrixSize; currentRow++)
	{
		for (currentColumn; currentColumn < this->matrixSize; currentColumn++)
			fout << "0 ";
		fout << "\n";
		currentColumn = 0;
	}
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

	for (int i = 0; i < this->pageCount; i++)
		matrixBufferManager.deleteFile(this->matrixName, i);
	if (!isPermanent())
		matrixBufferManager.deleteFile(this->sourceFileName);
}

void Matrix::getNextPage(MatrixCursor *cursor)
{
	logger.log("Matrix::getNextPage");
	if (cursor->pageIndex < this->pageCount - 1)
		cursor->nextPage(cursor->pageIndex + 1);
	else
		cout << "won't get next page\n";
}

MatrixCursor Matrix::getCursor()
{
	logger.log("Matrix::getCursor");
	MatrixCursor cursor(this->matrixName, 0);
	return cursor;
}
