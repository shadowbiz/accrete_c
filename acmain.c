#include "acmain.h"

chemTable gases[] =
	{
		//   An   sym   HTML symbol                      name                 Aw      melt    boil    dens       ABUNDe       ABUNDs         Rea	Max inspired pp
		{AN_H, "H", "H<SUB><SMALL>2</SMALL></SUB>", "Hydrogen", 1.0079, 14.06, 20.40, 8.99e-05, 0.00125893, 27925.4, 1, 0.0},
		{AN_HE, "He", "He", "Helium", 4.0026, 3.46, 4.20, 0.0001787, 7.94328e-09, 2722.7, 0, MAX_HE_IPP},
		{AN_N, "N", "N<SUB><SMALL>2</SMALL></SUB>", "Nitrogen", 14.0067, 63.34, 77.40, 0.0012506, 1.99526e-05, 3.13329, 0, MAX_N2_IPP},
		{AN_O, "O", "O<SUB><SMALL>2</SMALL></SUB>", "Oxygen", 15.9994, 54.80, 90.20, 0.001429, 0.501187, 23.8232, 10, MAX_O2_IPP},
		{AN_NE, "Ne", "Ne", "Neon", 20.1700, 24.53, 27.10, 0.0009, 5.01187e-09, 3.4435e-5, 0, MAX_NE_IPP},
		{AN_AR, "Ar", "Ar", "Argon", 39.9480, 84.00, 87.30, 0.0017824, 3.16228e-06, 0.100925, 0, MAX_AR_IPP},
		{AN_KR, "Kr", "Kr", "Krypton", 83.8000, 116.60, 119.70, 0.003708, 1e-10, 4.4978e-05, 0, MAX_KR_IPP},
		{AN_XE, "Xe", "Xe", "Xenon", 131.3000, 161.30, 165.00, 0.00588, 3.16228e-11, 4.69894e-06, 0, MAX_XE_IPP},
		//                                                                     from here down, these columns were originally: 0.001,         0
		{AN_NH3, "NH3", "NH<SUB><SMALL>3</SMALL></SUB>", "Ammonia", 17.0000, 195.46, 239.66, 0.001, 0.002, 0.0001, 1, MAX_NH3_IPP},
		{AN_H2O, "H2O", "H<SUB><SMALL>2</SMALL></SUB>O", "Water", 18.0000, 273.16, 373.16, 1.000, 0.03, 0.001, 0, 0.0},
		{AN_CO2, "CO2", "CO<SUB><SMALL>2</SMALL></SUB>", "CarbonDioxide", 44.0000, 194.66, 194.66, 0.001, 0.01, 0.0005, 0, MAX_CO2_IPP},
		{AN_O3, "O3", "O<SUB><SMALL>3</SMALL></SUB>", "Ozone", 48.0000, 80.16, 161.16, 0.001, 0.001, 0.000001, 2, MAX_O3_IPP},
		{AN_CH4, "CH4", "CH<SUB><SMALL>4</SMALL></SUB>", "Methane", 16.0000, 90.16, 109.16, 0.010, 0.005, 0.0001, 1, MAX_CH4_IPP},
		{0, "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int max_gas = (sizeof(gases) / sizeof(chemTable)) - 1;

void acInitGenerator()
{
	srand(time(NULL));

	for (int index = 0; index < max_gas; index++)
	{
		if (gases[index].max_ipp == 0.0)
		{
			gases[index].max_ipp = INCREDIBLY_LARGE_NUMBER;
		}
	}

	qsort(gases, (sizeof(gases) / sizeof(chemTable)) - 1, sizeof(*gases), diminishingAbundance);
}

void acGeneratePlanet(accrete *ac, Sun *sun, planetPointer planet, int planetNo, bool is_moon)
{
	double tmp;
	double eccCoef = ECCENTRICITY_COEFF;

	planet->atmosphere = NULL;
	planet->gases = 0;
	planet->surfaceTemperature = 0;
	planet->highTemperature = 0;
	planet->lowTemperature = 0;
	planet->maxTemperature = 0;
	planet->minTemperature = 0;
	planet->greenhouseRise = 0;
	planet->planetNo = planetNo;
	planet->resonantPeriod = false;
	planet->orbitalZone = acOrbitalZone(sun->luminosity, planet->a);
	planet->orbitalPeriod = acPeriod(planet->a, planet->mass, sun->mass);
	planet->axialTilt = acInclination(planet->a);
	planet->exosphericTemperature = EARTH_EXOSPHERE_TEMP / sePow2(planet->a / sun->r_ecosphere);
	planet->rmsVelocity = acRmsVelocity(MOLECULAR_NITROGEN, planet->exosphericTemperature);
	planet->coreRadius = acKothariRadius(planet->dustMass, false, planet->orbitalZone);
	planet->sun = sun;

	// Calculate the radius as a gas giant, to verify it will retain gas.
	// Then if mass > Earth, it's at least 5% gas and retains He, it's
	// some flavor of gas giant.

	planet->density = acEmpiricalDensity(planet->mass, planet->a, sun->r_ecosphere, true);
	planet->radius = acVolumeRadius(planet->mass, planet->density);

	planet->surfaceAcceleration = acAcceleration(planet->mass, planet->radius);
	planet->surfaceGravity = acGravity(planet->surfaceAcceleration);

	planet->moleculeWeight = acMinMoleculeWeight(planet);

	// seb jup = 317.83 earth masses
	// 6xJup = star
	// arbitrarily 4xJup = BrownDwarf = 1271.32 --> 1200

	if (planet->mass * SUN_MASS_IN_EARTH_MASSES > 1.0 && planet->gas_mass / planet->mass > 0.05 && acMinMoleculeWeight(planet) <= 4.0)
	{
		if (planet->gas_mass / planet->mass < 0.20)
		{
			planet->planetClass = tSubSubGasGiant;
		}
		else if (planet->mass * SUN_MASS_IN_EARTH_MASSES < 20.0)
		{
			planet->planetClass = tSubGasGiant;
		}
		else if (planet->mass * SUN_MASS_IN_EARTH_MASSES > 1200.0) //seb
		{
			planet->planetClass = tBrownDwarf; //seb
		}
		else
		{
			planet->planetClass = tGasGiant;
		}
	}
	else // If not, it's rocky.
	{
		planet->radius = acKothariRadius(planet->mass, false, planet->orbitalZone);
		planet->density = acVolumeDensity(planet->mass, planet->radius);

		planet->surfaceAcceleration = acAcceleration(planet->mass, planet->radius);
		planet->surfaceGravity = acGravity(planet->surfaceAcceleration);

		if (planet->gas_mass / planet->mass > 0.000001)
		{
			double h2_mass = planet->gas_mass * 0.85;
			double he_mass = (planet->gas_mass - h2_mass) * 0.999;

			double h2_loss = 0.0;
			double he_loss = 0.0;

			double h2_life = acGasLife(MOLECULAR_HYDROGEN, planet);
			double he_life = acGasLife(HELIUM, planet);

			if (h2_life < sun->age)
			{
				h2_loss = ((1.0 - (1.0 / exp(sun->age / h2_life))) * h2_mass);

				planet->gas_mass -= h2_loss;
				planet->mass -= h2_loss;

				planet->surfaceAcceleration = acAcceleration(planet->mass, planet->radius);
				planet->surfaceGravity = acGravity(planet->surfaceAcceleration);
			}

			if (he_life < sun->age)
			{
				he_loss = ((1.0 - (1.0 / exp(sun->age / he_life))) * he_mass);

				planet->gas_mass -= he_loss;
				planet->mass -= he_loss;

				planet->surfaceAcceleration = acAcceleration(planet->mass, planet->radius);
				planet->surfaceGravity = acGravity(planet->surfaceAcceleration);
			}
		}
	}

	//seb: recalculate planet radius for largest categories
	// formulae taken from Fig 4 in Fortney et al. (2009)
	// Rjup = 71492.0 km

	tmp = (planet->mass * SUN_MASS_IN_EARTH_MASSES) / 317.82; // mass as frac of Mj
	if ((planet->planetClass == tGasGiant || planet->planetClass == tBrownDwarf) && tmp > 0.22)
	{
		if (tmp < 1.65)
		{
			planet->radius = (pow(tmp, 0.1) * 71492.0);
		} // planet
		else if (tmp < 6.04)
		{
			planet->radius = (pow(tmp, -0.125) * 71492.0);
		} // brown dwarf
		else
		{
			planet->radius = (0.799 * 71492.0 + (tmp - 6.04) * 71492.0);
		} // star
	}

	planet->day = acDayLength(planet); /* Modifies planet->resonant_period */
	// seb: tidally locked by definition has 0 tilt
	if ((int)planet->day == (int)(planet->orbitalPeriod * 24.0))
	{
		planet->axialTilt = 0.0;
	}

	planet->escapeVelocity = acEscapeVelocity(planet->mass, planet->radius);

	if (planet->planetClass == tGasGiant || planet->planetClass == tBrownDwarf || planet->planetClass == tSubGasGiant || planet->planetClass == tSubSubGasGiant)
	{
		planet->greenhouseEffect = false;
		planet->volatileGasInventory = INCREDIBLY_LARGE_NUMBER;
		planet->surfacePressure = INCREDIBLY_LARGE_NUMBER;
		planet->boilPoint = INCREDIBLY_LARGE_NUMBER;
		planet->greenhouseRise = 0;
		planet->albedo = seAbout(GAS_GIANT_ALBEDO, 0.1);
		planet->hydrosphere = 1.0;
		planet->cloudCover = 1.0;
		planet->iceCover = 0.0;
		planet->surfaceGravity = acGravity(planet->surfaceAcceleration);
		planet->moleculeWeight = acMinMoleculeWeight(planet);
		planet->estimatedTemperature = acEstimatedTemperature(sun->r_ecosphere, planet->a, planet->albedo);
		planet->estimatedTerrainTemperature = acEstimatedTemperature(sun->r_ecosphere, planet->a, EARTH_ALBEDO);

		double temp = planet->estimatedTerrainTemperature;

		if (temp >= FREEZING_POINT_OF_WATER && temp <= EARTH_AVERAGE_KELVIN + 10.0 && sun->age > 2.0E9)
		{
			ac->habitable_jovians++;
		}
	}
	else
	{
		planet->estimatedTemperature = acEstimatedTemperature(sun->r_ecosphere, planet->a, EARTH_ALBEDO);
		planet->estimatedTerrainTemperature = acEstimatedTemperature(sun->r_ecosphere, planet->a, EARTH_ALBEDO);
		planet->surfaceGravity = acGravity(planet->surfaceAcceleration);
		planet->moleculeWeight = acMinMoleculeWeight(planet);
		planet->greenhouseEffect = acGreenhouse(sun->r_ecosphere, planet->a);
		planet->volatileGasInventory = acVolInventory(planet->mass, planet->escapeVelocity, planet->rmsVelocity, sun->mass, planet->orbitalZone, planet->greenhouseEffect, (planet->gas_mass / planet->mass) > 0.000001);
		planet->surfacePressure = acPressure(planet->volatileGasInventory, planet->radius, planet->surfaceGravity);

		if (planet->surfacePressure == 0.0)
		{
			planet->boilPoint = 0.0;
		}
		else
		{
			planet->boilPoint = acBoilingPoint(planet->surfacePressure);
		}

		acIterateSurfaceTemp(planet); /*	Sets:
								 *		planet->surf_temp
								 *		planet->greenhs_rise
								 *		planet->albedo
								 *		planet->hydrosphere
								 *		planet->cloud_cover
								 *		planet->ice_cover
								 */

		//seb: always want atmospheric composition, especially for cold planets
		//&&
		//				(planet->max_temp >= FREEZING_POINT_OF_WATER) &&
		//				(planet->min_temp <= planet->boil_point))
		if (planet->surfacePressure > 0.05) //seb: but only if there's some air
		{
			acCalculateGases(sun, planet);
		}

		/*
		 *	Next we assign a type to the planet.
		 */

		if (planet->surfacePressure < 1.0)
		{
			if (!is_moon && ((planet->mass * SUN_MASS_IN_EARTH_MASSES) < ASTEROID_MASS_LIMIT))
			{
				planet->planetClass = tAsteroids;
			}
			else
			{
				planet->planetClass = tRock;
			}
		}
		else if ((planet->surfacePressure > 6000.0) && (planet->moleculeWeight <= 2.0)) // Retains Hydrogen
		{
			planet->planetClass = tSubSubGasGiant;
			//seb: want gas damit				planet->gases = 0;
			//free(planet->atmosphere);
			//planet->atmosphere = NULL;
		}
		else
		{ // Atmospheres:
			if (((int)planet->day == (int)(planet->orbitalPeriod * 24.0) || (planet->resonantPeriod)))
			{
				planet->planetClass = t1Face;
			}
			else if (planet->hydrosphere >= 0.95)
			{
				planet->planetClass = tWater; // >95% water
			}
			else if (planet->iceCover >= 0.95)
			{
				planet->planetClass = tIce; // >95% ice
			}
			else if (planet->hydrosphere > 0.05)
			{
				planet->planetClass = tTerrestrial; // Terrestrial
													// else <5% water
			}
			else if (planet->maxTemperature > planet->boilPoint)
			{
				planet->planetClass = tVenusian; // Hot = Venusian
			}
			else if ((planet->gas_mass / planet->mass) > 0.0001)
			{										   // Accreted gas
				planet->planetClass = tIce;			   // But no Greenhouse
				planet->iceCover = 1.0;				   // or liquid water
			}										   // Make it an Ice World
			else if (planet->surfacePressure <= 250.0) // Thin air = Martian
			{
				planet->planetClass = tMartian;
			}
			else if (planet->surfaceTemperature < FREEZING_POINT_OF_WATER)
			{
				planet->planetClass = tIce;
			}
			else
			{
				planet->planetClass = tUnknown;
			}
		}
	}

	if (!is_moon)
	{
		if (planet->first_moon != NULL)
		{
			int n;
			planetPointer ptr;

			for (n = 0, ptr = planet->first_moon; ptr != NULL; ptr = ptr->nextPlanet)
			{
				if (ptr->mass * SUN_MASS_IN_EARTH_MASSES > .000001)
				{
					char moon_id[80];
					double roche_limit = 0.0;
					double hill_sphere = 0.0;

					ptr->a = planet->a;
					ptr->e = planet->e;

					n++;

					sprintf(moon_id, "%i.%d", planetNo, n);

					acGeneratePlanet(ac, sun, ptr, n, true); // Adjusts ptr->density

					roche_limit = 2.44 * planet->radius * pow((planet->density / ptr->density), (1.0 / 3.0));
					hill_sphere = planet->a * KM_PER_AU * pow((planet->mass / (3.0 * sun->mass)), (1.0 / 3.0));

					if ((roche_limit * 3.0) < hill_sphere)
					{
						ptr->moon_a = seRandomNumber(roche_limit * 1.5, hill_sphere / 2.0) / KM_PER_AU;
						ptr->moon_e = RandomEccentricity(ECCENTRICITY_COEFF);
					}
					else
					{
						ptr->moon_a = 0;
						ptr->moon_e = 0;
					}
				}
			}
		}
	}
}

void acGenetatePlanets(accrete *ac, Sun *sun, planetPointer innermost_planet)
{
	planetPointer planet;
	planetPointer moon;
	int planet_no = 0;
	int moons = 0;

	for (planet = innermost_planet, planet_no = 1; planet != NULL; planet = planet->nextPlanet, planet_no++)
	{
		char planet_id[80];

		acGeneratePlanet(ac, sun, planet, planet_no, false);

		/*
		 *	Now we're ready to test for habitable planets,
		 *	so we can count and log them and such
		 */

		//check_planet(planet, planet_id, false);

		for (moon = planet->first_moon, moons = 1; moon != NULL; moon = moon->nextPlanet, moons++)
		{
			char moon_id[80];
			//check_planet(moon, moon_id, true);
		}
	}
}

void acGenerateStellarSystem(accrete *ac)
{
	planetPointer innermost_planet = NULL;

	ac->radians_per_rotation = 2.0 * PI;

	ac->sun.mass = seRandomNumber(0.6, 1.3);
	ac->sun.luminosity = acLuminosity(ac->sun.mass);
	ac->sun.life = 1.0E10 * (ac->sun.mass / ac->sun.luminosity);

	double min_age = 1.0E9;
	double max_age = 6.0E9;

	if (ac->sun.life < max_age)
	{
		max_age = ac->sun.life;
	}

	ac->sun.age = seRandomNumber(min_age, max_age);

	ac->sun.r_ecosphere = sqrt(ac->sun.luminosity);
	ac->r_greenhouse = ac->sun.r_ecosphere * GREENHOUSE_EFFECT_CONST;

	double outerLimit = acStellarDustLimit(ac->sun.mass);
	double innerLimit = 0.0;

	innermost_planet = acDistributePlanetaryMasses(ac, ac->sun.mass, ac->sun.luminosity, innerLimit, outerLimit);

	acGenetatePlanets(ac, &ac->sun, innermost_planet);
	acDisplaySystem(ac);
}

void acFreeData(accrete *ac)
{
	if (!ac)
		return;

	acFreeAtmosphere(ac->planetHead);
	acFreePlanets(ac->planetHead);

	free(ac);
}

void acGetPlanets(accrete *ac, planetPointer planets)
{
	planets = ac->planetHead;
}