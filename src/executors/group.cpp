#include "global.h"

bool syntacticParseGROUP()
{
	logger.log("syntacticParseGROUP");
	string aggregate = tokenizedQuery[8].substr(0, 3);

	if (tokenizedQuery.size() != 9 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "RETURN")
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}

	if (aggregate != "MAX" && aggregate != "MIN" && aggregate != "SUM" && aggregate != "AVG")
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}

	parsedQuery.groupByRelationName = tokenizedQuery[6];
	
	for (int i=4; i<tokenizedQuery[8].size()-1; i++)
	{
		parsedQuery.groupByAggregateColumnName += tokenizedQuery[8][i];
	}

	parsedQuery.groupByOperatorName = aggregate;
	parsedQuery.groupByColumnName = tokenizedQuery[4];
	parsedQuery.groupByResultRelationName = tokenizedQuery[0];
	parsedQuery.queryType = GROUP;
	return true;
}

bool semanticParseGROUP()
{
	logger.log("semanticParseGROUP");

	if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
	{
		cout << "SEMANTIC ERROR: Group By relation doesn't exist" << endl;
		return false;
	}

	if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByColumnName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByAggregateColumnName, parsedQuery.groupByRelationName))
	{
		cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
		return false;
	}

	if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
	{
		cout << "SEMANTIC ERROR: Relation already exists" << endl;
		return false;
	}

	return true;
}

void executeGROUP()
{
	logger.log("executeGROUP");

	Table* table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
	table->groupBy(parsedQuery.groupByResultRelationName, parsedQuery.groupByColumnName, parsedQuery.groupByOperatorName, parsedQuery.groupByAggregateColumnName);
	return;
}
