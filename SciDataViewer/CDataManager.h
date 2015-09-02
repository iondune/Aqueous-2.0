
#pragma once

#include "CDataSet.h"
#include "CDataLoader.h"


class CDataManager : public Singleton<CDataManager>
{

public:

	CDataSet * GetCurrentDataSet();
	void Load(string const & FileName);
	void WriteOBJ(string const & FileName);

private:

	CDataSet * CurrentDataSet = nullptr;

	friend class Singleton<CDataManager>;
	CDataManager() {}

};
