#include "global.h"

BufferManager::BufferManager()
{
    logger.log("BufferManager::BufferManager");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::getPage(string tableName, int pageIndex)
{
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(tableName, pageIndex);
}

/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(string pageName)
{
    logger.log("BufferManager::inPool");
    for (auto page : this->pages)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Page 
 */
Page BufferManager::getFromPool(string pageName)
{
    logger.log("BufferManager::getFromPool");
    for (auto page : this->pages)
        if (pageName == page.pageName)
            return page;
}

/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::insertIntoPool(string tableName, int pageIndex)
{
    logger.log("BufferManager::insertIntoPool");
    Page page(tableName, pageIndex);
    if (this->pages.size() >= BLOCK_COUNT)
    {
        if(BUFFER_WRITE_FLAG == 1)
            pages.front().writePage();
        pages.pop_front();
    }
    pages.push_back(page);

    BLOCK_ACCESSES ++;
    return page;
}

/** 
 * @brief This function is responsible for reducing the deque size of the buffer manager in case of resizing in JOIN.
 */
void BufferManager::resetBufferManager()
{
    logger.log("BufferManager::resetBufferManager");
    while (this->pages.size() > BLOCK_COUNT)
    {
        pages.pop_front();
    }
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("BufferManager::writePage");
    Page page(tableName, pageIndex, rows, rowCount);
    page.writePage();
    
    BLOCK_ACCESSES ++;
}

void BufferManager::commitPages()
{
    logger.log("BufferManager::commitPages");
    for (auto page : this->pages)
    {
        page.writePage();
        BLOCK_ACCESSES ++;
    }
}

void BufferManager::appendPage(string tableName, vector<int> row)
{
    //cout<<tableName<<" "<<row[0]<<" getting it: "<<endl;
    logger.log("BufferManager::appendPage");
    Table *outTable = tableCatalogue.getTable(tableName);
    string pageName = "../data/temp/"+tableName + "_Page" + to_string(outTable->blockCount-1);
    deque<Page>::iterator pageIterator;
    //cout<<outTable->blockCount<<" "<<outTable->rowsPerBlockCount[outTable->rowsPerBlockCount.size()-1]<<endl;
    for(pageIterator = pages.begin();pageIterator != pages.end();pageIterator++)
        if(pageIterator->pageName == pageName)
            break;
    if(pageIterator == pages.end())
    {
        pageIterator = pages.begin();
        pageIterator->writePage();
        BLOCK_ACCESSES += 1;
        this->insertIntoPool(tableName,outTable->blockCount-1);
        for(pageIterator = pages.begin();pageIterator != pages.end();pageIterator++)
            if(pageIterator->pageName == pageName)
                break;
    }
    //cout<<outTable->maxRowsPerBlock<<" "<<pageIterator->getRowCount()<<endl;
    if(pageIterator->getRowCount() == outTable->maxRowsPerBlock)
    {
        pageIterator->writePage();
        outTable->blockCount ++;
        outTable->rowsPerBlockCount.emplace_back(1);
        outTable->rowCount ++;
        // if (this->pages.size() >= BLOCK_COUNT)
        // {    
        //     cout<<"yee";
        //     Page delPage = pages.front();
        //     //cout<<"deletion: "<<delPage.pageName<<endl;
        //     delPage.writePage();
        //     pages.pop_front();
        //     BLOCK_ACCESSES += 1;
        // }
        vector<vector<int>> temp;
        temp.push_back(row);    
        Page appendPage(tableName,outTable->blockCount-1,temp,1);
        appendPage.writePage();
        BLOCK_ACCESSES += 2;
    }
    else
    {
        outTable->rowsPerBlockCount[outTable->rowsPerBlockCount.size()-1]++;
        outTable->rowCount++;
        pageIterator->appendRow(row);
    }    
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void BufferManager::deleteFile(string fileName)
{
    logger.log("BufferManager::deleteFile");
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
        else logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteFile(string tableName, int pageIndex)
{
    logger.log("BufferManager::deleteFile");
    string fileName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}
