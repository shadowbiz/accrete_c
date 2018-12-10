#include "enviro.h"

char *breathability_phrase[4] =
	{
		"none",
		"breathable",
		"unbreathable",
		"poisonous"};

ldouble luminosity(ldouble massRatio)
{
	if (massRatio <= 0.6224)
	{
		return (0.3815 * pow(massRatio, 2.5185L));
	}
	else if (massRatio <= 1.0)
	{
		return (pow(massRatio, 4.551L));
	}
	else if (massRatio <= 3.1623)
	{
		return (pow(massRatio, 4.351L));
	}
	else if (massRatio <= 16.0)
	{
		return (2.7563 * pow(massRatio, 3.4704L));
	}
	else
	{
		return (42.321 * pow(massRatio, 2.4853L));
	}
}

int orbital_zone(ldouble luminosity, ldouble orbitalRadius)
{
	if (orbitalRadius < (4.0 * sqrt(luminosity)))
	{
		return (1);
	}
	else if (orbitalRadius < (15.0 * sqrt(luminosity)))
	{
		return (2);
	}
	else
	{
		return (3);
	}
}

ldouble volume_radius(ldouble mass, ldouble density)
{
	ldouble volume;

	mass = mass * SOLAR_MASS_IN_GRAMS;
	volume = mass / density;
	return pow((3.0L * volume) / (4.0L * PI), (1.0L / 3.0L)) / CM_PER_KM;
}

ldouble kothari_radius(ldouble mass, bool giant, int zone)
{
	volatile ldouble temp1;
	ldouble temp, temp2, atomicWeight, atomicNum;

	switch (zone)
	{
	case 1:
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
		break;

	case 2:
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
		break;

	default:
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
		break;
	}

	temp1 = atomicWeight * atomicNum;

	temp = (2.0 * BETA_20 * pow(SOLAR_MASS_IN_GRAMS, (1.0 / 3.0))) / (A1_20 * pow(temp1, (1.0L / 3.0L)));

	temp2 = A2_20 * pow(atomicWeight, (4.0L / 3.0)) * pow(SOLAR_MASS_IN_GRAMS, (2.0 / 3.0));
	temp2 = temp2 * pow(mass, (2.0L / 3.0));
	temp2 = temp2 / (A1_20 * pow2(atomicNum));
	temp2 = 1.0 + temp2;
	temp = temp / temp2;
	temp = (temp * pow(mass, (1.0L / 3.0))) / CM_PER_KM;

	temp /= JIMS_FUDGE;

	return temp;
}

ldouble empirical_density(ldouble mass, ldouble orbitalRadius, ldouble rEcosphere, bool isGasGiant)
{
	ldouble temp = pow(mass * SUN_MASS_IN_EARTH_MASSES, (1.0L / 8.0));
	temp = temp * pow1_4(rEcosphere / orbitalRadius);
	return isGasGiant ? temp * 1.2 : temp * 5.5;
}

ldouble volume_density(ldouble mass, ldouble equatorialRadius)
{
	ldouble volume;
	mass = mass * SOLAR_MASS_IN_GRAMS;
	equatorialRadius = equatorialRadius * CM_PER_KM;
	volume = (4.0 * PI * pow3(equatorialRadius)) / 3.0;
	return mass / volume;
}

ldouble period(ldouble separation, ldouble small_mass, ldouble large_mass)
{
	ldouble period_in_years = sqrt(pow3(separation) / (small_mass + large_mass));
	return period_in_years * DAYS_IN_A_YEAR;
}

