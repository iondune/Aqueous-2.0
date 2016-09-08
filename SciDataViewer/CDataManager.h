
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
	CDataManager()
	{}

};

vector<double> LoadOxymapsCSVData(string const & FileName);
vector<vec3d> LoadPointsFromXYZTxt(string const & FileName, bool commas);
vector<vec3d> LoadPointsFromESRIASCII(string const & FileName);
