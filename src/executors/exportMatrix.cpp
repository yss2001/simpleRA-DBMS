#include "global.h"


bool syntacticParseEXPORTMATRIX()
{
    logger.log("syntacticParseEXPORTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORTMATRIX;
    parsedQuery.exportMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORTMATRIX()
{
    logger.log("semanticParseEXPORTMATRIX");
    //Table should exist
    if (matrixCatalogue.isMatrix(parsedQuery.exportMatrixName))
        return true;
    cout << "SEMANTIC ERROR: No such matrix exists" << endl;
    return false;
}

void executeEXPORTMATRIX()
{
    logger.log("executeEXPORTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportMatrixName);
    matrix->makePermanent();
    return;
}
