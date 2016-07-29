
#include "UTMtoLatLong.h"


vec2d UTMToLatLon(double const utmX, double const utmY, int const utmZone, bool const isNorthHemisphere)
{
	static double const diflat = -0.00066286966871111111111111111111111111;
	static double const diflon = -0.0003868060578;

	double c_sa = 6378137.000000;
	double c_sb = 6356752.314245;
	double e2 = pow((pow(c_sa, 2) - pow(c_sb, 2)), 0.5) / c_sb;
	double e2cuadrada = pow(e2, 2);
	double c = pow(c_sa, 2) / c_sb;
	double x = utmX - 500000;
	double y = isNorthHemisphere ? utmY : utmY - 10000000;

	double s = ((utmZone * 6.0) - 183.0);
	double lat = y / (c_sa * 0.9996);
	double v = (c / pow(1 + (e2cuadrada * pow(Cos(lat), 2)), 0.5)) * 0.9996;
	double a = x / v;
	double a1 = sin(2 * lat);
	double a2 = a1 * pow((cos(lat)), 2);
	double j2 = lat + (a1 / 2.0);
	double j4 = ((3 * j2) + a2) / 4.0;
	double j6 = ((5 * j4) + pow(a2 * (cos(lat)), 2)) / 3.0;
	double alfa = (3.0 / 4.0) * e2cuadrada;
	double beta = (5.0 / 3.0) * pow(alfa, 2);
	double gama = (35.0 / 27.0) * pow(alfa, 3);
	double bm = 0.9996 * c * (lat - alfa * j2 + beta * j4 - gama * j6);
	double b = (y - bm) / v;
	double epsi = ((e2cuadrada * pow(a, 2)) / 2.0) * pow((Cos(lat)), 2);
	double eps = a * (1 - (epsi / 3.0));
	double nab = (b * (1 - epsi)) + lat;
	double senoheps = (exp(eps) - exp(-eps)) / 2.0;
	double delt = ArcTan(senoheps / (Cos(nab)));
	double tao = ArcTan(Cos(delt) * Tan(nab));

	return vec2d(
		((lat + (1 + e2cuadrada * pow(Cos(lat), 2) - (3.0 / 2.0) * e2cuadrada * Sin(lat) * Cos(lat) * (tao - lat)) * (tao - lat)) * (180.0 / Constants64::Pi)) + diflat,
		((delt * (180.0 / Constants64::Pi)) + s) + diflon);
}
