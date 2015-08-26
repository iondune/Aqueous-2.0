
#pragma once

#include <ionEngine.h>


class CDataSet;

class CDataRow
{

public:

private:

	CDataSet * DataSet;
	uint InternalIndex;

	friend class CDataSet;
	CDataRow(CDataSet * const DataSet, uint const InternalIndex);

};

class CDataSet
{

public:

	void SetSchema(vector<string> const & Schema);
	void AddRow(vector<double> const & Values);

	CDataRow GetRow(uint const i);
	uint Size() const;

private:

	vector<pair<string, vector<double>>> Columns;
	uint NumRows = 0;

};
