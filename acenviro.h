#pragma once

#include "acmain.h"
#include "acstructs.h"

double acLuminosity(double massRatio);
int acOrbitalZone(double luminosity, double orbital_radius);
double acVolumeRadius(double mass, double density);
double acKothariRadius(double mass, bool giant, int zone);
double acEmpiricalDensity(double mass, double orbital_radius, double r_ecosphere, bool gas_giant);
double acVolumeDensity(double mass, double equatorial_radius);
double acPeriod(double separation, double small_mass, double large_mass);
double acDayLength(planetPointer planet);
double acInclination(double orbital_radius);
double acEscapeVelocity(double mass, double radius);
double acRmsVelocity(double molecular_weight, double orbital_radius);
double acGasLife(double molecular_weight, planetPointer planet);
double acMinMoleculeWeight(planetPointer planet);
double acMoleculeLimit(double mass, double equat_radius, double exospheric_temp);
double acAcceleration(double mass, double radius);
double acGravity(double acceleration);
bool acGreenhouse(double r_ecosphere, double orb_radius);
double acVolInventory(double mass, double escape_vel, double rms_vel, double stellar_mass, int zone, bool greenhouse_effect, bool accreted_gas);
double acPressure(double volatile_gas_inventory, double equatorial_radius, double gravity);
double acBoilingPoint(double surface_pressure);
double acHydrosphereFraction(double volatile_gas_inventory, double planetary_radius);
double acCloudFraction(double surface_temp, double smallest_MW_retained, double equatorial_radius, double hydrosphere_fraction);
double acIceFraction(double hydrosphere_fraction, double surface_temp);
double acEffectiveTemperature(double ecosphere_radius, double orbital_radius, double albedo);
double acEstimatedTemperature(double ecosphere_radius, double orb_radius, double albedo);
double acGreenhouseRise(double optical_depth, double effective_temp, double surface_pressure);
double acPlanetAlbedo(double water_fraction, double cloud_fraction, double ice_fraction, double surface_pressure);
double acOpacity(double molecular_weight, double surface_pressure);
void acIterateSurfaceTemp(planetPointer planet);
void acCalculateSurfaceTemp(planetPointer planet, bool first, double last_water, double last_clouds, double last_ice, double last_temp, double last_albedo);
void set_temp_range(planetPointer planet);
void acCalculateGases(Sun *sun, planetPointer planet);

int diminishingAbundance(const void *xp, const void *yp);
int diminishingPressure(const void *xp, const void *yp);