
#include "CDataManager.h"
#include "CDataLoaderCSV.h"


CDataSet * CDataManager::GetCurrentDataSet()
{
	return CurrentDataSet;
}

void CDataManager::Load(string const & FileName)
{
	CurrentDataSet = new CDataSet();

	CDataLoaderCSV csv;
	csv.FileName = FileName;
	csv.Load(CurrentDataSet);
}
