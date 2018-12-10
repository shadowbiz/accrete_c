#include "acenviro.h"

double acLuminosity(double massRatio)
{
	/* revised 25oct11 seb from discussion on wikipedia*/
	if (massRatio <= 0.6224)
	{
		return 0.3815 * pow(massRatio, 2.5185);
	}
	else if (massRatio <= 1.0)
	{
		return pow(massRatio, 4.551);
	}
	else if (massRatio <= 3.1623)
	{
		return pow(massRatio, 4.351);
	}
	else if (massRatio <= 16.0)
	{
		return 2.7563 * pow(massRatio, 3.4704);
	}
	else
	{
		return 42.321 * pow(massRatio, 2.4853);
	}
}

/*--------------------------------------------------------------------------*/
/*   This function, given the orbital radius of a planet in AU, returns     */
/*   the orbital 'zone' of the particle.                                    */
/*--------------------------------------------------------------------------*/
int acOrbitalZone(double luminosity, double orbital_radius)
{
	if (orbital_radius < 4.0 * sqrt(luminosity))
	{
		return 1;
	}
	else
	{
		if (orbital_radius >= 4.0 * sqrt(luminosity) && orbital_radius < 15.0 * sqrt(luminosity))
		{
			return 2;
		}
		else
		{
			return 3;
		}
	}
}

/*--------------------------------------------------------------------------*/
/*   The mass is in units of solar masses, and the density is in units      */
/*   of grams/cc.  The radius returned is in units of km.                   */
/*--------------------------------------------------------------------------*/
double acVolumeRadius(double mass, double density)
{
	double volume;

	mass = mass * SOLAR_MASS_IN_GRAMS;
	volume = mass / density;
	return pow((3.0 * volume) / (4.0 * PI), (1.0 / 3.0)) / CM_PER_KM;
}

/*--------------------------------------------------------------------------*/
/*    Returns the radius of the planet in kilometers.                       */
/*   The mass passed in is in units of solar masses, the orbital radius     */
/*   in A.U.                                                                */
/*   This formula is listed as eq.9 in Fogg's article, although some typos  */
/*   crop up in that eq.  See "The Internal Constitution of Planets", by    */
/*   Dr. D. S. Kothari, Mon. Not. of the Royal Astronomical Society, vol 96 */
/*   pp.833-843, 1936 for the derivation.  Specifically, this is Kothari's  */
/*   eq.23, which appears on page 840.                                      */
/*--------------------------------------------------------------------------*/
double acKothariRadius(double mass, bool giant, int zone)
{
	volatile double temp1;
	double temp, temp2, atomicWeight, atomicNum;

	if (zone == 1)
	{
		if (giant)
		{
			atomicWeight = 9.5;
			atomicNum = 4.5;
		}
		else
		{
			atomicWeight = 15.0;
			atomicNum = 8.0;
		}
	}
	else if (zone == 2)
	{
		if (giant)
		{
			atomicWeight = 2.47;
			atomicNum = 2.0;
		}
		else
		{
			atomicWeight = 10.0;
			atomicNum = 5.0;
		}
	}
	else
	{
		if (giant)
		{
			atomicWeight = 7.0;
			atomicNum = 4.0;
		}
		else
		{
			atomicWeight = 10.0;
			atomicNum = 5.0;
		}
	}

	temp1 = atomicWeight * atomicNum;

	temp = (2.0 * BETA_20 * pow(SOLAR_MASS_IN_GRAMS, 1.0 / 3.0)) / (A1_20 * pow(temp1, 1.0 / 3.0));
	temp2 = A2_20 * pow(atomicWeight, 4.0 / 3.0) * pow(SOLAR_MASS_IN_GRAMS, 2.0 / 3.0);
	temp2 = temp2 * pow(mass, 2.0 / 3.0);
	temp2 = temp2 / (A1_20 * sePow2(atomicNum));
	temp2 = 1.0 + temp2;
	temp = temp / temp2;
	temp = (temp * pow(mass, 1.0 / 3.0)) / CM_PER_KM;

	temp /= JIMS_FUDGE;

	return temp;
}

/*--------------------------------------------------------------------------*/
/*  The mass passed in is in units of solar masses, and the orbital radius  */
/*  is in units of AU.  The density is returned in units of grams/cc.       */
/*--------------------------------------------------------------------------*/
double acEmpiricalDensity(double mass, double orbital_radius, double r_ecosphere, bool gas_giant)
{
	double temp;

	temp = pow(mass * SUN_MASS_IN_EARTH_MASSES, 1.0 / 8.0);
	temp = temp * pow(r_ecosphere / orbital_radius, 1.0 / 4.0);

	if (gas_giant)
	{
		return temp * 1.2;
	}
	else
	{
		return temp * 5.5;
	}
}

/*--------------------------------------------------------------------------*/
/*  The mass passed in is in units of solar masses, and the equatorial      */
/*  radius is in km.  The density is returned in units of grams/cc.         */
/*--------------------------------------------------------------------------*/
double acVolumeDensity(double mass, double equatorial_radius)
{
	double volume;

	mass = mass * SOLAR_MASS_IN_GRAMS;
	equatorial_radius = equatorial_radius * CM_PER_KM;
	volume = (4.0 * PI * pow(equatorial_radius, 3.0)) / 3.0;
	return mass / volume;
}

