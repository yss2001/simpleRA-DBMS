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

	bool blockify();
	Matrix();
	Matrix(string matrixName);
	bool load();
	bool calculateSize(string line);
	void print();
	void makePermanent();
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
