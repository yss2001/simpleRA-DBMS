#include"global.h"
#define DEFAULT_BLOCK_SIZE 10
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    if((tokenizedQuery.size() != 10 && tokenizedQuery.size() != 8) || (tokenizedQuery.size() == 10 && tokenizedQuery[8] != "BUFFER") || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    string sortingStrategy = tokenizedQuery[7];
    if(sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    if(tokenizedQuery.size() == 10)
        parsedQuery.sortBuffer = stoi(tokenizedQuery[9]);
    else
        parsedQuery.sortBuffer = DEFAULT_BLOCK_SIZE;
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }
    if(parsedQuery.sortBuffer < 1){
        cout<<"SEMANTIC ERROR: Invalid buffer size";
        return false;
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");

    int prevCount = BLOCK_COUNT;
    BLOCK_COUNT = parsedQuery.sortBuffer;
    Table *sortTable = tableCatalogue.getTable(parsedQuery.sortRelationName);
    int sortIndex = sortTable->getColumnIndex(parsedQuery.sortColumnName);
    
    int subFileCount = ceil(float(sortTable->blockCount)/float(parsedQuery.sortBuffer));
    int mergeFileCount = ceil(log(subFileCount)/log(min(subFileCount,parsedQuery.sortBuffer-1)));

    //cout<<subFileCount<<" "<<mergeFileCount<<endl;
    vector <string> subFile;

    for(int subFileCounter = 0;subFileCounter < subFileCount;subFileCounter++)
    {
        subFile.push_back(string("SubFile_"+to_string(subFileCounter)+"__phase_0"));
        Table *subFileTable = new Table(subFile[subFileCounter],sortTable->columns);
        tableCatalogue.insertTable(subFileTable);
        vector<vector<int>> sortTableRows;
        for(int blockCounter = subFileCounter*parsedQuery.sortBuffer;blockCounter<min(int(sortTable->blockCount),(subFileCounter+1)*parsedQuery.sortBuffer);blockCounter++)
        {
            Page sortPage = bufferManager.getPage(sortTable->tableName,blockCounter);
            vector<vector<int>> &blockRows = sortPage.getAllRows();
            for(int i=0;i<sortPage.getRowCount();i++)
                sortTableRows.push_back(blockRows[i]);
        }   
        //cout<<sortTableRows.size()<<endl;
        
        
        // TO CHECK     
        // sort(sortTableRows.begin(),sortTableRows.end(),
        //         [](const vector<int>& a, const vector<int> &b) {
        //             return a[sortIndex] < b[sortIndex];
        //         });
        // BLACK BOX HERE
        
        
        int rowCounter = 0,pageCounter = 0, rowPageCounter = 0;
        vector <vector <int> > outRows;
        while(rowCounter < sortTableRows.size())
        {
            outRows.push_back(sortTableRows[rowCounter]);
            rowCounter++;
            rowPageCounter++;
            if(rowCounter == sortTableRows.size() || rowPageCounter == sortTable->maxRowsPerBlock)
            {
                //cout<<rowCounter<<endl;
                subFileTable->blockCount ++;
                subFileTable->rowCount += outRows.size();
                subFileTable->rowsPerBlockCount.emplace_back(outRows.size());
                bufferManager.writePage(subFile[subFileCounter],pageCounter,outRows,outRows.size());
                rowPageCounter = 0;
                pageCounter++;
                outRows.clear();
            }
        }
    }
    cout<<"SubFiles Generated..."<<endl;

    int subFileMergeCounter = subFileCount,prevPhaseCounter = 0;
    while(subFileMergeCounter != 1)
    {
        int mergeIterations = ceil(float(subFileMergeCounter)/float(parsedQuery.sortBuffer));
        vector <string> mergeFile;
        for(int mergeFileCounter = 0;mergeFileCounter < mergeIterations;mergeFileCounter++)
        {   
            mergeFile.push_back(string("SubFile_"+to_string(mergeFileCounter)+"__phase_"+to_string(mergeFileCounter)));
            Table *mergeFileTable = new Table(mergeFile[mergeFileCounter],sortTable->columns);
            tableCatalogue.insertTable(mergeFileTable);
            vector <Cursor> cursorManager;
            vector <vector <int>> outRows;
            for(int subFileCounter = mergeFileCounter*parsedQuery.sortBuffer;subFileCounter<min(subFileMergeCounter,(mergeFileCounter+1)*parsedQuery.sortBuffer);subFileCounter++)
            {
                string subFileName = "SubFile_"+to_string(subFileCounter)+"__phase_"+to_string(prevPhaseCounter);
                cursorManager.push_back(tableCatalogue.getTable(subFileName)->getCursor());
            }
            int exitFlag = 0,minSubFile = 1e9,minValue = 1e9, pageCounter = 0;
            while(true)
            {
                exitFlag = 0,minSubFile = 1e9,minValue = 1e9;
                for(int mergeCounter = 0; mergeCounter < cursorManager.size();mergeCounter++)
                {
                    vector <int> subFileRow = cursorManager[mergeCounter].getNext();
                    if(subFileRow[sortIndex] < minValue)
                    {
                        exitFlag = 1;
                        minSubFile = mergeCounter;
                        minValue = subFileRow[sortIndex];
                    }
                }
                if(exitFlag == 0) {
                    if(outRows.size() != 0)
                    {
                        mergeFileTable->blockCount ++;
                        mergeFileTable->rowCount += outRows.size();
                        mergeFileTable->rowsPerBlockCount.emplace_back(outRows.size());
                        bufferManager.writePage(mergeFile[mergeFileCounter],pageCounter,outRows,outRows.size());
                    }
                    break;
                }
                vector <int> outRow = cursorManager[minSubFile].getNext();
                outRows.push_back(outRow);
                if(outRows.size() == sortTable->maxRowsPerBlock)
                {
                    mergeFileTable->blockCount ++;
                    mergeFileTable->rowCount += outRows.size();
                    mergeFileTable->rowsPerBlockCount.emplace_back(outRows.size());
                    bufferManager.writePage(mergeFile[mergeFileCounter],pageCounter,outRows,outRows.size());
                    pageCounter++;
                    outRows.clear();
                }
            }
        }
        subFileMergeCounter = mergeIterations;
    }
    
    return;
}