/*--------------------------------------------------------------------------*/
/*  The separation is in units of AU, and both masses are in units of solar */
/*  masses.  The period returned is in terms of Earth days.                 */
/*--------------------------------------------------------------------------*/
double acPeriod(double separation, double small_mass, double large_mass)
{
	double period_in_years;
	period_in_years = sqrt(pow(separation, 3.0) / (small_mass + large_mass));

	return period_in_years * DAYS_IN_A_YEAR;
}

/*--------------------------------------------------------------------------*/
/*	 Fogg's information for this routine came from Dole "Habitable Planets	*/
/* for Man", Blaisdell Publishing Company, NY, 1964.  From this, he came	*/
/* up with his eq.12, which is the equation for the 'base_angular_velocity' */
/* below.  He then used an equation for the change in angular velocity per	*/
/* time (dw/dt) from P. Goldreich and S. Soter's paper "Q in the Solar		*/
/* System" in Icarus, vol 5, pp.375-389 (1966).	 Using as a comparison the	*/
/* change in angular velocity for the Earth, Fogg has come up with an		*/
/* approximation for our new planet (his eq.13) and take that into account. */
/* This is used to find 'change_in_angular_velocity' below.					*/
/*																			*/
/*	 Input parameters are mass (in solar masses), radius (in Km), orbital	*/
/* period (in days), orbital radius (in AU), density (in g/cc),				*/
/* eccentricity, and whether it is a gas giant or not.						*/
/*	 The length of the day is returned in units of hours.					*/
/*--------------------------------------------------------------------------*/
double acDayLength(planetPointer planet)
{
	double planetaryMassInGrams = planet->mass * SOLAR_MASS_IN_GRAMS;
	double equatorialRadiusInCm = planet->radius * CM_PER_KM;
	double yearInHours = planet->orbitalPeriod * 24.0;
	bool stopped = false;
	bool giant = (planet->planetClass == tGasGiant || planet->planetClass == tBrownDwarf || planet->planetClass == tSubGasGiant || planet->planetClass == tSubSubGasGiant);
	double k2;
	double baseAngularVelocity;
	double changeInAngularVelocity;
	double angVelocity;
	double spinResonanceFactor;
	double dayInHours;

	planet->resonantPeriod = false; /* Warning: Modify the planet */

	if (giant)
	{
		k2 = 0.24;
	}
	else
	{
		k2 = 0.33;
	}

	baseAngularVelocity = sqrt(2.0 * J * (planetaryMassInGrams) / (k2 * sePow2(equatorialRadiusInCm)));

	/*	This next calculation determines how much the planet's rotation is	 */
	/*	slowed by the presence of the star.								 */

	changeInAngularVelocity = CHANGE_IN_EARTH_ANG_VEL * (planet->density / EARTH_DENSITY) * (equatorialRadiusInCm / EARTH_RADIUS) * (EARTH_MASS_IN_GRAMS / planetaryMassInGrams) * pow(planet->sun->mass, 2.0) * (1.0 / pow(planet->a, 6.0));
	angVelocity = baseAngularVelocity + (changeInAngularVelocity * planet->sun->age);

	/* Now we change from rad/sec to hours/rotation.						 */

	if (angVelocity <= 0.0)
	{
		stopped = true;
		dayInHours = INCREDIBLY_LARGE_NUMBER;
	}
	else
	{
		dayInHours = RADIANS_PER_ROTATION / (SECONDS_PER_HOUR * angVelocity);
	}

	if ((dayInHours >= yearInHours) || stopped)
	{
		if (planet->e > 0.1)
		{
			spinResonanceFactor = (1.0 - planet->e) / (1.0 + planet->e);
			planet->resonantPeriod = true;
			return spinResonanceFactor * yearInHours;
		}
		else
		{
			return yearInHours;
		}
	}

	return dayInHours;
}

/*--------------------------------------------------------------------------*/
/*   The orbital radius is expected in units of Astronomical Units (AU).    */
/*   Inclination is returned in units of degrees.                           */
/*--------------------------------------------------------------------------*/
double acInclination(double orbital_radius)
{
	double temp;

	//temp = (int)(pow(orbital_radius,0.2) * about(EARTH_AXIAL_TILT,0.4));

	temp = fabs(seGauss(33.3));
	temp = pow(orbital_radius / 50.0, 0.2) * temp;
	if (orbital_radius < 1.0)
	{
		temp = orbital_radius * temp;
	}
	return temp;
}

/*--------------------------------------------------------------------------*/
/*   This function implements the escape velocity calculation.  Note that   */
/*  it appears that Fogg's eq.15 is incorrect.                              */
/*  The mass is in units of solar mass, the radius in kilometers, and the   */
/*  velocity returned is in cm/sec.                                         */
/*--------------------------------------------------------------------------*/
double acEscapeVelocity(double mass, double radius)
{
	double mass_in_grams, radius_in_cm;

	mass_in_grams = mass * SOLAR_MASS_IN_GRAMS;
	radius_in_cm = radius * CM_PER_KM;
	return sqrt(2.0 * GRAV_CONSTANT * mass_in_grams / radius_in_cm);
}

