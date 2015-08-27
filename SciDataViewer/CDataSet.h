
#pragma once

#include <ionEngine.h>


class CDataSet;

class CDataRow
{

public:

	string GetFieldAsString(uint const i) const;

private:

	CDataSet * DataSet;
	uint InternalIndex;

	friend class CDataSet;
	CDataRow(CDataSet * const DataSet, uint const InternalIndex);

};

enum class EDataType
{
	String,
	Double,
	Longitude,
	Latitude,
	DateTime
};

class IDataColumn
{

public:

	virtual string GetRowAsString(uint const i) const = 0;
	virtual void PushBackValueFromString(string const & s) = 0;
	virtual EDataType GetDataType() const = 0;

};

class CDataColumnString : public IDataColumn
{

public:

	string GetRowAsString(uint const i) const;
	string const & GetRow(uint const i) const;
	void PushBackValueFromString(string const & s);
	EDataType GetDataType() const;

private:

	vector<string> Rows;

};

class CDataSet
{

public:

	void SetSchema(vector<string> const & Schema);
	void AddRow(vector<string> const & Values);

	CDataRow GetRow(uint const i);
	uint Size() const;
	uint GetColumnCount() const;
	string GetColumnLabel(uint const i);

private:

	vector<pair<string, IDataColumn *>> Columns;
	uint NumRows = 0;

	friend class CDataRow;

};
