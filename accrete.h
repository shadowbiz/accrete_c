#pragma once

#include "acmain.h"
#include "acstructs.h"

accrete *newAccrete();

void acSetInitialConditions(accrete *ac, double innerLimitOfDust, double outerLimitOfDust);
double acStellarDustLimit(double stellar_mass_ratio);
double acInnermostPlanet(double stellar_mass_ratio);
double acOutermostPlanet(double stellar_mass_ratio);
double acInnerEffectLimit(accrete *ac, double a, double e, double mass);
double acOuterEffectLimit(accrete *ac, double a, double e, double mass);
bool acDustAvailable(accrete *ac, double insideRange, double outsideRange);
void acUpdateDustLanes(accrete *ac, double minVal, double maxVal, double mass, double critMass, double bodyInnerBound, double bodyOuterBound);
double acCollectDust(accrete *ac, double lastMass, double *newDust, double *newGas, double a, double e, double critMass, dustPointer dustBand);
double acCriticalLimit(double orbital_radius, double eccentricity, double stellar_luminosity_ratio);
void acAccreteDust(accrete *ac, double *seedMass, double *newDust, double *newGas, double a, double e, double critMass, double bodyInnerBound, double bodyOuterBound);
void acCoalescePlanetesimals(accrete *ac, double a, double e, double mass, double critMass, double dustMass, double gasMass, double stellarLuminosityRatio, double bodyInnerBound, double bodyOuterBound);
planetPointer acDistributePlanetaryMasses(accrete *ac, const double stellarMassRatio, const double stellarLuminosityRatio, const double innerDust, const double outerDust);