/*--------------------------------------------------------------------------*/
/*  This is Fogg's eq.16.  The molecular weight (usually assumed to be N2)  */
/*  is used as the basis of the Root Mean Square velocity of the molecule   */
/*  or atom.  The velocity returned is in cm/sec.                           */
/*--------------------------------------------------------------------------*/
double acRmsVelocity(double molecularWeight, double orbital_radius)
{
	double exospheric_temp;

	exospheric_temp = EARTH_EXOSPHERE_TEMP / pow(orbital_radius, 2.0);
	return sqrt((3.0 * MOLAR_GAS_CONST * exospheric_temp) / molecularWeight) * CM_PER_METER;
}

/*
 *	calculates the number of years it takes for 1/e of a gas to escape
 *	from a planet's atmosphere. 
 *	Taken from Dole p. 34. He cites Jeans (1916) & Jones (1923)
 */
double acGasLife(double molecular_weight, planetPointer planet)
{
	double v = acRmsVelocity(molecular_weight, planet->exosphericTemperature);
	double g = planet->surfaceGravity * EARTH_ACCELERATION;
	double r = (planet->radius * CM_PER_KM);
	double t = (sePow3(v) / (2.0 * sePow2(g) * r)) * exp((3.0 * g * r) / sePow2(v));
	double years = t / (SECONDS_PER_HOUR * 24.0 * DAYS_IN_A_YEAR);

	if (years > 2.0E10)
	{
		years = INCREDIBLY_LARGE_NUMBER;
	}

	return years;
}

double acMinMoleculeWeight(planetPointer planet)
{
	double mass = planet->mass;
	double radius = planet->radius;
	double temp = planet->exosphericTemperature;
	double target = 5.0E9;

	double guess_1 = acMoleculeLimit(mass, radius, temp);
	double guess_2 = guess_1;

	long double life = acGasLife(guess_1, planet);

	int loops = 0;

	if (NULL != planet->sun)
	{
		target = planet->sun->age;
	}

	if (life > target)
	{
		while ((life > target) && (loops++ < 25))
		{
			guess_1 = guess_1 / 2.0;
			life = acGasLife(guess_1, planet);
		}
	}
	else
	{
		while ((life < target) && (loops++ < 25))
		{
			guess_2 = guess_2 * 2.0;
			life = acGasLife(guess_2, planet);
		}
	}

	loops = 0;

	while (((guess_2 - guess_1) > 0.1) && (loops++ < 25))
	{
		long double guess_3 = (guess_1 + guess_2) / 2.0;
		life = acGasLife(guess_3, planet);

		if (life < target)
			guess_1 = guess_3;
		else
			guess_2 = guess_3;
	}

	life = acGasLife(guess_2, planet);

	return (guess_2);
}

/*--------------------------------------------------------------------------*/
/*	 This function returns the smallest molecular weight retained by the	*/
/*	body, which is useful for determining the atmosphere composition.		*/
/*	Mass is in units of solar masses, and equatorial radius is in units of	*/
/*	kilometers.																*/
/*--------------------------------------------------------------------------*/
double acMoleculeLimit(double mass, double equat_radius, double exospheric_temp)
{
	double esc_velocity = acEscapeVelocity(mass, equat_radius);
	return ((3.0 * MOLAR_GAS_CONST * exospheric_temp) / (sePow2((esc_velocity / GAS_RETENTION_THRESHOLD) / CM_PER_METER)));
}

/*--------------------------------------------------------------------------*/
/*   This function returns the smallest molecular weight retained by the    */
/*  body, which is useful for determining the atmosphere composition.       */
/*  Orbital radius is in A.U.(ie: in units of the earth's orbital radius),  *)
    (*  mass is in units of solar masses, and equatorial radius is in units of  */
/*  kilometers.                                                             */
/*--------------------------------------------------------------------------*/
double acOldMoleculeLimit(double orbital_radius, double mass, double equatorial_radius)
{
	double numerator, denominator1, denominator2, escape_velocity, temp;

	escape_velocity = acEscapeVelocity(mass, equatorial_radius);
	return ((3.0 * pow(GAS_RETENTION_THRESHOLD * CM_PER_METER, 2.0) * MOLAR_GAS_CONST * EARTH_EXOSPHERE_TEMP) / pow(escape_velocity, 2.0));
}

/*--------------------------------------------------------------------------*/
/*   This function calculates the surface acceleration of a planet.  The    */
/*  mass is in units of solar masses, the radius in terms of km, and the    */
/*  acceleration is returned in units of cm/sec2.                           */
/*--------------------------------------------------------------------------*/
double acAcceleration(double mass, double radius)
{
	return GRAV_CONSTANT * (mass * SOLAR_MASS_IN_GRAMS) / pow(radius * CM_PER_KM, 2.0);
}

/*--------------------------------------------------------------------------*/
/*   This function calculates the surface gravity of a planet.  The         */
/*  acceleration is in units of cm/sec2, and the gravity is returned in     */
/*  units of Earth gravities.                                               */
/*--------------------------------------------------------------------------*/
double acGravity(double acceleration)
{
	return acceleration / EARTH_ACCELERATION;
}

/*--------------------------------------------------------------------------*/
/* Old grnhouse:                                                            */
/*	Note that if the orbital radius of the planet is greater than or equal	*/
/*	to R_inner, 99% of it's volatiles are assumed to have been deposited in */
/*	surface reservoirs (otherwise, it suffers from the greenhouse effect).	*/
/*--------------------------------------------------------------------------*/
/*	if ((orb_radius < r_greenhouse) && (zone == 1)) */

