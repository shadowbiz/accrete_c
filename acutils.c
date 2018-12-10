#include "acutils.h"
#include "globals.h"

double RandomEccentricity(double eccCoef)
{
	double e;
	e = 1.0 - sePow(seRandomNumber(0.0, 1.0), eccCoef);

	if (e > .99)
	{
		e = .99; // number less than 10E-26
	}
	return e;
}
