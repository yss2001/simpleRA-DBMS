#include "global.h"
#define HASH_VALUE 102217
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[7] != "ON" || tokenizedQuery[3] != "USING" || !(tokenizedQuery[4] == "NESTED" || tokenizedQuery[4] == "PARTHASH") || tokenizedQuery[11] != "BUFFER")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[5];
    parsedQuery.joinSecondRelationName = tokenizedQuery[6];
    parsedQuery.joinFirstColumnName = tokenizedQuery[8];
    parsedQuery.joinSecondColumnName = tokenizedQuery[10];
    parsedQuery.joinBuffer = stoi(tokenizedQuery[12]);

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void nestedJOIN()
{
    logger.log("executeNestedJOIN");
    BLOCK_ACCESSES = 0;

    Table firstTable = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));

    int index1 = 0;
    index1 = firstTable.getColumnIndex(parsedQuery.joinFirstColumnName);

    Table secondTable = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));

    int index2 = 0;
    index2 = secondTable.getColumnIndex(parsedQuery.joinSecondColumnName);

    int prevCount = BLOCK_COUNT;
    BLOCK_COUNT = parsedQuery.joinBuffer;

    // Check for self-join
    /*if(firstTable.tableName == secondTable.tableName)
    {
	    parsedQuery.joinFirstRelationName += "1";
	    parsedQuery.joinSecondRelationName += "2";
    }*/

    vector<string> columns;

    for (int columnCounter = 0; columnCounter < firstTable.columnCount; columnCounter++)
    {
        // Check for recurring column names
        /*string columnName = firstTable.columns[columnCounter];
	    if (secondTable.isColumn(columnName))
	    {
		    columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
	    }
	    columns.emplace_back(columnName);*/
        columns.emplace_back(firstTable.columns[columnCounter]);
    }

    for (int columnCounter = 0; columnCounter < secondTable.columnCount; columnCounter++)
    {
        // Check for recurring column names
        /*string columnName = secondTable.columns[columnCounter];
	    if (firstTable.isColumn(columnName))
	    {
		    columnName = parsedQuery.joinSecondRelationName + "_" + columnName;
	    }
	    columns.emplace_back(columnName);*/
        columns.emplace_back(secondTable.columns[columnCounter]);
    }

    Table *joinTable = new Table(parsedQuery.joinResultRelationName, columns);

    int firstPageNumber = 0;

    unordered_map<int, vector<vector<int>>> pageBatch;

    while (firstPageNumber < firstTable.blockCount)
    {
        pageBatch.clear();
        for (int _ = 0; _ < parsedQuery.joinBuffer - 2; _++)
        {
            Page curPage = bufferManager.getPage(firstTable.tableName, firstPageNumber);
            vector<vector<int>> &allRows = curPage.getAllRows();

            for (int i = 0; i < allRows.size(); i++)
            {
                int key = allRows[i][index1];

                if (pageBatch.find(key) != pageBatch.end())
                    pageBatch[key].push_back(allRows[i]);
                else
                {
                    vector<vector<int>> temp;
                    temp.push_back(allRows[i]);
                    pageBatch[key] = temp;
                }
            }
            firstPageNumber += 1;
            if (firstPageNumber == firstTable.blockCount)
                break;
        }
        Cursor cursor = secondTable.getCursor();
        vector<int> row = cursor.getNext();
        vector<int> joinRow;
        joinRow.reserve(joinTable->columnCount);

        while (!row.empty())
        {
            if (pageBatch.find(row[index2]) != pageBatch.end())
            {
                vector<vector<int>> matchedRows = pageBatch[row[index2]];
                for (int i = 0; i < matchedRows.size(); i++)
                {
                    joinRow = matchedRows[i];
                    joinRow.insert(joinRow.end(), row.begin(), row.end());
                    joinTable->writeRow<int>(joinRow);
                }
            }
            row = cursor.getNext();
        }
    }
    joinTable->blockify();
    tableCatalogue.insertTable(joinTable);

    BLOCK_COUNT = prevCount;
    bufferManager.resetBufferManager();
    cout << "Total block accesses: " << BLOCK_ACCESSES << "\n";
    BLOCK_ACCESSES = 0;

    return;
}

