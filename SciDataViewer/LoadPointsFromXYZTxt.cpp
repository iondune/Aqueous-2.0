
#include <ionEngine.h>

#include "UTMtoLatLong.h"

vector<vec3f> LoadPointsFromXYZTxt(string const & FileName, bool commas)
{
	vector<vec3f> Points;

	std::ifstream file(FileName);

	if (! file.is_open())
	{
		cerr << FileName << " could not be opened." << endl;
	}

	double northing = 0, easting = 0, elev = 0;
	char comma;

	if (commas)
	{
		while (file >> northing >> comma >> easting >> comma >> elev)
		{
			vec2d const latlong = UTMToLatLon(northing, easting, 11, true);
			Points.push_back(vec3d(latlong.X, latlong.Y, elev));
		}
	}
	else
	{
		while (file >> northing >> easting >> elev)
		{
			vec2d const latlong = UTMToLatLon(northing, easting, 11, true);
			Points.push_back(vec3d(latlong.X, latlong.Y, elev));
		}
	}

	if (Points.size() == 0)
	{
		cout << FileName << " has no points." << endl;
	}

	return Points;
}
