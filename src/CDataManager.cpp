
#include "CDataManager.h"
#include "CDataLoaderCSV.h"
#include "CParticleSystem.h"


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

void CDataManager::WriteOBJ(string const & FileName)
{
	//SingletonPointer<CParticleSystem> ParticleSystem;
	//ofstream File;

	//File.open(FileName.c_str());

	//if (File.is_open())
	//{
	//	for (auto Point : ParticleSystem->Particles)
	//	{
	//		File << "v " << Point.Position.X << " " << Point.Position.Y << " " << Point.Position.Z << endl;
	//	}
	//}
}
