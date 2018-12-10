#ifndef AC_SRUCTS_H
#define AC_SRUCTS_H

#include "globals.h"

typedef struct accrete accrete;
typedef struct planetRecord *planetPointer;
typedef struct dustRecord *dustPointer;
typedef struct gen *genPointer;

typedef struct Sun Sun;

typedef struct gas
{
	int num;
	double surf_pressure; /* units of millibars (mb)			 */

} gas;

struct Sun
{
	double luminosity;
	double mass;
	double life;
	double age;
	double r_ecosphere;
	char *name;
};

struct accrete
{
	Sun sun;

	double r_greenhouse;
	double radians_per_rotation;
	int spinResonance;
	int habitable_jovians;

	bool dustLeft;
	double r_inner;
	double r_outer;
	double reducedMass;
	double dustDensity;
	double cloudEccentricity;
	planetPointer planetHead;
	dustPointer dustHead;
};

typedef enum planetType
{
	tUnknown = 0,
	tRock = 1,
	tVenusian = 2,
	tTerrestrial = 3,
	tGasGiant = 4, //Jupiter
	tMartian = 5,
	tWater = 6,
	tIce = 7,
	tSubGasGiant = 8,	//Neptune Uranus
	tSubSubGasGiant = 9, //GasDwarf
	tAsteroids = 10,
	t1Face = 11,
	tBrownDwarf = 12 //seb

} planetType;

typedef enum starType
{
	tO = 0,
	tB = 1,
	tA = 2,
	tF = 3,
	tG = 4,
	tK = 5,
	tM = 6

} starType;

typedef struct gen
{
	dustPointer dusts;
	planetPointer planets;
	genPointer next;

} generation;

typedef struct chemTable
{
	int num;
	char *symbol;
	char *html_symbol;
	char *name;
	double weight;
	double melt;
	double boil;
	double density;
	double abunde;
	double abunds;
	double reactivity;
	double max_ipp; // Max inspired partial pressure im millibars

} chemTable;

typedef struct planetRecord
{
	int planetNo;
	double a;		  /* semi-major axis of the orbit (in AU)*/
	double e;		  /* eccentricity of the orbit	     */
	double axialTilt; /* units of degrees		     */
	double mass;	  /* mass (in solar masses)	     */
	bool gas_giant;   /* TRUE if the planet is a gas giant */
	double dustMass;  /* mass, ignoring gas				 */
	double gas_mass;  /* mass, ignoring dust				 */

	double moon_a; /* semi-major axis of lunar orbit (in AU)*/
	double moon_e; /* eccentricity of lunar orbit		 */

	double coreRadius; /* radius of the rocky core (in km)	 */
	double radius;	 /* equatorial radius (in km)	     */
	int orbitalZone;   /* the 'zone' of the planet          */

	double density;		  /* density (in g/cc)		     */
	double orbitalPeriod; /* length of the local year (days)   */
	double day;			  /* length of the local day (hours)   */
	bool resonantPeriod;  /* TRUE if in resonant rotation   */

	double escapeVelocity;		/* units of cm/sec		     */
	double surfaceAcceleration; /* units of cm/sec2		     */
	double surfaceGravity;		/* units of Earth gravities	     */
	double rmsVelocity;			/* units of cm/sec		     */
	double moleculeWeight;		/* smallest molecular weight retained*/
	double volatileGasInventory;
	double surfacePressure; /* units of millibars (mb)	     */
	bool greenhouseEffect;  /* runaway greenhouse effect?	*/
	double boilPoint;		/* the boiling point of water (Kelvin)*/

	double albedo; /* albedo of the planet		     */

	double exosphericTemperature;		/* units of degrees Kelvin			 */
	double estimatedTemperature;		/* quick non-iterative estimate (K)  */
	double estimatedTerrainTemperature; /* for terrestrial moons and the like*/
	double surfaceTemperature;			/* surface temperature in Kelvin     */

	double greenhouseRise;  /* Temperature rise due to greenhouse */
	double highTemperature; /* Day-time temperature              */
	double lowTemperature;  /* Night-time temperature			 */
	double maxTemperature;  /* Summer/Day						 */
	double minTemperature;  /* Winter/Night						 */

	double hydrosphere; /* fraction of surface covered	     */
	double cloudCover;  /* fraction of surface covered	     */
	double iceCover;	/* fraction of surface covered	     */

	Sun *sun;

	int gases; /* Count of gases in the atmosphere: */
	gas *atmosphere;

	planetType planetClass;

	int minor_moons;
	planetPointer first_moon;

	planetPointer nextPlanet;

} planets;

typedef struct dustRecord
{
	double innerEdge;
	double outerEdge;
	bool dustPresent;
	bool gasPresent;
	dustPointer nextBand;

} dust;

#endif // !AC_SRUCTS_H