void parthashJOIN()
{
    logger.log("executeParthashJOIN");
    BLOCK_ACCESSES = 0;
    int prevCount = BLOCK_COUNT;
    BLOCK_COUNT = parsedQuery.joinBuffer;

    Table firstTable = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    Cursor firstCursor = firstTable.getCursor();
    int firstIndex = 0;
    firstIndex = firstTable.getColumnIndex(parsedQuery.joinFirstColumnName);
    vector<int> partitionCheck(parsedQuery.joinBuffer - 1, 0);

    vector<int> firstRow = firstCursor.getNext();
    while (!firstRow.empty())
    {
        int hashValue = (long long int)(firstRow[firstIndex] * HASH_VALUE) % (parsedQuery.joinBuffer - 1);
        string partitionName = parsedQuery.joinFirstRelationName + "Partition";
        if (partitionCheck[hashValue] == 0)
        {
            vector<vector<int>> writeRows;
            writeRows.push_back(firstRow);
            bufferManager.writePage(partitionName, hashValue, writeRows, 1);
            partitionCheck[hashValue]++;
        }
        else
        {
            bufferManager.getPage(partitionName, hashValue);
            // TOWRITE - PAGE UPDATE
        }
    }

    Table secondTable = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));
    Cursor secondCursor = secondTable.getCursor();
    int secondIndex = 0;
    secondIndex = secondTable.getColumnIndex(parsedQuery.joinSecondColumnName);
    for (int i = 0; i < parsedQuery.joinBuffer - 1; i++)
        partitionCheck[i] = 0;

    vector<int> secondRow = secondCursor.getNext();
    while (!secondRow.empty())
    {
        int hashValue = (long long int)(secondRow[secondIndex] * HASH_VALUE) % (parsedQuery.joinBuffer - 1);
        string partitionName = parsedQuery.joinSecondRelationName + "Partition";
        if (partitionCheck[hashValue] == 0)
        {
            vector<vector<int>> writeRows;
            writeRows.push_back(secondRow);
            bufferManager.writePage(partitionName, hashValue, writeRows, 1);
        }
        else
        {
            bufferManager.getPage(partitionName, hashValue);
            // TOWRITE - PAGE UPDATE
        }
        partitionCheck[hashValue]++;
    }

    vector<string> columns;
    for (int columnCounter = 0; columnCounter < firstTable.columnCount; columnCounter++)
        columns.emplace_back(firstTable.columns[columnCounter]);
    for (int columnCounter = 0; columnCounter < secondTable.columnCount; columnCounter++)
        columns.emplace_back(secondTable.columns[columnCounter]);
    Table *joinTable = new Table(parsedQuery.joinResultRelationName, columns);

    for (int partId = 0; partId < parsedQuery.joinBuffer - 1; partId++)
    {
        Page firstPage = bufferManager.getPage(string(parsedQuery.joinFirstRelationName + "Partition"), partId);
        Page secondPage = bufferManager.getPage(string(parsedQuery.joinSecondRelationName + "Partition"), partId);
        vector<vector<int>> &firstRows = firstPage.getAllRows();
        unordered_map<int, vector<vector<int>>> firstPageMap;

        for (int i = 0; i < firstRows.size(); i++)
        {
            int key = firstRows[i][firstIndex];

            if (firstPageMap.find(key) != firstPageMap.end())
                firstPageMap[key].push_back(firstRows[i]);
            else
            {
                vector<vector<int>> temp;
                temp.push_back(firstRows[i]);
                firstPageMap[key] = temp;
            }
        }
        Cursor secondRows = secondTable.getCursor();
        vector<int> row = secondRows.getNext();
        vector<int> joinRow;
        joinRow.reserve(joinTable->columnCount);
        for (int _ = 0; _ < partitionCheck[partId]; _++)
        {
            vector<vector<int>> matchedRows = firstPageMap[row[secondIndex]];
            for (int i = 0; i < matchedRows.size(); i++)
            {
                joinRow = matchedRows[i];
                joinRow.insert(joinRow.end(), row.begin(), row.end());
                joinTable->writeRow<int>(joinRow);
            }
            row = secondRows.getNext();
        }
    }
    joinTable->blockify();
    tableCatalogue.insertTable(joinTable);
    for (int i = 0; i < parsedQuery.joinBuffer - 1; i++)
    {
        bufferManager.deleteFile(string(parsedQuery.joinFirstRelationName + "Partition"),i);
        bufferManager.deleteFile(string(parsedQuery.joinSecondRelationName + "Partition"),i);
    }

    BLOCK_COUNT = prevCount;
    bufferManager.resetBufferManager();
    cout << "Total block accesses: " << BLOCK_ACCESSES << "\n";
    BLOCK_ACCESSES = 0;

    return;
}

void executeJOIN()
{
    logger.log("executeJOIN");

    if (tokenizedQuery[4] == "NESTED")
        nestedJOIN();
    else if (tokenizedQuery[4] == "PARTHASH")
        parthashJOIN();
    return;
}
