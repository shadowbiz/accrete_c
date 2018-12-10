#include "accrete.h"

accrete *
newAccrete()
{
	accrete *result = (accrete *)malloc(sizeof(accrete));
	return result;
}

void acSetInitialConditions(accrete *ac, double innerLimitOfDust, double outerLimitOfDust)
{
	dust *dustHead = (dust *)malloc(sizeof(dust));

	dustHead->nextBand = NULL;
	dustHead->outerEdge = outerLimitOfDust;
	dustHead->innerEdge = innerLimitOfDust;
	dustHead->dustPresent = true;
	dustHead->gasPresent = true;

	ac->dustLeft = true;
	ac->planetHead = NULL;
	ac->dustHead = dustHead;
	ac->cloudEccentricity = seRandomNumber(0.2, 0.8);
}

double
acStellarDustLimit(double stellarMassRatio)
{
	return 200.0 * pow(stellarMassRatio, (1.0 / 3.0));
}

double
acInnermostPlanet(double stellaMassRatio)
{
	return 0.3 * pow(stellaMassRatio, (1.0 / 3.0));
}

double
acOutermostPlanet(double stellarMassRatio)
{
	return 50.0 * pow(stellarMassRatio, (1.0 / 3.0));
}

double
acInnerEffectLimit(accrete *ac, double a, double e, double mass)
{
	return a * (1.0 - e) * (1.0 - mass) / (1.0 + ac->cloudEccentricity);
}

double
acOuterEffectLimit(accrete *ac, double a, double e, double mass)
{
	return a * (1.0 + e) * (1.0 + ac->reducedMass) / (1.0 - ac->cloudEccentricity);
}

bool acDustAvailable(accrete *ac, double insideRange, double outsideRange)
{
	dustPointer currentDustBand;
	bool dustHere;

	currentDustBand = ac->dustHead;
	while (currentDustBand != NULL && currentDustBand->outerEdge < insideRange)
	{
		currentDustBand = currentDustBand->nextBand;
	}

	if (currentDustBand == NULL)
	{
		dustHere = false;
	}
	else
	{
		dustHere = currentDustBand->dustPresent;
	}

	while (currentDustBand != NULL && currentDustBand->innerEdge < outsideRange)
	{
		dustHere = dustHere || currentDustBand->dustPresent;
		currentDustBand = currentDustBand->nextBand;
	}

	return dustHere;
}

