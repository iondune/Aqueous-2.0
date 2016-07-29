
#pragma once

#include <ionMath.h>


vec2d UTMToLatLon(double const utmX, double const utmY, int const utmZone, bool const isNorthHemisphere = true);