ldouble day_length(PlanetPointer planet)
{
	ldouble planetary_mass_in_grams = planet->mass * SOLAR_MASS_IN_GRAMS;
	ldouble equatorial_radius_in_cm = planet->radius * CM_PER_KM;
	ldouble year_in_hours = planet->orb_period * 24.0;
	bool giant = planet->type == tGasGiant || planet->type == tBrownDwarf || planet->type == tSubGasGiant || planet->type == tSubSubGasGiant;
	ldouble k2 = giant ? 0.24 : 0.33;
	ldouble base_angular_velocity = sqrt(2.0 * J * planetary_mass_in_grams / k2 * pow2(equatorial_radius_in_cm));
	ldouble change_in_angular_velocity = CHANGE_IN_EARTH_ANG_VEL * (planet->density / EARTH_DENSITY) * (equatorial_radius_in_cm / EARTH_RADIUS) * (EARTH_MASS_IN_GRAMS / planetary_mass_in_grams) * pow(planet->sun->mass, 2.0L) * (1.0 / pow(planet->a, 6.0L));
	ldouble ang_velocity = base_angular_velocity + change_in_angular_velocity * planet->sun->age;
	ldouble spin_resonance_factor;
	ldouble day_in_hours;

	bool stopped = false;

	planet->resonant_period = false;

	if (ang_velocity <= 0.0)
	{
		stopped = true;
		day_in_hours = INCREDIBLY_LARGE_NUMBER;
	}
	else
	{
		day_in_hours = RADIANS_PER_ROTATION / (SECONDS_PER_HOUR * ang_velocity);
	}

	if ((day_in_hours >= year_in_hours) || stopped)
	{
		if (planet->e > 0.1)
		{
			spin_resonance_factor = (1.0 - planet->e) / (1.0 + planet->e);
			planet->resonant_period = true;
			return spin_resonance_factor * year_in_hours;
		}
		else
		{
			return year_in_hours;
		}
	}

	return day_in_hours;
}

ldouble inclination(ldouble orbitalRadius)
{
	ldouble temp = fabs(gaussian(33.3)) * pow(orbitalRadius / 50.0L, 0.2L);
	if (orbitalRadius < 1.0)
	{
		temp = orbitalRadius * temp;
	}

	return temp;
}

ldouble escape_velocity(ldouble mass, ldouble radius)
{
	ldouble mass_in_grams = mass * SOLAR_MASS_IN_GRAMS;
	ldouble radius_in_cm = radius * CM_PER_KM;
	return sqrt(2.0 * GRAV_CONSTANT * mass_in_grams / radius_in_cm);
}

ldouble rms_velocity(ldouble molecularWeight, ldouble exosphericTemp)
{
	return sqrt((3.0 * MOLAR_GAS_CONST * exosphericTemp) / molecularWeight) * CM_PER_METER;
}

ldouble molecule_limit(ldouble mass, ldouble equatorial_radius, ldouble exospheric_temp)
{
	long double esc_velocity = escape_velocity(mass, equatorial_radius);
	return (3.0 * MOLAR_GAS_CONST * exospheric_temp) / (pow2((esc_velocity / GAS_RETENTION_THRESHOLD) / CM_PER_METER));
}

ldouble acceleration(ldouble mass, ldouble radius)
{
	return GRAV_CONSTANT * (mass * SOLAR_MASS_IN_GRAMS) / pow2(radius * CM_PER_KM);
}

ldouble gravity(ldouble acceleration)
{
	return acceleration / EARTH_ACCELERATION;
}

