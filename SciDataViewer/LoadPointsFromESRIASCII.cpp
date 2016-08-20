
#include <ionEngine.h>


vector<vec3f> LoadPointsFromESRIASCII(string const & FileName)
{
	vector<vec3f> Points;
	std::ifstream file(FileName);

	if (! file.is_open())
	{
		cerr << FileName << " could not be opened." << endl;
	}


	int ncols;
	int nrows;

	float xllcorner;
	float yllcorner;
	float dx;
	float dy;

	int NODATA;
	int Data;

	string dump;

	file >> dump >> ncols;
	file >> dump >> nrows;
	file >> dump >> xllcorner;
	file >> dump >> yllcorner;
	file >> dump >> dx;
	file >> dump >> dy;
	file >> dump >> NODATA;

	for (int i = 0; i < nrows; ++ i)
	{
		for (int j = 0; j < ncols; ++ j)
		{
			file >> Data;
			Points.push_back(vec3f(yllcorner + dy * i, xllcorner + dx * j, (float) -Data));
		}
	}

	if (Points.size() == 0)
	{
		cout << FileName << " has no points." << endl;
	}

	return Points;
}
