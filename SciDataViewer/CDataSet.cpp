
#include "CDataSet.h"


CDataRow::CDataRow(CDataSet * const DataSet, uint const InternalIndex)
{
	this->DataSet = DataSet;
	this->InternalIndex = InternalIndex;
}

void CDataSet::SetSchema(vector<string> const & Schema)
{
	Columns.clear();

	for (auto Field : Schema)
	{
		Columns.push_back(make_pair(Field, vector<double>()));
	}
}

void CDataSet::AddRow(vector<double> const & Values)
{
	assert(Values.size() == Columns.size());
	
	for (size_t i = 0; i < Values.size(); ++ i)
	{
		Columns[i].second.push_back(Values[i]);
	}
}

CDataRow CDataSet::GetRow(uint const i)
{
	return CDataRow(this, i);
}

uint CDataSet::Size() const
{
	return NumRows;
}