void acUpdateDustLanes(accrete *ac, double minVal, double maxVal, double mass, double critMass, double bodyInnerBound, double bodyOuterBound)
{
	bool gas;
	dustPointer node1, node2, node3;

	ac->dustLeft = false;
	if (mass > critMass)
	{
		gas = false;
	}
	else
	{
		gas = true;
	}

	node1 = ac->dustHead;
	while (node1 != NULL)
	{
		if (node1->innerEdge < minVal && node1->outerEdge > maxVal)
		{
			node2 = (dust *)malloc(sizeof(dust));
			node2->innerEdge = minVal;
			node2->outerEdge = maxVal;

			if (node1->gasPresent == true)
			{
				node2->gasPresent = gas;
			}
			else
			{
				node2->gasPresent = false;
			}
			node2->dustPresent = false;
			node3 = (dust *)malloc(sizeof(dust));
			node3->innerEdge = maxVal;
			node3->outerEdge = node1->outerEdge;
			node3->gasPresent = node1->gasPresent;
			node3->dustPresent = node1->dustPresent;
			node3->nextBand = node1->nextBand;
			node1->nextBand = node2;
			node2->nextBand = node3;
			node1->outerEdge = minVal;
			node1 = node3->nextBand;
		}
		else if (node1->innerEdge < maxVal && node1->outerEdge > maxVal)
		{
			node2 = (dust *)malloc(sizeof(dust));
			node2->nextBand = node1->nextBand;
			node2->dustPresent = node1->dustPresent;
			node2->gasPresent = node1->gasPresent;
			node2->outerEdge = node1->outerEdge;
			node2->innerEdge = maxVal;
			node1->nextBand = node2;
			node1->outerEdge = maxVal;
			if (node1->gasPresent == true)
			{
				node1->gasPresent = gas;
			}
			else
			{
				node1->gasPresent = false;
			}
			node1->dustPresent = false;
			node1 = node2->nextBand;
		}
		else if (node1->innerEdge < minVal && node1->outerEdge > minVal)
		{
			node2 = (dust *)malloc(sizeof(dust));
			node2->nextBand = node1->nextBand;
			node2->dustPresent = false;
			if (node1->gasPresent == true)
			{
				node2->gasPresent = gas;
			}
			else
			{
				node2->gasPresent = false;
			}

			node2->outerEdge = node1->outerEdge;
			node2->innerEdge = minVal;
			node1->nextBand = node2;
			node1->outerEdge = minVal;
			node1 = node2->nextBand;
		}
		else if (node1->innerEdge >= minVal && node1->outerEdge <= maxVal)
		{
			if (node1->gasPresent == true)
			{
				node1->gasPresent = gas;
			}
			node1->dustPresent = false;
			node1 = node1->nextBand;
		}
		else if (node1->outerEdge < minVal || node1->innerEdge > maxVal)
		{
			node1 = node1->nextBand;
		}
	}
	node1 = ac->dustHead;
	while (node1 != NULL)
	{
		if (node1->dustPresent && (node1->outerEdge >= bodyInnerBound && node1->innerEdge <= bodyOuterBound))
		{
			ac->dustLeft = true;
		}
		node2 = node1->nextBand;
		if (node2 != NULL)
		{
			if (node1->dustPresent == node2->dustPresent && node1->gasPresent == node2->gasPresent)
			{
				node1->outerEdge = node2->outerEdge;
				node1->nextBand = node2->nextBand;
				free(node2);
			}
		}
		node1 = node1->nextBand;
	}
}

double
acCollectDust(accrete *ac, double lastMass, double *newDust, double *newGas, double a, double e, double critMass, dustPointer dustBand)
{
	double massDensity;
	double temp1;
	double temp2;
	double temp;
	double tempDensity;
	double bandwidth;
	double width;
	double volume;
	double gasDensity = 0.0;
	double newMass;
	double nextMass;
	double nextDust = 0;
	double nextGas = 0;

	temp = lastMass / (1.0 + lastMass);
	ac->reducedMass = pow(temp, (1.0 / 4.0));
	ac->r_inner = acInnerEffectLimit(ac, a, e, ac->reducedMass);
	ac->r_outer = acOuterEffectLimit(ac, a, e, ac->reducedMass);

	if (ac->r_inner < 0.0)
	{
		ac->r_inner = 0.0;
	}
	if (dustBand == NULL)
	{
		return 0.0;
	}
	else
	{
		if (dustBand->dustPresent == false)
		{
			tempDensity = 0.0;
		}
		else
		{
			tempDensity = ac->dustDensity;
		}

		if (lastMass < critMass || dustBand->gasPresent == false)
		{
			massDensity = tempDensity;
		}
		else
		{
			massDensity = K * tempDensity / (1.0 + sqrt(critMass / lastMass) * (K - 1.0));
			gasDensity = massDensity - tempDensity;
		}
		if (dustBand->outerEdge <= ac->r_inner || dustBand->innerEdge >= ac->r_outer)
		{
			return acCollectDust(ac, lastMass, newDust, newGas, a, e, critMass, dustBand->nextBand);
		}
		else
		{
			bandwidth = (ac->r_outer - ac->r_inner);

			temp1 = ac->r_outer - dustBand->outerEdge;
			if (temp1 < 0.0)
			{
				temp1 = 0.0;
			}
			width = bandwidth - temp1;

			temp2 = dustBand->innerEdge - ac->r_inner;
			if (temp2 < 0.0)
			{
				temp2 = 0.0;
			}
			width = width - temp2;

			temp = 4.0 * PI * pow(a, 2.0) * ac->reducedMass * (1.0 - e * (temp1 - temp2) / bandwidth);
			volume = temp * width;

			newMass = volume * massDensity;
			*newGas = volume * gasDensity;
			*newDust = newMass - *newGas;

			nextMass = acCollectDust(ac, lastMass, &nextDust, &nextGas, a, e, critMass, dustBand->nextBand);

			*newGas = *newGas + nextGas;
			*newDust = *newDust + nextDust;

			return newMass + nextMass;
		}
	}
}

