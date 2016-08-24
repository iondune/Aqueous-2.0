
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

	double xllcorner;
	double yllcorner;
	double dx;
	double dy;

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

	for (int j = 0; j < nrows; ++ j)
	{
		for (int i = 0; i < ncols; ++ i)
		{
			file >> Data;
			Points.push_back(vec3d(yllcorner + dy * (nrows - 1 - j), xllcorner + dx * i, (double) -Data));
		}
	}

	if (Points.size() == 0)
	{
		cout << FileName << " has no points." << endl;
	}

	return Points;
}
