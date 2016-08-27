
#include "UTMtoLatLong.h"


vec2d UTMToLatLon(double const utmX, double const utmY, int const utmZone, bool const isNorthHemisphere)
{
	static double const diflat = -0.00066286966871111111111111111111111111;
	static double const diflon = -0.0003868060578;

	double const c_sa = 6378137.000000;
	double const c_sb = 6356752.314245;
	double const e2 = pow((pow(c_sa, 2) - pow(c_sb, 2)), 0.5) / c_sb;
	double const e2cuadrada = pow(e2, 2);
	double const c = pow(c_sa, 2) / c_sb;
	double const x = utmX - 500000;
	double const y = isNorthHemisphere ? utmY : utmY - 10000000;

	double const s = ((utmZone * 6.0) - 183.0);
	double const lat = y / (c_sa * 0.9996);
	double const v = (c / pow(1 + (e2cuadrada * pow(Cos(lat), 2)), 0.5)) * 0.9996;
	double const a = x / v;
	double const a1 = sin(2 * lat);
	double const a2 = a1 * pow((cos(lat)), 2);
	double const j2 = lat + (a1 / 2.0);
	double const j4 = ((3 * j2) + a2) / 4.0;
	double const j6 = ((5 * j4) + pow(a2 * (cos(lat)), 2)) / 3.0;
	double const alfa = (3.0 / 4.0) * e2cuadrada;
	double const beta = (5.0 / 3.0) * pow(alfa, 2);
	double const gama = (35.0 / 27.0) * pow(alfa, 3);
	double const bm = 0.9996 * c * (lat - alfa * j2 + beta * j4 - gama * j6);
	double const b = (y - bm) / v;
	double const epsi = ((e2cuadrada * pow(a, 2)) / 2.0) * pow((Cos(lat)), 2);
	double const eps = a * (1 - (epsi / 3.0));
	double const nab = (b * (1 - epsi)) + lat;
	double const senoheps = (exp(eps) - exp(-eps)) / 2.0;
	double const delt = ArcTan(senoheps / (Cos(nab)));
	double const tao = ArcTan(Cos(delt) * Tan(nab));

	return vec2d(
		((lat + (1 + e2cuadrada * pow(Cos(lat), 2) - (3.0 / 2.0) * e2cuadrada * Sin(lat) * Cos(lat) * (tao - lat)) * (tao - lat)) * (180.0 / Constants64::Pi)) + diflat,
		((delt * (180.0 / Constants64::Pi)) + s) + diflon);
}