/*--------------------------------------------------------------------------*/
/*  Orbital radius is in AU, eccentricity is unitless, and the stellar     */
/*  luminosity ratio is with respect to the sun.  The value returned is the */
/*  mass at which the planet begins to accrete gas as well as dust, and is  */
/*  in units of solar masses.                                               */
/*--------------------------------------------------------------------------*/
double
acCriticalLimit(double orbitalRadius, double eccentricity, double stellarLuminosityRatio)
{
	double temp;
	double perihelionDist;
	perihelionDist = orbitalRadius - orbitalRadius * eccentricity;
	temp = perihelionDist * sqrt(stellarLuminosityRatio);
	return Bc * pow(temp, -0.75);
}

void acAccreteDust(accrete *ac, double *seedMass, double *newDust, double *newGas, double a, double e, double critMass, double bodyInnerBound, double bodyOuterBound)
{
	double perihelionDist;
	double newMass = *seedMass;
	double tempMass;

	do
	{
		tempMass = newMass;
		newMass = acCollectDust(ac, newMass, newDust, newGas, a, e, critMass, ac->dustHead);
	} while (!(newMass - tempMass < 0.0001 * tempMass));

	*seedMass = *seedMass + newMass;
	acUpdateDustLanes(ac, ac->r_inner, ac->r_outer, *seedMass, critMass, bodyInnerBound, bodyOuterBound);
}

