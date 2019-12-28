#pragma once

#include <cmath>      // fmod

#ifndef M_PI  // M_PI windows definition
namespace
{
	const double M_PI = std::acos(-1.0);
}
#endif