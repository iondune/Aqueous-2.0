
#include "CDataSet.h"


string CDataRow::GetFieldAsString(uint const i) const
{
	return DataSet->Columns[i].second->GetRowAsString(InternalIndex);
}


static inline f64 const string_to_double(std::string const & s)
{
	std::istringstream iss(s);
	f64 t;
	iss >> t;
	return t;
}

double CDataRow::GetFieldAsDouble(uint const i) const
{
	return string_to_double(GetFieldAsString(i));
}

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
		Columns.push_back(make_pair(Field, new CDataColumnString()));
	}
}

void CDataSet::AddRow(vector<string> const & Values)
{
	for (size_t i = 0; i < Columns.size(); ++ i)
	{
		string Value;
		if (i < Values.size())
		{
			Value = Values[i];
		}

		Columns[i].second->PushBackValueFromString(Value);
	}
	NumRows ++;
}

CDataRow CDataSet::GetRow(uint const i)
{
	return CDataRow(this, i);
}

uint CDataSet::Size() const
{
	return NumRows;
}

uint CDataSet::GetColumnCount() const
{
	return (uint) Columns.size();
}

string CDataSet::GetColumnLabel(uint const i)
{
	return Columns[i].first;
}

string CDataColumnString::GetRowAsString(uint const i) const
{
	return Rows[i];
}

string const & CDataColumnString::GetRow(uint const i) const
{
	return Rows[i];
}

void CDataColumnString::PushBackValueFromString(string const & s)
{
	Rows.push_back(s);
}

EDataType CDataColumnString::GetDataType() const
{
	return EDataType::String;
}