void acCoalescePlanetesimals(accrete *ac, double a, double e, double mass, double critMass, double dustMass, double gasMass, double stellarLuminosityRatio, double bodyInnerBound, double bodyOuterBound)
{
	planetPointer thePlanet;
	planetPointer nextPlanet;
	planetPointer prevPlanet;

	planetPointer planetHead = ac->planetHead;

	bool finished;
	double temp;
	double diff;
	double dist1;
	double dist2;

	bool do_moons = true;

	finished = false;
	prevPlanet = NULL;

	for (thePlanet = planetHead; thePlanet != NULL; thePlanet = thePlanet->nextPlanet)
	{
		diff = thePlanet->a - a;

		if (diff > 0.0)
		{
			dist1 = (a * (1.0 + e) * (1.0 + ac->reducedMass)) - a;
			ac->reducedMass = pow((thePlanet->mass / (1.0 + thePlanet->mass)), (1.0 / 4.0));
			dist2 = thePlanet->a - (thePlanet->a * (1.0 - thePlanet->e) * (1.0 - ac->reducedMass));
		}
		else
		{
			dist1 = a - (a * (1.0 - e) * (1.0 - ac->reducedMass));
			ac->reducedMass = pow((thePlanet->mass / (1.0 + thePlanet->mass)), (1.0 / 4.0));
			dist2 = (thePlanet->a * (1.0 + thePlanet->e) * (1.0 + ac->reducedMass)) - thePlanet->a;
		}

		if (fabs(diff) <= fabs(dist1) || fabs(diff) <= fabs(dist2))
		{
			double new_dust = 0;
			double new_gas = 0;
			double new_a = (thePlanet->mass + mass) / ((thePlanet->mass / thePlanet->a) + (mass / a));

			temp = thePlanet->mass * sqrt(thePlanet->a) * sqrt(1.0 - pow(thePlanet->e, 2.0));
			temp = temp + (mass * sqrt(a) * sqrt(sqrt(1.0 - pow(e, 2.0))));
			temp = temp / ((thePlanet->mass + mass) * sqrt(new_a));
			temp = 1.0 - pow(temp, 2.0);
			if (((temp < 0.0) || (temp >= 1.0)))
				temp = 0.0;
			e = sqrt(temp);

			if (do_moons)
			{
				long double existing_mass = 0.0;

				if (thePlanet->first_moon != NULL)
				{
					planetPointer m;

					for (m = thePlanet->first_moon;
						 m != NULL;
						 m = m->nextPlanet)
					{
						existing_mass += m->mass;
					}
				}

				if (mass < critMass)
				{
					if ((mass * SUN_MASS_IN_EARTH_MASSES) < 2.5 && (mass * SUN_MASS_IN_EARTH_MASSES) > .0001 && existing_mass < (thePlanet->mass * .05))
					{
						planetPointer the_moon = (planets *)malloc(sizeof(planets));

						the_moon->planetClass = tUnknown;
						the_moon->mass = mass;
						the_moon->dustMass = dustMass;
						the_moon->gas_mass = gasMass;
						the_moon->atmosphere = NULL;
						the_moon->nextPlanet = NULL;
						the_moon->first_moon = NULL;
						the_moon->gas_giant = false;
						the_moon->atmosphere = NULL;
						the_moon->albedo = 0;
						the_moon->gases = 0;
						the_moon->surfaceTemperature = 0;
						the_moon->highTemperature = 0;
						the_moon->lowTemperature = 0;
						the_moon->maxTemperature = 0;
						the_moon->minTemperature = 0;
						the_moon->greenhouseRise = 0;
						the_moon->minor_moons = 0;

						if ((the_moon->dustMass + the_moon->gas_mass) > (thePlanet->dustMass + thePlanet->gas_mass))
						{
							double temp_dust = thePlanet->dustMass;
							double temp_gas = thePlanet->gas_mass;
							double temp_mass = thePlanet->mass;

							thePlanet->dustMass = the_moon->dustMass;
							thePlanet->gas_mass = the_moon->gas_mass;
							thePlanet->mass = the_moon->mass;

							the_moon->dustMass = temp_dust;
							the_moon->gas_mass = temp_gas;
							the_moon->mass = temp_mass;
						}

						if (thePlanet->first_moon == NULL)
						{
							thePlanet->first_moon = the_moon;
						}
						else
						{
							the_moon->nextPlanet = thePlanet->first_moon;
							thePlanet->first_moon = the_moon;
						}

						finished = true;
					}
				}
			}

			if (!finished)
			{
				temp = thePlanet->mass + mass;
				acAccreteDust(ac, &temp, &new_dust, &new_gas, new_a, e, stellarLuminosityRatio, bodyInnerBound, bodyOuterBound);

				thePlanet->a = new_a;
				thePlanet->e = e;
				thePlanet->mass = temp;
				thePlanet->dustMass += dustMass + new_dust;
				thePlanet->gas_mass += gasMass + new_gas;
				if (temp >= critMass)
				{
					thePlanet->gas_giant = true;
				}

				while (thePlanet->nextPlanet != NULL && thePlanet->nextPlanet->a < new_a)
				{
					nextPlanet = thePlanet->nextPlanet;

					if (thePlanet == planetHead)
					{
						planetHead = nextPlanet;
					}
					else
					{
						prevPlanet->nextPlanet = nextPlanet;
					}

					thePlanet->nextPlanet = nextPlanet->nextPlanet;
					nextPlanet->nextPlanet = thePlanet;
					prevPlanet = nextPlanet;
				}
			}

			finished = true;
			break;
		}
		else
		{
			prevPlanet = thePlanet;
		}
	}

	if (!(finished))
	{
		thePlanet = (planets *)malloc(sizeof(planets));

		thePlanet->planetClass = tUnknown;
		thePlanet->a = a;
		thePlanet->e = e;
		thePlanet->mass = mass;
		thePlanet->dustMass = dustMass;
		thePlanet->gas_mass = gasMass;
		thePlanet->atmosphere = NULL;
		thePlanet->first_moon = NULL;
		thePlanet->atmosphere = NULL;
		thePlanet->albedo = 0;
		thePlanet->gases = 0;
		thePlanet->surfaceTemperature = 0;
		thePlanet->highTemperature = 0;
		thePlanet->lowTemperature = 0;
		thePlanet->maxTemperature = 0;
		thePlanet->minTemperature = 0;
		thePlanet->greenhouseRise = 0;
		thePlanet->minor_moons = 0;

		if ((mass >= critMass))
		{
			thePlanet->gas_giant = true;
		}
		else
		{
			thePlanet->gas_giant = false;
		}

		if (planetHead == NULL)
		{
			planetHead = thePlanet;
			thePlanet->nextPlanet = NULL;
		}
		else if (a < planetHead->a)
		{
			thePlanet->nextPlanet = planetHead;
			planetHead = thePlanet;
		}
		else if (planetHead->nextPlanet == NULL)
		{
			planetHead->nextPlanet = thePlanet;
			thePlanet->nextPlanet = NULL;
		}
		else
		{
			nextPlanet = planetHead;
			while (nextPlanet != NULL && nextPlanet->a < a)
			{
				prevPlanet = nextPlanet;
				nextPlanet = nextPlanet->nextPlanet;
			}
			thePlanet->nextPlanet = nextPlanet;
			prevPlanet->nextPlanet = thePlanet;
		}
	}
}