/*--------------------------------------------------------------------------*/
/*	The new definition is based on the inital surface temperature and what	*/
/*	state water is in. If it's too hot, the water will never condense out	*/
/*	of the atmosphere, rain down and form an ocean. The albedo used here	*/
/*	was chosen so that the boundary is about the same as the old method		*/
/*	Neither zone, nor r_greenhouse are used in this version				JLB	*/
/*--------------------------------------------------------------------------*/

bool acGreenhouse(double r_ecosphere, double orb_radius)
{
	double temp = acEffectiveTemperature(r_ecosphere, orb_radius, GREENHOUSE_TRIGGER_ALBEDO);

	if (temp > FREEZING_POINT_OF_WATER)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*--------------------------------------------------------------------------*/
/*  This implements Fogg's eq.17.  The 'inventory' returned is unitless.    */
/*--------------------------------------------------------------------------*/

double acVolInventory(double mass, double escape_vel, double rms_vel, double stellar_mass, int zone, bool greenhouse_effect, bool accreted_gas)
{
	double velocity_ratio, proportion_const, temp1, temp2, earth_units;

	velocity_ratio = escape_vel / rms_vel;

	if (velocity_ratio >= GAS_RETENTION_THRESHOLD)
	{
		switch (zone)
		{
		case 1:
			proportion_const = 140000.0; /* 100 -> 140 JLB */
			break;

		case 2:
			proportion_const = 75000.0;
			break;

		case 3:
			proportion_const = 250.0;
			break;

		default:
			proportion_const = 0.0;
			printf("Error: orbital zone not initialized correctly!\n");
			break;
		}
		earth_units = mass * SUN_MASS_IN_EARTH_MASSES;
		temp1 = (proportion_const * earth_units) / stellar_mass;
		temp2 = seAbout(temp1, 0.2);

		/*
		temp2 = temp1;
		*/

		if (greenhouse_effect || accreted_gas)
		{
			return temp2;
		}
		else
		{
			return temp2 / 140.0; /* 100 -> 140 JLB */
		}
	}
	else
	{
		return 0.0;
	}
}

/*--------------------------------------------------------------------------*/
/*  This implements Fogg's eq.18.  The pressure returned is in units of     */
/*  millibars (mb).  The gravity is in units of Earth gravities, the radius */
/*  in units of kilometers.                                                 */
/*--------------------------------------------------------------------------*/
double acPressure(double volatile_gas_inventory, double equatorial_radius, double gravity)
{
	equatorial_radius = EARTH_RADIUS_IN_KM / equatorial_radius;
	return volatile_gas_inventory * gravity / pow(equatorial_radius, 2.0);
}

/*--------------------------------------------------------------------------*/
/*   This function returns the boiling point of water in an atmosphere of   */
/*   pressure 'surface_pressure', given in millibars.  The boiling point is */
/*   returned in units of Kelvin.  This is Fogg's eq.21.                    */
/*--------------------------------------------------------------------------*/
double acBoilingPoint(double surface_pressure)
{
	double surface_pressure_in_bars;
	surface_pressure_in_bars = surface_pressure / MILLIBARS_PER_BAR;
	return 1.0 / (log(surface_pressure_in_bars) / -5050.5 + 1.0 / 373.0);
}

/*--------------------------------------------------------------------------*/
/*   This function is Fogg's eq.22.  Given the volatile gas inventory and   */
/*   planetary radius of a planet (in Km), this function returns the        */
/*   fraction of the planet covered with water.                             */
/*   I have changed the function very slightly:  the fraction of Earth's    */
/*   surface covered by water is 71%, not 75% as Fogg used.                 */
/*--------------------------------------------------------------------------*/
double acHydrosphereFraction(double volatile_gas_inventory, double planetary_radius)
{
	double temp;

	temp = (0.71 * volatile_gas_inventory / 1000.0) * pow(EARTH_RADIUS_IN_KM / planetary_radius, 2.0);
	if (temp >= 1.0)
	{
		return 1.0;
	}
	else
	{
		return temp;
	}
}

/*--------------------------------------------------------------------------*/
/*   Given the surface temperature of a planet (in Kelvin), this function   */
/*   returns the fraction of cloud cover available.  This is Fogg's eq.23.  */
/*   See Hart in "Icarus" (vol 33, pp23 - 39, 1978) for an explanation.     */
/*   This equation is Hart's eq.3.                                          */
/*   I have modified it slightly using constants and relationships from     */
/*   Glass's book "Introduction to Planetary Geology", p.46.                */
/*   The 'CLOUD_COVERAGE_FACTOR' is the amount of surface area on Earth     */
/*   covered by one Kg. of cloud.					    */
/*--------------------------------------------------------------------------*/
double acCloudFraction(double surface_temp, double smallest_MW_retained, double equatorial_radius, double hydrosphere_fraction)
{
	double water_vapor_in_kg, fraction, surface_area, hydrosphere_mass;

	if (smallest_MW_retained > WATER_VAPOR)
	{
		return 0.0;
	}
	else
	{
		surface_area = 4.0 * PI * pow(equatorial_radius, 2.0);
		hydrosphere_mass = hydrosphere_fraction * surface_area * EARTH_WATER_MASS_PER_AREA;
		water_vapor_in_kg = (0.00000001 * hydrosphere_mass) * exp(Q2_36 * (surface_temp - 288.0));
		fraction = CLOUD_COVERAGE_FACTOR * water_vapor_in_kg / surface_area;
		if (fraction >= 1.0)
		{
			return 1.0;
		}
		else
		{
			return fraction;
		}
	}
}

/*--------------------------------------------------------------------------*/
/*   Given the surface temperature of a planet (in Kelvin), this function   */
/*   returns the fraction of the planet's surface covered by ice.  This is  */
/*   Fogg's eq.24.  See Hart[24] in Icarus vol.33, p.28 for an explanation. */
/*   I have changed a constant from 70 to 90 in order to bring it more in   */
/*   line with the fraction of the Earth's surface covered with ice, which  */
/*   is approximatly .016 (=1.6%).                                          */
/*--------------------------------------------------------------------------*/
double acIceFraction(double hydrosphere_fraction, double surface_temp)
{
	double temp;

	if (surface_temp > 328.0)
	{
		surface_temp = 328.0;
	}

	temp = pow(((328.0 - surface_temp) / 90.0), 5.0);
	if (temp > 1.5 * hydrosphere_fraction)
	{
		temp = 1.5 * hydrosphere_fraction;
	}
	if (temp >= 1.0)
	{
		return 1.0;
	}
	else
	{
		return temp;
	}
}

/*--------------------------------------------------------------------------*/
/*  This is Fogg's eq.19.  The ecosphere radius is given in AU, the orbital */
/*  radius in AU, and the temperature returned is in Kelvin.		    */
/*--------------------------------------------------------------------------*/
double acEffectiveTemperature(double ecosphere_radius, double orbital_radius, double albedo)
{
	return sqrt(ecosphere_radius / orbital_radius) * pow((1.0 - albedo) / 0.7, 0.25) * EARTH_EFFECTIVE_TEMP;
}

/*--------------------------------------------------------------------------*/
/*	This is Fogg's eq.19.  The ecosphere radius is given in AU, the orbital */
/*	radius in AU, and the temperature returned is in Kelvin.				*/
/*--------------------------------------------------------------------------*/

double acEstimatedTemperature(double ecosphere_radius, double orb_radius, double albedo)
{
	return (sqrt(ecosphere_radius / orb_radius) * sePow1_4((1.0 - albedo) / (1.0 - EARTH_ALBEDO)) * EARTH_AVERAGE_KELVIN);
}

/*--------------------------------------------------------------------------*/
/*  This is Fogg's eq.20, and is also Hart's eq.20 in his "Evolution of     */
/*  Earth's Atmosphere" article.  The effective temperature given is in     */
/*  units of Kelvin, as is the rise in temperature produced by the          */
/*  greenhouse effect, which is returned.                                   */
/*--------------------------------------------------------------------------*/
double acGreenhouseRise(double optical_depth, double effective_temp, double surface_pressure)
{
	double convection_factor;
	convection_factor = EARTH_CONVECTION_FACTOR * pow((surface_pressure / EARTH_SURF_PRES_IN_MILLIBARS), 0.25);
	return (pow(1.0 + 0.75 * optical_depth, 0.25) - 1.0) * effective_temp * convection_factor;
}

/*--------------------------------------------------------------------------*/
/*   The surface temperature passed in is in units of Kelvin.               */
/*   The cloud adjustment is the fraction of cloud cover obscuring each     */
/*   of the three major components of albedo that lie below the clouds.     */
/*--------------------------------------------------------------------------*/
double acPlanetAlbedo(double water_fraction, double cloud_fraction, double ice_fraction, double surface_pressure)
{
	double rock_fraction, cloud_adjustment, components, cloud_contribution;
	double rock_contribution, water_contribution, ice_contribution;

	rock_fraction = 1.0 - water_fraction - ice_fraction;
	components = 0.0;

	if (water_fraction > 0.0)
	{
		components = components + 1.0;
	}

	if (ice_fraction > 0.0)
	{
		components = components + 1.0;
	}

	if (rock_fraction > 0.0)
	{
		components = components + 1.0;
	}

	cloud_adjustment = cloud_fraction / components;

	if (rock_fraction >= cloud_adjustment)
	{
		rock_fraction = rock_fraction - cloud_adjustment;
	}
	else
	{
		rock_fraction = 0.0;
	}
	if (water_fraction > cloud_adjustment)
	{
		water_fraction = water_fraction - cloud_adjustment;
	}
	else
	{
		water_fraction = 0.0;
	}

	if (ice_fraction > cloud_adjustment)
	{
		ice_fraction = ice_fraction - cloud_adjustment;
	}
	else
	{
		ice_fraction = 0.0;
	}

	cloud_contribution = cloud_fraction * seAbout(CLOUD_ALBEDO, 0.2);

	if (surface_pressure == 0.0)
	{
		rock_contribution = rock_fraction * seAbout(AIRLESS_ROCKY_ALBEDO, 0.3);
	}
	else
	{
		rock_contribution = rock_fraction * seAbout(ROCKY_ALBEDO, 0.1);
	}

	water_contribution = water_fraction * seAbout(WATER_ALBEDO, 0.2);

	if (surface_pressure == 0.0)
	{
		ice_contribution = ice_fraction * seAbout(AIRLESS_ICE_ALBEDO, 0.4);
	}
	else
	{
		ice_contribution = ice_fraction * seAbout(ICE_ALBEDO, 0.1);
	}

	return cloud_contribution + rock_contribution + water_contribution + ice_contribution;
}

/*--------------------------------------------------------------------------*/
/*   This function returns the dimensionless quantity of optical depth,     */
/*   which is useful in determining the amount of greenhouse effect on a    */
/*   planet.                                                                */
/*--------------------------------------------------------------------------*/
double acOpacity(double molecular_weight, double surface_pressure)
{
	double optical_depth;

	optical_depth = 0.0;
	if (molecular_weight >= 0.0 && molecular_weight < 10.0)
	{
		optical_depth = optical_depth + 3.0;
	}

	if (molecular_weight >= 10.0 && molecular_weight < 20.0)
	{
		optical_depth = optical_depth + 2.34;
	}

	if (molecular_weight >= 20.0 && molecular_weight < 30.0)
	{
		optical_depth = optical_depth + 1.0;
	}

	if (molecular_weight >= 30.0 && molecular_weight < 45.0)
	{
		optical_depth = optical_depth + 0.15;
	}

	if (molecular_weight >= 45.0 && molecular_weight < 100.0)
	{
		optical_depth = optical_depth + 0.05;
	}

	if (surface_pressure >= 70.0 * EARTH_SURF_PRES_IN_MILLIBARS)
	{
		optical_depth = optical_depth * 8.333;
	}
	else if (surface_pressure >= 50.0 * EARTH_SURF_PRES_IN_MILLIBARS)
	{
		optical_depth = optical_depth * 6.666;
	}
	else if (surface_pressure >= 30.0 * EARTH_SURF_PRES_IN_MILLIBARS)
	{
		optical_depth = optical_depth * 3.333;
	}
	else if (surface_pressure >= 10.0 * EARTH_SURF_PRES_IN_MILLIBARS)
	{
		optical_depth = optical_depth * 2.0;
	}
	else if (surface_pressure >= 5.0 * EARTH_SURF_PRES_IN_MILLIBARS)
	{
		optical_depth = optical_depth * 1.5;
	}

	return optical_depth;
}

void acIterateSurfaceTemp(planetPointer planet)
{
	int count = 0;
	double initial_temp = acEstimatedTemperature(planet->sun->r_ecosphere, planet->a, planet->albedo);

	long double h2_life = acGasLife(MOLECULAR_HYDROGEN, planet);
	long double h2o_life = acGasLife(WATER_VAPOR, planet);
	long double n2_life = acGasLife(MOLECULAR_NITROGEN, planet);
	long double n_life = acGasLife(ATOMIC_NITROGEN, planet);

#ifdef VERBOSE
	if (flag_verbose & 0x20000)
		fprintf(stderr, "%d:                     %5.1Lf it [%5.1Lf re %5.1Lf a %5.1Lf alb]\n",
				planet->planet_no,
				initial_temp,
				planet->sun->r_ecosphere, planet->a, planet->albedo);

	if (flag_verbose & 0x0040)
		fprintf(stderr, "\nGas lifetimes: H2 - %Lf, H2O - %Lf, N - %Lf, N2 - %Lf\n",
				h2_life, h2o_life, n_life, n2_life);
#endif

	acCalculateSurfaceTemp(planet, true, 0, 0, 0, 0, 0);

	for (count = 0; count <= 25; count++)
	{
		double last_water = planet->hydrosphere;
		double last_clouds = planet->cloudCover;
		double last_ice = planet->iceCover;
		double last_temp = planet->surfaceTemperature;
		double last_albedo = planet->albedo;

		acCalculateSurfaceTemp(planet, false, last_water, last_clouds, last_ice, last_temp, last_albedo);

		if (fabs(planet->surfaceTemperature - last_temp) < 0.25)
		{
			break;
		}
	}

	planet->greenhouseRise = planet->surfaceTemperature - initial_temp;

#ifdef VERBOSE
	if (flag_verbose & 0x20000)
		fprintf(stderr, "%d: %5.1Lf gh = %5.1Lf (%5.1Lf C) st - %5.1Lf it [%5.1Lf re %5.1Lf a %5.1Lf alb]\n",
				planet->planet_no,
				planet->greenhs_rise,
				planet->surf_temp,
				planet->surf_temp - FREEZING_POINT_OF_WATER,
				initial_temp,
				planet->sun->r_ecosphere, planet->a, planet->albedo);
#endif
}

/*--------------------------------------------------------------------------*/
/*	 The temperature calculated is in degrees Kelvin.						*/
/*	 Quantities already known which are used in these calculations:			*/
/*		 planet->molec_weight												*/
/*		 planet->surf_pressure												*/
/*		 R_ecosphere														*/
/*		 planet->a															*/
/*		 planet->volatile_gas_inventory										*/
/*		 planet->radius														*/
/*		 planet->boil_point													*/
/*--------------------------------------------------------------------------*/
void acCalculateSurfaceTemp(planetPointer planet, bool first, double last_water, double last_clouds, double last_ice, double last_temp, double last_albedo)
{
	double effective_temp;
	double water_raw;
	double clouds_raw;
	double greenhouse_temp;
	bool boil_off = false;

	if (first)
	{
		planet->albedo = EARTH_ALBEDO;

		effective_temp = acEffectiveTemperature(planet->sun->r_ecosphere, planet->a, planet->albedo);
		greenhouse_temp = acGreenhouseRise(acOpacity(planet->moleculeWeight, planet->surfacePressure), effective_temp, planet->surfacePressure);
		planet->surfaceTemperature = effective_temp + greenhouse_temp;

		set_temp_range(planet);
	}

	if (planet->greenhouseEffect && planet->maxTemperature < planet->boilPoint)
	{
#ifdef VERBOSE
		fprintf(stderr, "Deluge: %s %d max (%Lf) < boil (%Lf)\n",
				planet->sun->name,
				planet->planet_no,
				planet->max_temp,
				planet->boil_point);
#endif
		planet->greenhouseEffect = 0;

		planet->volatileGasInventory = acVolInventory(planet->mass, planet->escapeVelocity, planet->rmsVelocity, planet->sun->mass, planet->orbitalZone, planet->greenhouseEffect, (planet->gas_mass / planet->mass) > 0.000001);
		planet->surfacePressure = acPressure(planet->volatileGasInventory, planet->radius, planet->surfaceGravity);
		planet->boilPoint = acBoilingPoint(planet->surfacePressure);
	}

	water_raw = planet->hydrosphere = acHydrosphereFraction(planet->volatileGasInventory, planet->radius);
	clouds_raw = planet->cloudCover = acCloudFraction(planet->surfaceTemperature, planet->moleculeWeight, planet->radius, planet->hydrosphere);
	planet->iceCover = acIceFraction(planet->hydrosphere, planet->surfaceTemperature);

	if ((planet->greenhouseEffect) && (planet->surfacePressure > 0.0))
	{
		planet->cloudCover = 1.0;
	}

	if ((planet->highTemperature >= planet->boilPoint) && (!first) && !((int)planet->day == (int)(planet->orbitalPeriod * 24.0) || (planet->resonantPeriod)))
	{
		planet->hydrosphere = 0.0;
		boil_off = true;

		if (planet->moleculeWeight > WATER_VAPOR)
		{
			planet->cloudCover = 0.0;
		}
		else
		{
			planet->cloudCover = 1.0;
		}
	}

	if (planet->surfaceTemperature < (FREEZING_POINT_OF_WATER - 3.0))
	{
		planet->hydrosphere = 0.0;
	}

	planet->albedo = acPlanetAlbedo(planet->hydrosphere, planet->cloudCover, planet->iceCover, planet->surfacePressure);

	effective_temp = acEffectiveTemperature(planet->sun->r_ecosphere, planet->a, planet->albedo);
	greenhouse_temp = acGreenhouseRise(acOpacity(planet->moleculeWeight, planet->surfacePressure), effective_temp, planet->surfacePressure);
	planet->surfaceTemperature = effective_temp + greenhouse_temp;

	if (!first)
	{
		if (!boil_off)
		{
			planet->hydrosphere = (planet->hydrosphere + (last_water * 2)) / 3;
		}
		planet->cloudCover = (planet->cloudCover + (last_clouds * 2)) / 3;
		planet->iceCover = (planet->iceCover + (last_ice * 2)) / 3;
		planet->albedo = (planet->albedo + (last_albedo * 2)) / 3;
		planet->surfaceTemperature = (planet->surfaceTemperature + (last_temp * 2)) / 3;
	}

	set_temp_range(planet);

#ifdef VERBOSE
	if (flag_verbose & 0x0020)
		fprintf(stderr, "%5.1Lf AU: %5.1Lf = %5.1Lf ef + %5.1Lf gh%c "
						"(W: %4.2Lf (%4.2Lf) C: %4.2Lf (%4.2Lf) I: %4.2Lf A: (%4.2Lf))\n",
				planet->a,
				planet->surf_temp - FREEZING_POINT_OF_WATER,
				effective_temp - FREEZING_POINT_OF_WATER,
				greenhouse_temp,
				(planet->greenhouseEffect) ? '*' : ' ',
				planet->hydrosphere, water_raw,
				planet->cloud_cover, clouds_raw,
				planet->ice_cover,
				planet->albedo);
#endif
}

void set_temp_range(planetPointer planet)
{
	double pressmod = 1.0 / sqrt(1 + 20 * planet->surfacePressure / 1000.0);
	double ppmod = 1.0 / sqrt(10 + 5 * planet->surfacePressure / 1000.0);
	double tiltmod = fabs(cos(planet->axialTilt * PI / 180) * pow(1 + planet->e, 2));
	double daymod = 1.0 / (200 / planet->day + 1);
	double mh = pow(1 + daymod, pressmod);
	double ml = pow(1 - daymod, pressmod);
	double hi = mh * planet->surfaceTemperature;
	double lo = ml * planet->surfaceTemperature;
	double sh = hi + pow((100 + hi) * tiltmod, sqrt(ppmod));
	double wl = lo - pow((150 + lo) * tiltmod, sqrt(ppmod));
	double max = planet->surfaceTemperature + sqrt(planet->surfaceTemperature) * 10;
	double min = planet->surfaceTemperature / sqrt(planet->day + 24);

	if (lo < min)
		lo = min;
	if (wl < 0)
		wl = 0;

	planet->highTemperature = seSoft(hi, max, min);
	planet->lowTemperature = seSoft(lo, max, min);
	planet->maxTemperature = seSoft(sh, max, min);
	planet->minTemperature = seSoft(wl, max, min);
}

void acCalculateGases(Sun *sun, planetPointer planet)
{
	if (planet->surfacePressure > 0)
	{
		double *amount = (double *)calloc((max_gas + 1), sizeof(double));
		double totamount = 0;
		double pressure = planet->surfacePressure / MILLIBARS_PER_BAR;
		int n = 0;
		int i;

		for (i = 0; i < max_gas; i++)
		{
			double yp = gases[i].boil / (373.0 * ((log((pressure) + 0.001) / -5050.5) + (1.0 / 373.0)));

			if ((yp >= 0 && yp < planet->lowTemperature) && (gases[i].weight >= planet->moleculeWeight))
			{
				double vrms = acRmsVelocity(gases[i].weight, planet->exosphericTemperature);
				double pvrms = pow(1 / (1 + vrms / planet->escapeVelocity), sun->age / 1e9);
				double abund = gases[i].abunds; /* gases[i].abunde */
				double react = 1.0;
				double fract = 1.0;
				double pres2 = 1.0;

				if (strcmp(gases[i].symbol, "Ar") == 0)
				{
					react = 0.15 * sun->age / 4e9;
				}
				else if (strcmp(gases[i].symbol, "He") == 0)
				{
					abund = abund * (0.001 + (planet->gas_mass / planet->mass));
					pres2 = (0.75 + pressure);
					react = pow(1.0 / (1 + gases[i].reactivity), sun->age / 2e9 * pres2);
				}
				else if ((strcmp(gases[i].symbol, "O") == 0 || strcmp(gases[i].symbol, "O3") == 0) && //seb: typo? O2 --> O3
						 sun->age > 2e9 && planet->surfaceTemperature > 270 && planet->surfaceTemperature < 400)
				{
					/*	pres2 = (0.65 + pressure/2);			Breathable - M: .55-1.4 	*/
					pres2 = (0.89 + pressure / 4); /*	Breathable - M: .6 -1.8 	*/
					react = pow(1 / (1 + gases[i].reactivity), pow(sun->age / 2e9, 0.25) * pres2);
				}
				else if (strcmp(gases[i].symbol, "CO2") == 0 && sun->age > 2e9 && planet->surfaceTemperature > 270 && planet->surfaceTemperature < 400)
				{
					pres2 = (0.75 + pressure);
					react = pow(1 / (1 + gases[i].reactivity), pow(sun->age / 2e9, 0.5) * pres2);
					react *= 1.5;
				}
				else
				{
					pres2 = (0.75 + pressure);
					react = pow(1 / (1 + gases[i].reactivity), sun->age / 2e9 * pres2);
				}

				fract = (1 - (planet->moleculeWeight / gases[i].weight));

				amount[i] = abund * pvrms * react * fract;
				totamount += amount[i];
				if (amount[i] > 0.0)
				{
					n++;
				}
			}
			else
			{
				amount[i] = 0.0;
			}
		}

		if (n > 0)
		{
			planet->gases = n;
			planet->atmosphere = (gas *)calloc(n, sizeof(gas));

			for (i = 0, n = 0; i < max_gas; i++)
			{
				if (amount[i] > 0.0)
				{
					planet->atmosphere[n].num = gases[i].num;
					planet->atmosphere[n].surf_pressure = planet->surfacePressure * amount[i] / totamount;
#ifdef VERBOSE
					if (flag_verbose & 0x2000)
					{
						if ((planet->atmosphere[n].num == AN_O) && inspired_partial_pressure(planet->surf_pressure, planet->atmosphere[n].surf_pressure) > gases[i].max_ipp)
						{
							fprintf(stderr, "%s\t Poisoned by O2\n", planet_id);
						}
					}
#endif
					n++;
				}
			}

			qsort(planet->atmosphere, planet->gases, sizeof(gas), diminishingPressure);

#ifdef VERBOSE
			if (flag_verbose & 0x0010)
			{
				fprintf(stderr, "\n%s (%5.1Lf AU) gases:\n",
						planet_id, planet->a);

				for (i = 0; i < planet->gases; i++)
				{
					fprintf(stderr, "%3d: %6.1Lf, %11.7Lf%%\n",
							planet->atmosphere[i].num,
							planet->atmosphere[i].surf_pressure,
							100. * (planet->atmosphere[i].surf_pressure /
									planet->surf_pressure));
				}
			}
#endif
		}

		free(amount);
	}
}

int diminishingAbundance(const void *xp, const void *yp)
{
	const chemTable *x = (chemTable *)xp;
	const chemTable *y = (chemTable *)yp;
	double xx = x->abunds * x->abunde;
	double yy = y->abunds * y->abunde;

	if (xx < yy)
	{
		return +1;
	}
	return (xx > yy ? -1 : 0);
}

/*
 *  Sort a ChemTable by decreasing pressure.
 */

int diminishingPressure(const void *xp, const void *yp)
{
	const gas *x = (gas *)xp;
	const gas *y = (gas *)yp;

	if (x->surf_pressure < y->surf_pressure)
	{
		return +1;
	}
	return (x->surf_pressure > y->surf_pressure ? -1 : 0);
}