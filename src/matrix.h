#ifndef MATRIX_H
#define MATRIX_H
#include "matrixCursor.h"

class Matrix{

	public:

	string sourceFileName = "";
	string matrixName = "";
	uint matrixSize = 0;
	uint pageCount = 0;
	uint maxValuesPerBlock = 0;
	uint sparseMaxValuesPerBlock = 0;

	bool sparseBlockify();
	bool blockify();
	Matrix();
	Matrix(string matrixName);
	bool load();
	bool calculateSize(string line);
	void print();
	void sparsePrint();
	void makePermanent();
	void sparseMakePermanent();
	bool isPermanent();
	void getNextPage(MatrixCursor *cursor);
	MatrixCursor getCursor();
	void unload();
	void transpose();
	void sparseTranspose();


	bool isSparse = false;
	uint sparseElementCount = 0;
};

#endif