planetPointer
acDistributePlanetaryMasses(accrete *ac, const double stellarMassRatio, const double stellarLuminosityRatio, const double innerDust, const double outerDust)
{
	double a, e, mass, critMass, planetesimalInnerBound, planetesimalOuterBound;
	double dustMass, gasMass;

	acSetInitialConditions(ac, innerDust, outerDust);

	planetesimalInnerBound = acInnermostPlanet(stellarMassRatio);
	planetesimalOuterBound = acOutermostPlanet(stellarMassRatio);

	while (ac->dustLeft)
	{
		a = seRandomNumber(planetesimalInnerBound, planetesimalOuterBound);
		e = RandomEccentricity(ECCENTRICITY_COEFF);
		mass = PROTOPLANET_MASS;
		dustMass = 0;
		gasMass = 0;

		if (acDustAvailable(ac, acInnerEffectLimit(ac, a, e, mass), acOuterEffectLimit(ac, a, e, mass)))
		{
			ac->dustDensity = DUST_DENSITY_COEFF * sqrt(stellarMassRatio) * exp(-ALPHA * pow(a, 1.0 / N));
			critMass = acCriticalLimit(a, e, stellarLuminosityRatio);
			acAccreteDust(ac, &mass, &dustMass, &gasMass, a, e, critMass, planetesimalInnerBound, planetesimalOuterBound);
			dustMass += PROTOPLANET_MASS;

			if (mass != 0.0 && mass != PROTOPLANET_MASS)
			{
				acCoalescePlanetesimals(ac, a, e, mass, critMass, dustMass, gasMass, stellarLuminosityRatio, planetesimalInnerBound, planetesimalOuterBound);
			}
		}
	}
	acFreeDust(ac->dustHead);
	return ac->planetHead;
}

void acFreeDust(dustPointer head)
{
	dustPointer node;
	dustPointer next;

	for (node = head; node != NULL; node = next)
	{
		next = node->nextBand;
		free(node);
	}
}

void acFreePlanets(planetPointer head)
{
	planetPointer node;
	planetPointer next;

	for (node = head; node != NULL; node = next)
	{
		next = node->nextPlanet;
		free(node);
	}
}

void acFreeAtmosphere(planetPointer head)
{
	planetPointer node;

	for (node = head;
		 node != NULL;
		 node = node->nextPlanet)
	{
		if (node->atmosphere != NULL)
		{
			free(node->atmosphere);

			node->atmosphere = NULL;
		}

		if (node->first_moon != NULL)
		{
			acFreeAtmosphere(node->first_moon);
		}
	}
}