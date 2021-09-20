#include "global.h"

//check sparse or not here
bool syntacticParsePRINTMATRIX()
{
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINTMATRIX;
    parsedQuery.printMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParsePRINTMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printMatrixName);
    if(matrix->isSparse)
        matrix->sparsePrint();
    else
        matrix->print();
    return;
}