ldouble vol_inventory(ldouble mass, ldouble escape_vel, ldouble rms_vel, ldouble stellar_mass, int zone, bool greenhouse_effect, bool accreted_gas)
{
	ldouble velocity_ratio, proportion_const, temp1, temp2, earth_units;

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
		temp2 = about(temp1, 0.2);
		temp2 = temp1;

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

ldouble pressure(ldouble volatile_gas_inventory, ldouble equatorial_radius, ldouble gravity)
{
	equatorial_radius = EARTH_RADIUS_IN_KM / equatorial_radius;
	return (volatile_gas_inventory * gravity * (EARTH_SURF_PRES_IN_MILLIBARS / 1000.0) / pow2(equatorial_radius));
}

ldouble boiling_point(ldouble surface_pressure)
{
	ldouble surface_pressure_in_bars = surface_pressure / MILLIBARS_PER_BAR;
	return 1.0 / ((log(surface_pressure_in_bars) / -5050.5) + (1.0 / 373.0));
}

ldouble hydrosphere_fraction(ldouble volatile_gas_inventory, ldouble planetary_radius)
{
	long double temp = (0.71 * volatile_gas_inventory / 1000.0) * pow2(EARTH_RADIUS_IN_KM / planetary_radius);

	return temp >= 1.0 ? 1.0 : temp;
}

ldouble cloud_fraction(ldouble surface_temp, ldouble smallest_MW_retained, ldouble euqatorial_radius, ldouble hydrophere_fraction)
{
	ldouble water_vapor_in_kg, fraction, surf_area, hydro_mass;

	if (smallest_MW_retained > WATER_VAPOR)
	{
		return 0.0;
	}
	else
	{
		surf_area = 4.0 * PI * pow2(euqatorial_radius);
		hydro_mass = hydrophere_fraction * surf_area * EARTH_WATER_MASS_PER_AREA;
		water_vapor_in_kg = (0.00000001 * hydro_mass) * exp(Q2_36 * (surface_temp - EARTH_AVERAGE_KELVIN));
		fraction = CLOUD_COVERAGE_FACTOR * water_vapor_in_kg / surf_area;

		return fraction >= 1.0 ? 1.0 : fraction;
	}
}

ldouble ice_fraction(ldouble hydrosphere_fraction, ldouble surface_temp)
{
	long double temp;

	if (surface_temp > 328.0)
	{
		surface_temp = 328.0;
	}
	temp = pow(((328.0 - surface_temp) / 90.0L), 5.0L);
	if (temp > (1.5 * hydrosphere_fraction))
	{
		temp = (1.5 * hydrosphere_fraction);
	}

	return temp >= 1.0 ? 1.0 : temp;
}

ldouble eff_temp(ldouble ecosphere_radius, ldouble orbital_radius, ldouble albedo)
{
	return sqrt(ecosphere_radius / orbital_radius) * pow1_4((1.0 - albedo) / (1.0 - EARTH_ALBEDO)) * EARTH_EFFECTIVE_TEMP;
}

ldouble est_temp(ldouble ecosphere_radius, ldouble orbital_radius, ldouble albedo)
{
	return sqrt(ecosphere_radius / orbital_radius) * pow1_4((1.0 - albedo) / (1.0 - EARTH_ALBEDO)) * EARTH_AVERAGE_KELVIN;
}

bool greenhouse(ldouble r_ecosphere, ldouble greenhouse_radius)
{
	ldouble temp = eff_temp(r_ecosphere, greenhouse_radius, GREENHOUSE_TRIGGER_ALBEDO);

	return temp > FREEZING_POINT_OF_WATER ? true : false;
}

ldouble green_rise(ldouble optical_depth, ldouble effective_temp, ldouble surface_pressure)
{
	ldouble convection_factor = EARTH_CONVECTION_FACTOR * pow(surface_pressure / EARTH_SURF_PRES_IN_MILLIBARS, 0.4L);
	ldouble rise = (pow1_4(1.0 + 0.75 * optical_depth) - 1.0) * effective_temp * convection_factor;

	return rise < 0.0 ? 0.0 : rise;
}

ldouble planet_albedo(ldouble water_fraction, ldouble cloud_fraction, ldouble ice_fraction, ldouble surface_pressure)
{
	ldouble rock_fraction, cloud_adjustment, components, cloud_part, rock_part, water_part, ice_part;

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

	cloud_part = cloud_fraction * CLOUD_ALBEDO; /* about(...,0.2); */

	if (surface_pressure == 0.0)
	{
		rock_part = rock_fraction * ROCKY_AIRLESS_ALBEDO; /* about(...,0.3); */
		ice_part = ice_fraction * AIRLESS_ICE_ALBEDO;	 /* about(...,0.4); */
		water_part = 0;
	}
	else
	{
		rock_part = rock_fraction * ROCKY_ALBEDO;   /* about(...,0.1); */
		water_part = water_fraction * WATER_ALBEDO; /* about(...,0.2); */
		ice_part = ice_fraction * ICE_ALBEDO;		/* about(...,0.1); */
	}

	return cloud_part + rock_part + water_part + ice_part;
}

ldouble opacity(ldouble molecular_weight, ldouble surf_pressure)
{
	long double optical_depth = 0.0;

	if ((molecular_weight >= 0.0) && (molecular_weight < 10.0))
	{
		optical_depth = optical_depth + 3.0;
	}
	if ((molecular_weight >= 10.0) && (molecular_weight < 20.0))
	{
		optical_depth = optical_depth + 2.34;
	}
	if ((molecular_weight >= 20.0) && (molecular_weight < 30.0))
	{
		optical_depth = optical_depth + 1.0;
	}
	if ((molecular_weight >= 30.0) && (molecular_weight < 45.0))
	{
		optical_depth = optical_depth + 0.15;
	}
	if ((molecular_weight >= 45.0) && (molecular_weight < 100.0))
	{
		optical_depth = optical_depth + 0.05;
	}

	if (surf_pressure >= (70.0 * EARTH_SURF_PRES_IN_MILLIBARS))
	{
		optical_depth = optical_depth * 8.333;
	}
	else if (surf_pressure >= (50.0 * EARTH_SURF_PRES_IN_MILLIBARS))
	{
		optical_depth = optical_depth * 6.666;
	}
	else if (surf_pressure >= (30.0 * EARTH_SURF_PRES_IN_MILLIBARS))
	{
		optical_depth = optical_depth * 3.333;
	}
	else if (surf_pressure >= (10.0 * EARTH_SURF_PRES_IN_MILLIBARS))
	{
		optical_depth = optical_depth * 2.0;
	}
	else if (surf_pressure >= (5.0 * EARTH_SURF_PRES_IN_MILLIBARS))
	{
		optical_depth = optical_depth * 1.5;
	}

	return optical_depth;
}

ldouble gas_life(ldouble molecular_weight, PlanetPointer planet)
{
	ldouble v = rms_velocity(molecular_weight, planet->exospheric_temp);
	ldouble g = planet->surf_gravity * EARTH_ACCELERATION;
	ldouble r = (planet->radius * CM_PER_KM);
	ldouble t = (pow3(v) / (2.0 * pow2(g) * r)) * exp((3.0 * g * r) / pow2(v));
	ldouble years = t / (SECONDS_PER_HOUR * 24.0 * DAYS_IN_A_YEAR);

	return years > 2.0E10 ? INCREDIBLY_LARGE_NUMBER : years;
}

ldouble min_molec_weight(PlanetPointer planet)
{
	ldouble mass = planet->mass;
	ldouble radius = planet->radius;
	ldouble temp = planet->exospheric_temp;
	ldouble target = 5.0E9;

	ldouble guess_1 = molecule_limit(mass, radius, temp);
	ldouble guess_2 = guess_1;

	ldouble life = gas_life(guess_1, planet);

	int loops = 0;

	if (planet->sun != NULL)
	{
		target = planet->sun->age;
	}

	if (life > target)
	{
		while ((life > target) && (loops++ < 25))
		{
			guess_1 = guess_1 / 2.0;
			life = gas_life(guess_1, planet);
		}
	}
	else
	{
		while ((life < target) && (loops++ < 25))
		{
			guess_2 = guess_2 * 2.0;
			life = gas_life(guess_2, planet);
		}
	}

	loops = 0;

	while (((guess_2 - guess_1) > 0.1) && (loops++ < 25))
	{
		ldouble guess_3 = (guess_1 + guess_2) / 2.0;
		life = gas_life(guess_3, planet);

		if (life < target)
		{
			guess_1 = guess_3;
		}
		else
		{
			guess_2 = guess_3;
		}
	}

	life = gas_life(guess_2, planet);

	return guess_2;
}

void calculate_surface_temp(PlanetPointer planet, bool first, ldouble last_water, ldouble last_clouds, ldouble last_ice, ldouble last_temp, ldouble last_albedo)
{
	ldouble effective_temp;
	ldouble water_raw;
	ldouble clouds_raw;
	ldouble greenhouse_temp;
	bool boil_off = false;

	if (first)
	{
		planet->albedo = EARTH_ALBEDO;

		effective_temp = eff_temp(planet->sun->r_ecosphere, planet->a, planet->albedo);
		greenhouse_temp = green_rise(opacity(planet->molec_weight, planet->surf_pressure), effective_temp, planet->surf_pressure);
		planet->surf_temp = effective_temp + greenhouse_temp;

		set_temp_range(planet);
	}

	if (planet->greenhouse_effect && planet->max_temp < planet->boil_point)
	{
		printf("Deluge: %s %d max (%Lf) < boil (%Lf)\n", planet->sun->name, planet->planet_no, planet->max_temp, planet->boil_point);

		planet->greenhouse_effect = 0;

		planet->volatile_gas_inventory = vol_inventory(planet->mass, planet->esc_velocity, planet->rms_velocity, planet->sun->mass, planet->orbit_zone, planet->greenhouse_effect, (planet->gas_mass / planet->mass) > 0.000001);
		planet->surf_pressure = pressure(planet->volatile_gas_inventory, planet->radius, planet->surf_gravity);
		planet->boil_point = boiling_point(planet->surf_pressure);
	}

	planet->hydrosphere = hydrosphere_fraction(planet->volatile_gas_inventory, planet->radius);
	water_raw = planet->hydrosphere;
	planet->cloud_cover = cloud_fraction(planet->surf_temp, planet->molec_weight, planet->radius, planet->hydrosphere);
	clouds_raw = planet->cloud_cover;
	planet->ice_cover = ice_fraction(planet->hydrosphere, planet->surf_temp);

	if ((planet->greenhouse_effect) && (planet->surf_pressure > 0.0))
	{
		planet->cloud_cover = 1.0;
	}

	if ((planet->high_temp >= planet->boil_point) && (!first) && !((int)planet->day == (int)(planet->orb_period * 24.0) || (planet->resonant_period)))
	{
		planet->hydrosphere = 0.0;
		boil_off = true;

		if (planet->molec_weight > WATER_VAPOR)
		{
			planet->cloud_cover = 0.0;
		}
		else
		{
			planet->cloud_cover = 1.0;
		}
	}

	if (planet->surf_temp < (FREEZING_POINT_OF_WATER - 3.0))
	{
		planet->hydrosphere = 0.0;
	}

	planet->albedo = planet_albedo(planet->hydrosphere, planet->cloud_cover, planet->ice_cover, planet->surf_pressure);
	effective_temp = eff_temp(planet->sun->r_ecosphere, planet->a, planet->albedo);
	greenhouse_temp = green_rise(opacity(planet->molec_weight, planet->surf_pressure), effective_temp, planet->surf_pressure);
	planet->surf_temp = effective_temp + greenhouse_temp;

	if (!first)
	{
		if (!boil_off)
		{
			planet->hydrosphere = (planet->hydrosphere + (last_water * 2)) / 3;
		}
		planet->cloud_cover = (planet->cloud_cover + (last_clouds * 2)) / 3;
		planet->ice_cover = (planet->ice_cover + (last_ice * 2)) / 3;
		planet->albedo = (planet->albedo + (last_albedo * 2)) / 3;
		planet->surf_temp = (planet->surf_temp + (last_temp * 2)) / 3;
	}

	set_temp_range(planet);

	printf("%5.1Lf AU: %5.1Lf = %5.1Lf ef + %5.1Lf gh%c (W: %4.2Lf (%4.2Lf) C: %4.2Lf (%4.2Lf) I: %4.2Lf A: (%4.2Lf))\n", planet->a, planet->surf_temp - FREEZING_POINT_OF_WATER, effective_temp - FREEZING_POINT_OF_WATER, greenhouse_temp, (planet->greenhouse_effect) ? '*' : ' ', planet->hydrosphere, water_raw, planet->cloud_cover, clouds_raw, planet->ice_cover, planet->albedo);
}

void iterate_surface_temp(PlanetPointer planet)
{
	int count = 0;
	ldouble initial_temp = est_temp(planet->sun->r_ecosphere, planet->a, planet->albedo);

	ldouble h2_life = gas_life(MOLECULAR_HYDROGEN, planet);
	ldouble h2o_life = gas_life(WATER_VAPOR, planet);
	ldouble n2_life = gas_life(MOLECULAR_NITROGEN, planet);
	ldouble n_life = gas_life(ATOMIC_NITROGEN, planet);

	printf("%d:                     %5.1Lf it [%5.1Lf re %5.1Lf a %5.1Lf alb]\n", planet->planet_no, initial_temp, planet->sun->r_ecosphere, planet->a, planet->albedo);
	printf("\nGas lifetimes: H2 - %Lf, H2O - %Lf, N - %Lf, N2 - %Lf\n", h2_life, h2o_life, n_life, n2_life);

	calculate_surface_temp(planet, true, 0, 0, 0, 0, 0);

	for (count = 0; count <= 25; count++)
	{
		ldouble last_water = planet->hydrosphere;
		ldouble last_clouds = planet->cloud_cover;
		ldouble last_ice = planet->ice_cover;
		ldouble last_temp = planet->surf_temp;
		ldouble last_albedo = planet->albedo;

		calculate_surface_temp(planet, false, last_water, last_clouds, last_ice, last_temp, last_albedo);

		if (fabs(planet->surf_temp - last_temp) < 0.25)
		{
			break;
		}
	}

	planet->greenhs_rise = planet->surf_temp - initial_temp;

	printf("%d: %5.1Lf gh = %5.1Lf (%5.1Lf C) st - %5.1Lf it [%5.1Lf re %5.1Lf a %5.1Lf alb]\n", planet->planet_no, planet->greenhs_rise, planet->surf_temp, planet->surf_temp - FREEZING_POINT_OF_WATER, initial_temp, planet->sun->r_ecosphere, planet->a, planet->albedo);
}

ldouble inspired_partial_pressure(ldouble surf_pressure, ldouble gas_pressure)
{
	ldouble pH2O = (H20_ASSUMED_PRESSURE);
	ldouble fraction = gas_pressure / surf_pressure;

	return (surf_pressure - pH2O) * fraction;
}

uint breathability(PlanetPointer planet)
{
	bool oxygen_ok = false;
	int index;

	if (planet->gases == 0)
	{
		return NONE;
	}

	for (index = 0; index < planet->gases; index++)
	{
		int n;
		int gas_no = 0;

		ldouble ipp = inspired_partial_pressure(planet->surf_pressure, planet->atmosphere[index].surf_pressure);

		for (n = 0; n < max_gas; n++)
		{
			if (gases[n].num == planet->atmosphere[index].num)
			{
				gas_no = n;
			}
		}

		if (ipp > gases[gas_no].max_ipp)
		{
			return POISONOUS;
		}

		if (planet->atmosphere[index].num == AN_O)
		{
			oxygen_ok = ((ipp >= MIN_O2_IPP) && (ipp <= MAX_O2_IPP));
		}
	}

	return oxygen_ok ? BREATHABLE : UNBREATHABLE;
}

ldouble lim(ldouble x)
{
	return x / sqrt(sqrt(1 + x * x * x * x));
}

ldouble soft(ldouble v, ldouble max, ldouble min)
{
	ldouble dv = v - min;
	ldouble dm = max - min;
	return (lim(2 * dv / dm - 1) + 1) / 2 * dm + min;
}

void set_temp_range(PlanetPointer planet)
{
	ldouble pressmod = 1 / sqrt(1 + 20 * planet->surf_pressure / 1000.0);
	ldouble ppmod = 1 / sqrt(10 + 5 * planet->surf_pressure / 1000.0);
	ldouble tiltmod = fabs(cos(planet->axial_tilt * PI / 180) * pow(1 + planet->e, 2));
	ldouble daymod = 1 / (200 / planet->day + 1);
	ldouble mh = pow(1 + daymod, pressmod);
	ldouble ml = pow(1 - daymod, pressmod);
	ldouble hi = mh * planet->surf_temp;
	ldouble lo = ml * planet->surf_temp;
	ldouble sh = hi + pow((100 + hi) * tiltmod, sqrt(ppmod));
	ldouble wl = lo - pow((150 + lo) * tiltmod, sqrt(ppmod));
	ldouble max = planet->surf_temp + sqrt(planet->surf_temp) * 10;
	ldouble min = planet->surf_temp / sqrt(planet->day + 24);

	if (lo < min)
	{
		lo = min;
	}
	if (wl < 0)
	{
		wl = 0;
	}

	planet->high_temp = soft(hi, max, min);
	planet->low_temp = soft(lo, max, min);
	planet->max_temp = soft(sh, max, min);
	planet->min_temp = soft(wl, max, min);
}