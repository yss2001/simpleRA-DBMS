#ifndef MATRIXCATALOGUE_H
#define MATRIXCATALOGUE_H
#include "matrix.h"

class MatrixCatalogue{
	
	unordered_map<string, Matrix*> matrices;

	public:

	MatrixCatalogue() {}
	void insertMatrix(Matrix* matrix);
	void deleteMatrix(string matrixName);
	Matrix* getMatrix(string matrixName);
	bool isMatrix(string matrixName);
	~MatrixCatalogue();
};

#endif
