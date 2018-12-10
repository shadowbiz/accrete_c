#ifndef AC_MAIN_H
#define AC_MAIN_H

#include <time.h>
#include "math.h"
#include "constants.h"
#include "structs.h"
#include "globals.h"
#include "acutils.h"
#include "accrete.h"
#include "acenviro.h"
#include "acdisplay.h"
#include "acstructs.h"

//#define	VERBOSE

/*  These are all of the global variables used during accretion:  */

chemTable gases[];
int max_gas;

void acInitGenerator();
void acGenetatePlanets(accrete *ac, Sun *sun, planetPointer innermost_planet);
void acGeneratePlanet(accrete *ac, Sun *sun, planetPointer planet, int planetNo, bool is_moon);
void acGenerateStellarSystem();
void acFreeData(accrete *ac);
void acGetPlanets(accrete *ac, planetPointer planets);

void acFreeDust(dustPointer dust);
void acFreePlanets(planetPointer head);
void acFreeAtmosphere(planetPointer head);

#endif